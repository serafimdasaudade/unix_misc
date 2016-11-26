#include <stdio.h>
#include <string.h>
#include "scard.h"

BOOL Context_Created=0;
void scard_shutdown(FILE *fplog,SCARDCONTEXT *hContext);

struct calypsoAtributes  cAtr;
struct calypsoFCI  cFCI;
struct cardEnvironment cEnv;
struct cardContracts cContracts;

/*
extern const unsigned char CALYPSO_APPLICATION[];
extern const unsigned char ENVIRONMENT_FILE[];
extern const unsigned char CONTRACTS_FILE[];
extern const unsigned char COUNTERS_FILE[];
extern const unsigned char EVENTS_FILE[];
extern const unsigned char SPECIAL_EVENT_FILE[];
extern const unsigned char CONTRACT_LIST_FILE[];
*/
const unsigned char CALYPSO_APPLICATION[] = "1TIC.ICA";
const unsigned char ENVIRONMENT_FILE[]    = { 0x20, 0x01 ,0x00};
const unsigned char CONTRACTS_FILE[]      = { 0x20, 0x20 ,0x00};
const unsigned char COUNTERS_FILE[]       = { 0x20, 0x69 ,0x00};
const unsigned char EVENTS_FILE[]         = { 0x20, 0x10 ,0x00};
const unsigned char SPECIAL_EVENT_FILE[]  = { 0x20, 0x40 ,0x00};
const unsigned char CONTRACT_LIST_FILE[]   = { 0x20, 0x50 ,0x00};

void CalypsoParseSelAppResponse(FILE *fplog,BYTE Resp[], int RespLen);
void CalypsoParseEnvAndHolderData(FILE *fplog,BYTE Resp[], int RespLen);


const char* iso7816_stringify_sw(unsigned short sw);



int get_file_from_card(FILE *fplog,SCARDHANDLE hCard,char *card_file_name,const BYTE card_file_num[],int size_cf_fnum, int record_num, BYTE r_apdu[], int sizeof_r_apdu,DWORD *r_len)
{
	LONG rc;

	BYTE   c_apdu[128];			 /* Buffer to pass the command to the card */
	DWORD  c_length;
	DWORD  r_length;

	*r_len=0;
	r_length=sizeof_r_apdu;

	/* This is an ISO 7816-4 SELECT APDU with the file number as data */
	c_length = 0;
	c_apdu[c_length++] = 0x94;	 /* CLA - we use 0x94 for compliance with legacy cards */
	c_apdu[c_length++] = 0xA4;	 /* INS */
	c_apdu[c_length++] = 0x02;	 /* P1  */
	c_apdu[c_length++] = 0x00;	 /* P2  */
								 /* Lc */
	c_apdu[c_length++] = size_cf_fnum;
	//printf("CS: c_length=%d\n",(int)c_length);

	//printf("hCard=%d c_apdu=%d\n",sizeof(hCard), sizeof (c_apdu));
	//printf("hCard=%04x c_apdu=",(unsigned int)hCard);
	//for (i=0;i<sizeof(c_apdu);i++) printf("%02x",(unsigned int)c_apdu[i]);


	memcpy(&c_apdu[c_length], card_file_num, size_cf_fnum);
	c_length += size_cf_fnum;

	//printf("CS b1=%x b2=%x\n",*card_file_num,*(card_file_num+1));
	//printf("CS r_length=%d c_length=%d sizef=%d\n",(int)r_length,(int)c_length,size_cf_fnum);
	rc = SCardTransmit(hCard, SCARD_PCI_T1, c_apdu, c_length, NULL, r_apdu, &r_length);
	if (rc != SCARD_S_SUCCESS)
	{
                log_printf(fplog,LOG_ERROR,"SC: SCardTransmit(SELECT Application) error %08lX", rc);
		return(3);
	}

	if (r_length<2)
	{
                log_printf(fplog,LOG_ERROR,"SC: Response too small");
		return(31);
	}

	/* Check that we got SW=9000 */
	if ((r_apdu[r_length-2] != 0x90) || (r_apdu[r_length-1] != 0x00))
	{

                log_printf(fplog,LOG_WARNING,"SC: SCardTransmit(SELECT %s) -> SW=%02X%02X",card_file_name, r_apdu[r_length-2], r_apdu[r_length-1]);
	}

	/* Read record of the file */
	/* ------------------------------------- */

	/* This is an ISO 7816-4 READ RECORD APDU  */
	c_length = 0;
	c_apdu[c_length++] = 0x94;	 /* CLA - we use 0x94 for compliance with legacy cards*/
	c_apdu[c_length++] = 0xB2;	 /* INS */
	c_apdu[c_length++] = record_num; /* P1 = record number */
	c_apdu[c_length++] = 0x04;	 /* P2  */
	c_apdu[c_length++] = 0x00;	 /* Le */
	/* CS quando cartao definitivo ver se posso 
		logo por Le certo ficara mais rapido*/

	r_length=sizeof_r_apdu;
	rc = SCardTransmit(hCard, SCARD_PCI_T1, c_apdu, c_length, NULL, r_apdu, &r_length);
	if (rc != SCARD_S_SUCCESS)
	{
                log_printf(fplog,LOG_ERROR,"SC: SCardTransmit(READ RECORD %d from %s) error %08lX",record_num,card_file_name, rc);
		return(4);
	}

	/* If SW=6700 we should provide Le=19 (legacy cards) */
	if ((r_apdu[r_length-2] == 0x67) && (r_apdu[r_length-1] == 0x00))
	{
		log_printf(fplog,LOG_WARNING,"SC: Fixing Le. Received 0x6700.");
		c_apdu[c_length-1] = 19; /* Le */

		r_length=sizeof_r_apdu;
		rc = SCardTransmit(hCard, SCARD_PCI_T1, c_apdu, c_length, NULL, r_apdu, &r_length);
		if (rc != SCARD_S_SUCCESS)
		{
                        log_printf(fplog,LOG_ERROR,"SC: SCardTransmit(SW=6700)(READ RECORD %d from %s) error %08lX", record_num, card_file_name, rc);
			return(5);
		}
	}

	if ((r_apdu[r_length-2] == 0x6C) )
	{
		/* Wrong Le lets fix and try again */
		BYTE sw2;
		sw2=r_apdu[r_length-1];
                log_printf(fplog,LOG_WARNING,"SC: Fixing Le. Received 0x6C%x.",sw2);
		c_apdu[c_length-1] = sw2;

		r_length=sizeof_r_apdu;
		rc = SCardTransmit(hCard, SCARD_PCI_T1, c_apdu, c_length, NULL, r_apdu, &r_length);
		if (rc != SCARD_S_SUCCESS)
		{
                        log_printf(fplog,LOG_ERROR,"SC: SCardTransmit(SW=6C)(READ RECORD %d from %s) error %08lX", record_num, card_file_name, rc);
			return(15);
		}
	}

	/* Check that we got SW=9000 */
	if ((r_apdu[r_length-2] == 0x6A) && (r_apdu[r_length-1] == 0x83))
	{
                log_printf(fplog,LOG_WARNING,"SC: SCardTransmit(READ RECORD %d from %s file) -> Does not exist",record_num, card_file_name  );
		return(20);
	}
	if ((r_apdu[r_length-2] != 0x90) || (r_apdu[r_length-1] != 0x00))
	{
                log_printf(fplog,LOG_WARNING,"SC: SCardTransmit(READ RECORD %d from %s file) -> SW=%02X%02X - %s",record_num, card_file_name,  r_apdu[r_length-2], r_apdu[r_length-1],iso7816_stringify_sw(r_apdu[r_length-2]));
	}
	*r_len=r_length;
	return (0);
}

int scard_read_all_mifare(FILE *fplog,char *reader_used )
{
	LONG    rc;
	SCARDCONTEXT    hContext;

	memset((void*)&cAtr,0,sizeof(cAtr));
	memset((void*)&cFCI,0,sizeof(cFCI));
	memset((void*)&cEnv,0,sizeof(cEnv));
	memset((void*)&cContracts,0,sizeof(cContracts));

	rc=scard_startup(fplog,&hContext);
	if (rc)
	{
		log_printf(fplog,LOG_ERROR,"SC: scard_startup: Failed. rc=%ld\n",rc);
		rc=1;
		goto leave;
	}
	rc=scard_find_first_reader_with_a_card(fplog,hContext,reader_used);
	if (rc)
	{
		if (rc==2) rc=5;
		else rc=2;
		goto leave;
	}

	rc=scard_get_mifare(fplog,hContext,reader_used,(unsigned char*)"",0);
	if (rc)
	{
		log_printf(fplog,LOG_ERROR,"SC: scard_get: Failed. rc=%ld\n",rc);
		rc=3;
		goto leave;
	}
	rc=0;
leave:
	(void)scard_shutdown(fplog,&hContext);
	return (rc);
}
int scard_get_mifare(FILE *fplog,SCARDCONTEXT hContext,const char *szReader, const unsigned char *file, int record )
{
	SCARDHANDLE hCard;
	DWORD dwProtocol;
	LONG rc;

	BYTE   c_apdu[128];	 /* Buffer to pass the command to the card */
	BYTE   r_apdu[128];	 /* Buffer to receive the response */
	DWORD  c_length;
	DWORD  r_length;

	/* Connect to the card */
	rc = SCardConnect(hContext, szReader, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T1, &hCard, &dwProtocol);
	if (rc != SCARD_S_SUCCESS)
	{
                log_printf(fplog,LOG_ERROR,"SC: SCardConnect error %08lX", rc);
		return 1;
	}
	//goto cs_cont;

	c_length = 0;
	c_apdu[c_length++] = 0xFF;	 
	c_apdu[c_length++] = 0x41;	 
	c_apdu[c_length++] = 0x00;	 
	c_apdu[c_length++] = 0x00;	 

	r_length=sizeof(r_apdu);
	rc = SCardTransmit(hCard, SCARD_PCI_T1, c_apdu, c_length, NULL, r_apdu, &r_length);
	if (rc != SCARD_S_SUCCESS)
	{
                log_printf(fplog,LOG_ERROR,"SC: SCardTransmit(Mifare) error %08lX", rc);
		rc=2;
		goto failed;
	}
        log_printf_hex_title(fplog,LOG_DEBUG,"SC: MIFARE",r_apdu,r_length);

	SCardDisconnect(hCard, SCARD_RESET_CARD);
	return (rc);

failed:
	SCardDisconnect(hCard, SCARD_RESET_CARD);
	return (rc);
}
int scard_get(FILE *fplog,SCARDCONTEXT hContext,const char *szReader, const unsigned char *file, int record )
{
	SCARDHANDLE hCard;
	DWORD dwProtocol;
	LONG rc;

	BYTE   c_apdu[128];	 /* Buffer to pass the command to the card */
	BYTE   r_apdu[128];	 /* Buffer to receive the response */
	DWORD  c_length;
	DWORD  r_length;

	//DWORD  i;
	char strtemp[1000];

	/* Connect to the card */
	rc = SCardConnect(hContext, szReader, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T1, &hCard, &dwProtocol);
	if (rc != SCARD_S_SUCCESS)
	{
                log_printf(fplog,LOG_ERROR,"SC: SCardConnect error %08lX", rc);
		return 1;
	}
	//goto cs_cont;

	/* Select the Calypso application within the card */
	/* ---------------------------------------------- */

	/* This is an ISO 7816-4 SELECT APDU with the CALYPSO_APPLICATION as data */
	c_length = 0;
	c_apdu[c_length++] = 0x94;	 /* CLA - we use 0x94 for compliance with legacy cards*/
	c_apdu[c_length++] = 0xA4;	 /* INS */
	c_apdu[c_length++] = 0x04;	 /* P1  */
	c_apdu[c_length++] = 0x00;	 /* P2  */
								 /* Lc */
	c_apdu[c_length++] = strlen((char*)CALYPSO_APPLICATION);
	memcpy(&c_apdu[c_length], CALYPSO_APPLICATION, strlen((char*)CALYPSO_APPLICATION));
	c_length += strlen((char*)CALYPSO_APPLICATION);

	r_length=sizeof(r_apdu);
	rc = SCardTransmit(hCard, SCARD_PCI_T1, c_apdu, c_length, NULL, r_apdu, &r_length);
	if (rc != SCARD_S_SUCCESS)
	{
                log_printf(fplog,LOG_ERROR,"SC: SCardTransmit(SELECT CALYPSO_APPLICATION) error %08lX", rc);
		rc=2;
		goto failed;
	}

	if (r_length<2)
	{
                log_printf(fplog,LOG_ERROR,"SC: Response too small");
		return(31);
	}


	/* Check that we got SW=9000 */
	if ((r_apdu[r_length-2] != 0x90) || (r_apdu[r_length-1] != 0x00))
	{
		log_printf(fplog,LOG_WARNING,"SC: SCardTransmit(SELECT CALYPSO_APPLICATION) -> SW=%02X%02X", r_apdu[r_length-2], r_apdu[r_length-1]);
	}
	log_printf(fplog,LOG_DEBUG,"SC: APPLIC\t%s", CALYPSO_APPLICATION);

	/* Parse the FCI returned by the application */
	/* ----------------------------------------- */

	if (r_length > 2)
	{
		cFCI.raw_len=r_length-2;
		memcpy(cFCI.raw,r_apdu,cFCI.raw_len);
                log_printf_hex_title(fplog,LOG_DEBUG,"SC: FCI",cFCI.raw,cFCI.raw_len);
	}
	if (strlen((char*)file))
	{
                rc=get_file_from_card(fplog,hCard,"?????????",file,strlen((char*)file),record,r_apdu,sizeof(r_apdu),&r_length);
		if (rc==20)
		{
			/*This contract does not exist */
			rc=0;
		}
		else if (rc==0)
		{
			sprintf(strtemp,"SC: CONTRACT %d",(int)record);
			log_printf_hex_title(fplog,LOG_INFO,strtemp,r_apdu, r_length-2);
			cContracts.contract[record].raw_len=r_length-2;
			memcpy(cContracts.contract[record].raw,r_apdu,r_length-2);
		}
		else if (rc)
		{
			log_printf(fplog,LOG_ERROR,"SC: Error reading contracts file rec ?? rc=%d", rc);
			rc=6;
			goto failed;
		}
	}
	else
	{
		int i;
		rc=get_file_from_card(fplog,hCard,"Contracts",CONTRACTS_FILE,strlen((char*)CONTRACTS_FILE),2,r_apdu,sizeof(r_apdu),&r_length);
		rc=get_file_from_card(fplog,hCard,"Environment",ENVIRONMENT_FILE, strlen((char*)ENVIRONMENT_FILE),1,r_apdu,sizeof(r_apdu),&r_length);
		if (rc)
		{
			log_printf(fplog,LOG_ERROR,"SC: Error reading environment file", rc);
			rc=5;
			goto failed;
		}
		log_printf_hex_title(fplog,LOG_INFO,"SC: ENV_FILE",r_apdu, r_length-2);
		CalypsoParseEnvAndHolderData(fplog,r_apdu, (int) (r_length ));

		for (i=1;i<=10;i++)
		{
			rc=get_file_from_card(fplog,hCard,"Contracts",CONTRACTS_FILE,strlen((char*)CONTRACTS_FILE),i,r_apdu,sizeof(r_apdu),&r_length);
			if (rc==20)
			{
				/*This contract does not exist */
				rc=0;
			}
			else if (rc==0)
			{
				sprintf(strtemp,"SC: CONTRACT %d",(int)i);
				log_printf_hex_title(fplog,LOG_DEBUG,strtemp,r_apdu, r_length-2);
				cContracts.contract[i].raw_len=r_length-2;
				memcpy(cContracts.contract[i].raw,r_apdu,r_length-2);
			}
			else if (rc)
			{
				log_printf(fplog,LOG_ERROR,"SC: Error reading contracts file rec %d rc=%d", i, rc);
				rc=6;
				goto failed;
			}
		}
	}

	SCardDisconnect(hCard, SCARD_RESET_CARD);
	return (rc);

failed:
	SCardDisconnect(hCard, SCARD_RESET_CARD);
	return (rc);
}

int scard_startup(FILE *fplog,SCARDCONTEXT *hContext)
{

	LONG   rc;

	/*
	 * Get a handle to the PC/SC resource manager
	 */

	if ( Context_Created) 
	{
		log_printf(fplog,LOG_ERROR,"SC: SCardEstablishContext Context already exists");
		rc=6;
		goto leave;
	}

	Context_Created=0;
	rc = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, hContext);
	if(rc != SCARD_S_SUCCESS)
	{
		log_printf(fplog,LOG_ERROR,"SC: SCardEstablishContext error %08lX", rc);
		rc=1;
		goto leave;
	}
	log_printf(fplog,LOG_DEBUG2,"SC: CONTEXT captured");
	Context_Created=1;
	rc=0;
leave:
	return (rc);

}

int scard_find_first_reader_with_a_card(FILE *fplog, SCARDCONTEXT hContext, char *reader_used)
{
	SCARD_READERSTATE rgscState;
	LONG rc=-1;

	LPTSTR *pReader;
	LPSTR *szReaders ;
	DWORD *dwReadersSz;

	LPSTR _szReaders = NULL;
	DWORD _dwReadersSz;
	LPTSTR _pReader;

	int mem_allocated=0;

	pReader     = &_pReader;
	szReaders   = &_szReaders;
	dwReadersSz = &_dwReadersSz;

	/*
	 * Get the list of available readers
	 */
	*dwReadersSz = SCARD_AUTOALLOCATE;

	log_printf(fplog,LOG_DEBUG2,"SC: Bef cardList 0 szReaders %ld %ld",szReaders,*szReaders);
	rc = SCardListReaders(hContext,
		NULL,					 /* Any group */
		(LPTSTR) szReaders,	 /* Diabolic cast for buffer auto-allocation */
		dwReadersSz);			 /* Beg for auto-allocation */

	if (*szReaders!=NULL)
	{
		mem_allocated=1;
		log_printf(fplog,LOG_DEBUG2,"SC: ALLOC szReaders");
	}

	log_printf(fplog,LOG_DEBUG2,"SC: aft cardList szReaders %ld %ld",szReaders,*szReaders);

	if (rc == SCARD_E_NO_READERS_AVAILABLE)
	{
		/* No reader at all */
		log_printf(fplog,LOG_ERROR,"SC: Find First-No PC/SC reader");
		rc=2;
		goto leave;
	}

	if (rc != SCARD_S_SUCCESS)
	{
		log_printf(fplog,LOG_ERROR,"SC: SCardListReaders error %08lX",rc);
		rc=3;
		goto leave;
	}
	/* On success pReader has a working reader with a card */

	/*
	 * Loop withing reader(s) to find a card
	 */
	log_printf(fplog,LOG_DEBUG2,"SC: DEB pReader %ld",pReader);
	log_printf(fplog,LOG_DEBUG2,"SC: DEB szReaders %ld %ld",szReaders,*szReaders);
	log_printf(fplog,LOG_DEBUG2,"SC: DEB sizeof reader(%d) szR(%d)",sizeof(pReader),sizeof(szReaders));
	log_printf(fplog,LOG_DEBUG2,"SC: DEB sizeof *reader(%d) *szR(%d)",sizeof(*pReader),sizeof(*szReaders));
	*pReader = *szReaders;

	/**
	 * LS - change this on 16-10-2013
	 */
	//while (**pReader != '\0')
	{
		/* Got a reader name */
		/* Get status */
		rgscState.szReader = *pReader;
		rgscState.dwCurrentState = SCARD_STATE_UNAWARE;
		rc = SCardGetStatusChange(hContext, 0, &rgscState, 1);

		if (rc != SCARD_S_SUCCESS)
		{
			log_printf(fplog,LOG_ERROR,"SC: SCardGetStatusChange error %08lX", rc);
			rc=4;
			goto leave;
		}

		/* Show reader's status */
		if (rgscState.dwEventState & SCARD_STATE_IGNORE)
			log_printf(fplog,LOG_WARNING,"SC: Ignore this reader");

		if (rgscState.dwEventState & SCARD_STATE_UNKNOWN)
			log_printf(fplog,LOG_WARNING,"SC: Reader unknown");

		if (rgscState.dwEventState & SCARD_STATE_UNAVAILABLE)
			log_printf(fplog,LOG_WARNING,"SC: Status unavailable");

		if (rgscState.dwEventState & SCARD_STATE_EMPTY)
			log_printf(fplog,LOG_DEBUG,"SC: No card in: %s",*pReader);

		if (rgscState.dwEventState & SCARD_STATE_PRESENT)
			log_printf(fplog,LOG_DEBUG,"SC: READER\t%s",*pReader);

		/* Show ATR (if some) */
		if (rgscState.cbAtr)
		{
			cAtr.raw_len=rgscState.cbAtr;
			memcpy(cAtr.raw,rgscState.rgbAtr,rgscState.cbAtr);
			log_printf_hex_title(fplog,LOG_DEBUG,"SC: ATRIB",cAtr.raw,cAtr.raw_len);
		}

		/* Show card's status */
		if (rgscState.dwEventState & SCARD_STATE_ATRMATCH)
			log_printf(fplog,LOG_DEBUG,"SC: \tATR match");

		if (rgscState.dwEventState & SCARD_STATE_INUSE)
			log_printf(fplog,LOG_DEBUG,"SC: \tCard (or reader) in use");

		if (rgscState.dwEventState & SCARD_STATE_EXCLUSIVE)
			log_printf(fplog,LOG_DEBUG,"SC: \tCard (or reader) reserved for exclusive use");

		if (rgscState.dwEventState & SCARD_STATE_MUTE)
			log_printf(fplog,LOG_DEBUG,"SC: \tCard is mute");

		if ( (rgscState.dwEventState & SCARD_STATE_PRESENT)
			&& !(rgscState.dwEventState & SCARD_STATE_MUTE)
			&& !(rgscState.dwEventState & SCARD_STATE_INUSE)
			&& !(rgscState.dwEventState & SCARD_STATE_EXCLUSIVE))
		{
			/* OK this card looks good lets use it */

			rc=0;
			strncpy(reader_used, *pReader,strlen(reader_used));
			log_printf(fplog,LOG_DEBUG,"SC: LS: reader_used=%ld",reader_used);
			goto leave;
		}

		/* Jump to next entry in multi-string array */
		*pReader += strlen(*pReader) + 1;
	}

	rc=5; // Nothing found
leave:

	if (mem_allocated)
	{
		/* Free the list of readers  */
		if (szReaders != NULL)
		{
			log_printf(fplog,LOG_DEBUG2,"SC: FREE szReaders");
			SCardFreeMemory(hContext, *szReaders);
			szReaders = NULL;
		}
	}
	return(rc);
}

void scard_shutdown(FILE *fplog,SCARDCONTEXT *hContext)
{
	if (Context_Created)
	{
		log_printf(fplog,LOG_DEBUG,"SC: CONTEXT released");
		Context_Created=0;
		SCardReleaseContext(*hContext);
	}
}


/*
 * TLVLoop
 * --------
 * A really (too much ?) simple ASN1 T,L,V parser
 */
BOOL TLVLoop(BYTE buffer[], int *offset, WORD *tag, int *length, BYTE *value[])
{
	WORD t;
	int o, l;

	if (buffer == NULL)
		return MC_FALSE;

	if (offset != NULL)
		o = *offset;
	else
		o = 0;

	if ((buffer[o] == 0x00) || (buffer[o] == 0xFF))
		return MC_FALSE;

	/* Read the tag */
	if ((buffer[o] & 0x1F) != 0x1F)
	{
		/* Short tag */
		t = buffer[o++];
	} else
	{
		/* Long tag */
		t = buffer[o++];
		t <<= 8;
		t |= buffer[o++];
	}

	if (tag != NULL)
		*tag = t;

	/* Read the length */
	if (buffer[o] & 0x80)
	{
		/* Multi-byte lenght */
		switch (buffer[o++] & 0x7F)
		{
			case 0x01:
				l = buffer[o++];
				break;
			case 0x02:
				l = buffer[o++];
				l <<= 8;
				l += buffer[o++];
				break;
			default:
				return MC_FALSE;
		}
	} else
	{
		/* Length on a single byte */
		l = buffer[o++];
	}

	if (l > 65535)
		return MC_FALSE;			 /* Overflow */

	if (length != NULL)
		*length = (WORD) l;

	/* Get a pointer on data */
	if (value != NULL)
		*value = &buffer[o];

	/* Jump to the end of data */
	o += l;

	if (offset != NULL)
		*offset = (WORD) o;

	return 1;
}


/*
 * Parse Calypso FCI Discretionary Data
 */
void CalypsoParseFCIDisc(FILE *fplog,BYTE FciDisc[], int FciDiscLen)
{
	int offset, length;
	WORD tag;
	BYTE *value;

	/* Parse the TLV structure */
	offset = 0;
	while ((offset < FciDiscLen) && TLVLoop(FciDisc, &offset, &tag, &length, &value))
	{
		if (tag == 0xC7)
		{
			memcpy(cFCI.serial_num,value,length);
		} else
		if (tag == 0x53)
		{
			cFCI.session_max_mods=value[0];
			cFCI.platform=value[1];
			cFCI.type=value[2];
			cFCI.subtype=value[3];
			cFCI.softissuer=value[4];
			cFCI.softversion=value[5];
			cFCI.softrevision=value[6];
		} else
		{
			log_printf(fplog,LOG_ERROR,"SC: FCI unhandled %s[%i]",__FILE__,__LINE__);
			log_printf(fplog,LOG_INFO,"SC: Tag %04X, length=%d (unhandled)", tag, length);
		}
	}
}


/*
 * Parse Calypso FCI Proprietary Data
 */
void CalypsoParseFCIProp(FILE *fplog,BYTE FciProp[], int FciPropLen)
{
	int offset, length;
	WORD tag;
	BYTE *value;

	/* Parse the TLV structure */
	offset = 0;
	while ((offset < FciPropLen) && TLVLoop(FciProp, &offset, &tag, &length, &value))
	{
		if (tag == 0xBF0C)
		{
			/* This is the FCI Issuer Discreationary template */
			CalypsoParseFCIDisc(fplog,value, length);
		} else
		{
			log_printf(fplog,LOG_ERROR,"SC: FCI unhandled %s[%i]",__FILE__,__LINE__);
			log_printf(fplog,LOG_INFO,"SC: Tag %04X, length=%d (unhandled)", tag, length);
		}
	}
}


/*
 * Parse Calypso FCI
 */


void CalypsoParseFCI(FILE *fplog,BYTE Fci[], int FciLen)
{
	int offset, length;
	WORD tag;
	BYTE *value;

	/* Parse the TLV structure */
	offset = 0;
	while ((offset < FciLen) && TLVLoop(Fci, &offset, &tag, &length, &value))
	{
		if (tag == 0x84)
		{
			memcpy(cFCI.df_name,value,length);
		} else
		if (tag == 0xA5)
		{
			/* This is the FCI Proprietary template */
			CalypsoParseFCIProp(fplog,value, length);
		} else
		{
			log_printf(fplog,LOG_ERROR,"SC: FCI unhandled %s[%i]",__FILE__,__LINE__);
			log_printf(fplog,LOG_INFO,"SC: Tag %04X, length=%d (unhandled)", tag, length);
		}
	}
}


/*
 * Parse Calypso response to select application (must be a FCI)
 */
void CalypsoParseSelAppResponse(FILE *fplog,BYTE Resp[], int RespLen)
{
	int offset, length;
	WORD tag;
	BYTE *value;

	/* Parse the TLV structure */
	offset = 0;
	while ((offset < RespLen) && TLVLoop(Resp, &offset, &tag, &length, &value))
	{
		if (tag == 0x6F)
		{
			/* This is the FCI template */
			CalypsoParseFCI(fplog,value, length);
		} else
		{
			log_printf(fplog,LOG_ERROR,"SC: FCI unhandled %s[%i]",__FILE__,__LINE__);
			log_printf(fplog,LOG_INFO,"SC: Tag %04X, length=%d (unhandled)", tag, length);
		}
	}

}


/*
 * Data within the records of a Calypso cards are not aligned on byte boundaries, so
 * we must be able to get then bit by bit
 */
unsigned short GetBits(unsigned char buffer[], unsigned int *offset, unsigned char count)
{
	unsigned char i;
	unsigned short r = 0;

	if (count > 16) return r;

	for (i=0; i<count; i++)
	{
		unsigned int byte_offset = *offset / 8;
		unsigned int bit_offset  = 7 - (*offset % 8);
		unsigned char mask = 1 << bit_offset;

		r <<= 1;
		r |= (buffer[byte_offset] & mask) ? 1 : 0;

		*offset = *offset + 1;
	}

	return r;
}


/*
 * Minimalistic parser of the EnvironmentAndHolder file
 */
void CalypsoParseEnvAndHolderData(FILE *fplog,BYTE Resp[], int RespLen)
{
	unsigned int offset = 0;

	memcpy(cEnv.raw,Resp,RespLen);
	cEnv.raw_len=RespLen;

	cEnv.version         = (unsigned char) GetBits(Resp, &offset, 6);
	log_printf(fplog,LOG_DEBUG,"SC: \t\tenvVersionNumber       = %d", cEnv.version);

	cEnv.bitmap          = (unsigned char) GetBits(Resp, &offset, 7);
	log_printf(fplog,LOG_DEBUG,"SC: \t\tbitmap=%02X", cEnv.bitmap);

	if (cEnv.bitmap & 0x01)
	{
		cEnv.network_country = GetBits(Resp, &offset, 12);
		cEnv.network_ident   = GetBits(Resp, &offset, 12);
		log_printf(fplog,LOG_DEBUG,"SC: \t\tenvNetworkId           = %03X %03X", cEnv.network_country, cEnv.network_ident);
	}
	if (cEnv.bitmap & 0x02)
	{
		cEnv.app_issuer      = (unsigned char) GetBits(Resp, &offset, 8);
		log_printf(fplog,LOG_DEBUG,"SC: \t\tenvApplicationIssuerId = %02X", cEnv.app_issuer);
	}
	if (cEnv.bitmap & 0x04)
	{
		cEnv.end_date        = GetBits(Resp, &offset, 14);
		log_printf(fplog,LOG_DEBUG,"SC: \t\tenvApplicationEndDate  = %d", cEnv.end_date);
	}
}

const char* iso7816_stringify_sw(unsigned short sw)
{
  static char msg[200];

  msg[0]=0;

  if (sw==0x9000)
    return strcpy(msg,"Normal processing");

  switch (sw>>8) {
    case 0x61: 
      strcpy(msg,"More bytes available (see SW2)");
      break;
    case 0x62: 
      strcpy(msg,"State of non-volatile memory unchanged - ");
      switch (sw&0xFF) {
	case 0x00: strcat(msg,"No information given"); break;
	case 0x81: strcat(msg,"Part of returned data may be corrupted"); break;
	case 0x82: strcat(msg,"End of file/record reached before reading Le bytes"); break;
	case 0x83: strcat(msg,"Selected file invalidated"); break;
	case 0x84: strcat(msg,"FCI not formatted correctly"); break;
      }
      break;
    case 0x63:
      strcpy(msg,"State of non-volatile memory changed - ");
      switch (sw&0xFF) {
	case 0x00: strcat(msg,"No information given"); break;
	case 0x81: strcat(msg,"File filled up by the last write"); break;
      }
      if ((sw&0xF0)==0xC0) strcat(msg,"Counter value");
      break;
    case 0x64:
      strcpy(msg,"State of non-volatile memory unchanged - ");
      if ((sw&0xFF)==0) strcat(msg,"OK");
      break;
    case 0x65:
      strcpy(msg,"State of non-volatile memory changed - ");
      switch (sw&0xFF) {
	case 0x00: strcat(msg,"No information given"); break;
	case 0x81: strcat(msg,"Memory failure"); break;
      }
      break;
    case 0x66:
      strcpy(msg,"security-related issue - ");
      switch (sw&0xFF) {
	case 0x00: strcat(msg,"No information given"); break;
      }
      break;
    case 0x67:
      if (sw==0x6700) 
	strcpy(msg,"Wrong length");
      else
	strcpy(msg,"Unknown 67XX error code");
      break;
    case 0x68:
      strcpy(msg,"Functions in CLA not supported - ");
      switch (sw&0xFF) {
	case 0x00: strcat(msg,"No information given"); break;
	case 0x81: strcat(msg,"Logical channel not supported"); break;
	case 0x82: strcat(msg,"Secure messaging not supported"); break;
      }
      break;
    case 0x69:
      strcpy(msg,"Command not allowed - ");
      switch (sw&0xFF) {
	case 0x00: strcat(msg,"No information given"); break;
	case 0x81: strcat(msg,"Command incompatible with file structure"); break;
	case 0x82: strcat(msg,"Security status not satisfied"); break;
	case 0x83: strcat(msg,"Authentication method blocked"); break;
	case 0x84: strcat(msg,"Referenced data invalidated"); break;
	case 0x85: strcat(msg,"Conditions of use not satisfied"); break;
	case 0x86: strcat(msg,"Command not allowed (no current EF)"); break;
	case 0x87: strcat(msg,"Expected SM data objects missing"); break;
	case 0x88: strcat(msg,"SM data objects incorrect"); break;
      }
      break;
    case 0x6A:
      strcpy(msg,"Wrong parameter(s) P1-P2 - ");
      switch (sw&0xFF) {
	case 0x00: strcat(msg,"No information given"); break;
	case 0x80: strcat(msg,"Incorrect parameters in the data field"); break;
	case 0x81: strcat(msg,"Function not supported"); break;
	case 0x82: strcat(msg,"File not found"); break;
	case 0x83: strcat(msg,"Record not found"); break;
	case 0x84: strcat(msg,"Not enough memory space in the file"); break;
	case 0x85: strcat(msg,"Lc inconsistent with TLV structure"); break;
	case 0x86: strcat(msg,"Incorrect parameters P1-P2"); break;
	case 0x87: strcat(msg,"Lc inconsistent with P1-P2"); break;
	case 0x88: strcat(msg,"Referenced data not found"); break;
      }
      break;
    case 0x6B:
      if (sw==0x6B00)
        strcpy(msg,"Wrong parameter(s) P1-P2");
      else
        strcpy(msg,"Unknown 6BXX error code");
      break;
    case 0x6C:
      strcpy(msg,"Wrong length Le, see SW2");
      break;
    case 0x6D:
      if (sw==0x6D00)
        strcpy(msg,"Instruction code not supported or invalid");
      else
        strcpy(msg,"Unknown 6DXX error code");
      break;
    case 0x6E:
      if (sw==0x6E00)
        strcpy(msg,"Class not supported");
      else
        strcpy(msg,"Unknown 6EXX error code");
      break;
    case 0x6F:
      if (sw==0x6FFF)
	strcpy(msg,"Cardpeek application-level error");
      else
	strcpy(msg,"No precise diagnosis");
      break;
    default:
      strcpy(msg,"** Unkown error code **");
  }
  return msg;
}

int scard_read_all(FILE *fplog,char *reader_used )
{
	LONG    rc;
	SCARDCONTEXT    hContext;

	memset((void*)&cAtr,0,sizeof(cAtr));
	memset((void*)&cFCI,0,sizeof(cFCI));
	memset((void*)&cEnv,0,sizeof(cEnv));
	memset((void*)&cContracts,0,sizeof(cContracts));

	rc=scard_startup(fplog,&hContext);
	if (rc)
	{
		log_printf(fplog,LOG_ERROR,"SC: scard_startup: Failed. rc=%ld\n",rc);
		rc=1;
		goto leave;
	}
	rc=scard_find_first_reader_with_a_card(fplog,hContext,reader_used);
	if (rc)
	{
		if (rc==2) rc=5;
		else rc=2;
		goto leave;
	}

	rc=scard_get(fplog,hContext,(char*)reader_used,(unsigned char*)"",0);
	if (rc)
	{
		log_printf(fplog,LOG_ERROR,"SC: scard_get: Failed. rc=%ld\n",rc);
		rc=3;
		goto leave;
	}
	rc=0;
leave:
	(void)scard_shutdown(fplog,&hContext);;
	return (rc);
}

int scard_read_file_record(FILE *fplog,const unsigned char *file, int record, char *reader_used )
{
	LONG    rc;
	SCARDCONTEXT    hContext;

	memset((void*)&cAtr,0,sizeof(cAtr));
	memset((void*)&cFCI,0,sizeof(cFCI));
	memset((void*)&cEnv,0,sizeof(cEnv));
	memset((void*)&cContracts,0,sizeof(cContracts));

	rc=scard_startup(fplog,&hContext);
	if (rc)
	{
		log_printf(fplog,LOG_ERROR,"SC: scard_startup: Failed. rc=%ld\n",rc);
		rc=1;
		goto leave;
	}
	rc=scard_find_first_reader_with_a_card(fplog,hContext,reader_used);
	if (rc)
	{
		if (rc==2) rc=5;
		else rc=2;
		goto leave;
	}

	rc=scard_get(fplog,hContext,reader_used,file,record);
	if (rc)
	{
		log_printf(fplog,LOG_ERROR,"SC: scard_get: Failed. rc=%ld\n",rc);
		rc=3;
		goto leave;
	}
	rc=0;
leave:
	(void)scard_shutdown(fplog,&hContext);;
	return (rc);
}

int scard_read_card_atrib(	FILE *fplog, SCARDCONTEXT hContext, unsigned char *atrib, int *atrib_len)
{
	SCARD_READERSTATE rgscState;
	LONG rc;

	DWORD _dwReadersSz;
	LPSTR _szReaders = NULL;
	LPSTR *szReaders;
	LPTSTR *pReader;
	LPTSTR _pReader;

	DWORD *dwReadersSz;

	pReader     = &_pReader;
	szReaders   = &_szReaders;
	dwReadersSz = &_dwReadersSz;

	/*
	 * Get the list of available readers
	 */
	*dwReadersSz = SCARD_AUTOALLOCATE;
	rc = SCardListReaders(hContext,
		NULL,					 /* Any group */
		(LPTSTR) szReaders,	 /* Diabolic cast for buffer auto-allocation */
		dwReadersSz);			 /* Beg for auto-allocation */

	if (rc == SCARD_E_NO_READERS_AVAILABLE)
	{
		/* No reader at all */
		log_printf(fplog,LOG_ERROR,"SC: Read atrib: No PC/SC reader");
		rc=2;
		goto leave;
	}

	if (rc != SCARD_S_SUCCESS)
	{
		log_printf(fplog,LOG_ERROR,"SC: SCardListReaders error %08lX",rc);
		rc=3;
		goto leave;
	}
	/* On success pReader has a working reader with a card */

	/*
	 * Loop withing reader(s) to find a card
	 */
	*pReader = *szReaders;
	/**
	 * LS - change this on 16-10-2013
	 * 
	 */
	while (**pReader != '\0')
	{
		/* Got a reader name */
		/* Get status */
		rgscState.szReader = *pReader;
		rgscState.dwCurrentState = SCARD_STATE_UNAWARE;
		rc = SCardGetStatusChange(hContext, 0, &rgscState, 1);

		if (rc != SCARD_S_SUCCESS)
		{
			log_printf(fplog,LOG_ERROR,"SC: SCardGetStatusChange error %08lX", rc);
			rc=4;
			goto leave;
		}

		/* Show reader's status */
		if (rgscState.dwEventState & SCARD_STATE_IGNORE)
			log_printf(fplog,LOG_WARNING,"SC: \tIgnore this reader");

		if (rgscState.dwEventState & SCARD_STATE_UNKNOWN)
			log_printf(fplog,LOG_WARNING,"SC: \tReader unknown");

		if (rgscState.dwEventState & SCARD_STATE_UNAVAILABLE)
			log_printf(fplog,LOG_WARNING,"SC: \tStatus unavailable");

		if (rgscState.dwEventState & SCARD_STATE_EMPTY)
			log_printf(fplog,LOG_WARNING,"SC: \tNo card in: %s",*pReader);

		if (rgscState.dwEventState & SCARD_STATE_PRESENT)
			log_printf(fplog,LOG_DEBUG,"SC: READER\t%s",*pReader);

		/* Show ATR (if some) */
		if (rgscState.cbAtr)
		{
			cAtr.raw_len=rgscState.cbAtr;
			memcpy(cAtr.raw,rgscState.rgbAtr,rgscState.cbAtr);
			memcpy(atrib,cAtr.raw,rgscState.cbAtr);
			*atrib_len=rgscState.cbAtr;
			rc=0;
			goto leave;
		}

		/* Jump to next entry in multi-string array */
		*pReader += strlen(*pReader) + 1;
	}


	rc=0;
leave:
	return(rc);
}

int scard_read_atrib(FILE *fplog, unsigned char *atrib , int *atrib_len)
{
	LONG    rc;
	SCARDCONTEXT    hContext;

	memset((void*)&cAtr,0,sizeof(cAtr));
	memset((void*)&cFCI,0,sizeof(cFCI));
	memset((void*)&cEnv,0,sizeof(cEnv));
	memset((void*)&cContracts,0,sizeof(cContracts));

	rc=scard_startup(fplog,&hContext);
	if (rc)
	{
		log_printf(fplog,LOG_ERROR,"SC: scard_startup: Failed. rc=%ld\n",rc);
		rc=1;
		goto leave;
	}
	rc=scard_read_card_atrib(fplog,hContext,atrib,atrib_len);
	if (rc)
	{
		log_printf(fplog,LOG_ERROR,"SC: scard_read_card_atrib: Failed. rc=%ld\n",rc);
		rc=2;
		goto leave;
	}
	rc=0;
leave:
	(void)scard_shutdown(fplog,&hContext);;
	return (rc);
}


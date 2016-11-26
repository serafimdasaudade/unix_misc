/*
 Mifare
    64 blocks of 16 bytes = 1024
    First block hidden used to save keys and internal info
    Organized in 16sectors
    1 sector = 3 blocks
    each sector has last block hidden for saving keys etc
    Usable for storage = 48 bytes per sector (768 bytes)
    Block zero is read only and contains card serial num etc

    Note can only write into Blocks 1,2,4,5,6,8,9,10,12,13,14,16,17,18,....

*/


//#include "log.h"
#include "pcsc_helpers.h"
#include "mifare_helpers.h"
#include "scard.h"
#include "mifare.h"

char stemp[1000];
BYTE data[3*16];

/* Key do Sector 15 da MOBIE*/
//unsigned char key_mobie[6]   = { 0x4D,0x6F,0x62,0x69,0x2E,0x45 };
unsigned char key_mobie[6]   = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
unsigned char key_initial[6] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };

static const BYTE MIFARE_1K_ATR[] = { 0x3B, 0x8F, 0x80, 0x01, 0x80, 0x4F, 0x0C, 0xA0, 0x00, 0x00, 0x03, 0x06, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x6A };
static const BYTE MIFARE_4K_ATR[] = { 0x3B, 0x8F, 0x80, 0x01, 0x80, 0x4F, 0x0C, 0xA0, 0x00, 0x00, 0x03, 0x06, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x69 };

#define APDU_BUFFERS_SZ 280

int MifareGetCardId(FILE *fplog, SCARDHANDLE hCard, BYTE cardid[], int *size)
{
  BYTE pbSendBuffer[APDU_BUFFERS_SZ];
  BYTE  pbRecvBuffer[APDU_BUFFERS_SZ];
  DWORD pcbRecvLength = sizeof(pbRecvBuffer);

  int rc;

  pbSendBuffer[0]  = 0xFF;
  pbSendBuffer[1]  = 0xCA;
  pbSendBuffer[2]  = 0x00;
  pbSendBuffer[3]  = 0x00;
  pbSendBuffer[4]  = 0x00;

  rc = SCardTransmit(hCard, SCARD_PCI_T1, pbSendBuffer, 5, NULL, pbRecvBuffer, &pcbRecvLength);
  if (rc != SCARD_S_SUCCESS)
  {
    log_printf(fplog,LOG_ERROR,"SCardTransmit(MIFARE_CLASSIC_get_card_id) error %lX\n", rc);
    return 1;
  }

  if (data != NULL)
  {
    log_printf(fplog,LOG_INFO,"recvd len=%d",pcbRecvLength);
    if (pcbRecvLength)
    {
      log_printf_hex(fplog,LOG_INFO,pbRecvBuffer, pcbRecvLength);
      memcpy(data, pbRecvBuffer, pcbRecvLength);
    }
  }

  return 0;
}

int MifareClassicRead(FILE *fplog, SCARDHANDLE hCard, WORD address, BYTE data[], int size, const BYTE key_value[6])
{
  BYTE pbSendBuffer[APDU_BUFFERS_SZ];
  BYTE  pbRecvBuffer[APDU_BUFFERS_SZ];
  DWORD pcbRecvLength = sizeof(pbRecvBuffer);

  LONG rc;

  if (key_value == NULL) return -1; // TODO : return an explicit error code
 
  pbSendBuffer[0]  = 0xFF;
  pbSendBuffer[1]  = 0xF3;
  pbSendBuffer[2] = (BYTE) (address >> 8);
  pbSendBuffer[3] = (BYTE) address;
  pbSendBuffer[4]  = 6; /*LC Num bytes sending below */
  memcpy(&pbSendBuffer[5], key_value, 6);
  pbSendBuffer[11] = size; /*LE bytes expected */

  rc = SCardTransmit(hCard, SCARD_PCI_T1, pbSendBuffer, 12, NULL, pbRecvBuffer, &pcbRecvLength);
  if (rc != SCARD_S_SUCCESS)
  {
    log_printf(fplog,LOG_ERROR,"SCardTransmit(MIFARE_CLASSIC_READ) error %lX\n", rc);
    return -1;
  }

  if (pcbRecvLength!=(size+2))
  	log_printf(fplog,LOG_DEBUG2,"recv_len=%ld expected=%d\n",pcbRecvLength,size+2);

  if ((pcbRecvLength != (DWORD) (2+size)) && (pcbRecvLength != 2))
  {
    log_printf(fplog,LOG_ERROR,"SCardTransmit(MIFARE_CLASSIC_READ) returned %ld bytes\n", pcbRecvLength);
    return -2;
  }

  if ((pcbRecvLength == 2) && (pbRecvBuffer[pcbRecvLength-2] == 0x69)
     && (pbRecvBuffer[pcbRecvLength-1] == 0x82))
  {
    log_printf(fplog,LOG_ERROR,"MIFARE_CLASSIC_READ, SW=%02X%02X. Security Status not satisfied.\n", pbRecvBuffer[0], pbRecvBuffer[1]);
    return -9;
  }

  if ((pcbRecvLength == 2) || (pbRecvBuffer[pcbRecvLength-2] != 0x90) || (pbRecvBuffer[pcbRecvLength-1] != 0x00))
  {
    log_printf(fplog,LOG_ERROR,"MIFARE_CLASSIC_READ failed, SW=%02X%02X\n", pbRecvBuffer[0], pbRecvBuffer[1]);
    return -3;
  }

  if (data != NULL)
    memcpy(data, pbRecvBuffer, size);

  return 0;
}

/*
rc = MifareClassicWrite(hCard, 4*15, data, sizeof(data), key);
*/

LONG MifareClassicWrite(FILE *fplog, SCARDHANDLE hCard, BYTE block, const BYTE data[], int size, const BYTE key_value[6])
{
  BYTE pbSendBuffer[APDU_BUFFERS_SZ];
  BYTE  pbRecvBuffer[APDU_BUFFERS_SZ];
  DWORD pcbRecvLength = sizeof(pbRecvBuffer);

  LONG rc;

  if (data == NULL) return -1;
  if (key_value == NULL) return -2;
  if (size > APDU_BUFFERS_SZ) return -3;
  if (size != 16 ) return -4;
 
  pbSendBuffer[0] = 0xFF;
  pbSendBuffer[1] = 0xF4;
  pbSendBuffer[2] = 0x00;
  pbSendBuffer[3] = block;
  pbSendBuffer[4] = 22;

  memcpy(&pbSendBuffer[5], data, 16);
  memcpy(&pbSendBuffer[5+16], key_value, 6);
  pbSendBuffer[27] = 0x00; //LE not used


  rc = SCardTransmit(hCard, SCARD_PCI_T1, pbSendBuffer, 5+16+6+1, NULL, pbRecvBuffer, &pcbRecvLength);
  if (rc != SCARD_S_SUCCESS)
  {
    log_printf(fplog,LOG_ERROR,"SCardTransmit(MIFARE_CLASSIC_WRITE) error %lX\n", rc);
    return rc;
  }

  if (pcbRecvLength != 2)
  {
    log_printf(fplog,LOG_ERROR,"SCardTransmit(MIFARE_CLASSIC_WRITE) returned %ld bytes\n", pcbRecvLength);
    return -5;
  }

  if ((pbRecvBuffer[0] != 0x90) || (pbRecvBuffer[1] != 0x00))
  {
    log_printf(fplog,LOG_ERROR,"MIFARE_CLASSIC_WRITE failed, SW=%02X%02X\n", pbRecvBuffer[0], pbRecvBuffer[1]);
    return -6;
  }

  return 0;
}

/* Nota: sector_data tem que ter pelo menos size =48 */
int scard_mifare_read_sector(FILE *fplog, SCARDCONTEXT hContext, const char *szReader,int sector, unsigned char *key, unsigned char *sector_data, int *sector_data_len)
{
	SCARDHANDLE hCard;
	DWORD dwProtocol;
	LONG rc;

	/* Connect to the card */
	rc = SCardConnect(hContext, szReader, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T1, &hCard, &dwProtocol);
	if (rc != SCARD_S_SUCCESS)
	{
		log_printf(fplog,LOG_ERROR,"SCardConnect error %08lX\n", rc);
		rc=1;
		goto failed_opening_card;
	}

	log_printf(fplog,LOG_DEBUG,"Going Read sector=%d",sector);

	/*Example Sector 15 Addr=4*15*/
	rc = MifareClassicRead(fplog,hCard, 4*sector, data, sizeof(data), key);
	if (rc)
	{
		if (rc==-9) rc=9;
		else rc=2;
		goto done;
	}
	sprintf(stemp,"Sector %d:",sector);
	memcpy(sector_data,data,3*16);
	*sector_data_len=3*16;
	log_printf_hex_title(fplog,LOG_DEBUG,stemp,sector_data,*sector_data_len);

	rc=0;

done:	//SCardDisconnect(hCard, SCARD_RESET_CARD);
	SCardDisconnect(hCard, SCARD_UNPOWER_CARD );
	return rc;

failed_opening_card:
	return rc;
}

int scard_mifare_write_block(FILE *fplog, SCARDCONTEXT hContext, const char *szReader,int block, unsigned char *key, unsigned char *_data, int data_len)
{
	SCARDHANDLE hCard;
	DWORD dwProtocol;
	LONG rc;

	/* Connect to the card */
	rc = SCardConnect(hContext, szReader, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T1, &hCard, &dwProtocol);
	if (rc != SCARD_S_SUCCESS)
	{
		log_printf(fplog,LOG_ERROR,"Writing SCardConnect error %08lX\n", rc);
		rc=1;
		goto failed_opening_card;
	}

	log_printf(fplog,LOG_DEBUG,"Going Write Block=%d",block);

	rc=MifareClassicWrite (fplog,hCard, block, data, data_len, key);
	if (rc)
	{
		rc=2;
		goto done;
	}
	rc=0;

done:	//SCardDisconnect(hCard, SCARD_RESET_CARD);
	SCardDisconnect(hCard, SCARD_UNPOWER_CARD );
	return rc;

failed_opening_card:
	return rc;
}

int mobie_mifare_read_sector(FILE *fplog, SCARDCONTEXT hContext, const char *szReader,int sector, unsigned char *sector_data, int *sector_data_len)
{
	int rc;
	unsigned char *key;

	if (sector==15) key=key_mobie;
	else key=key_initial;

	rc=scard_mifare_read_sector(fplog, hContext, (char*)szReader, sector, key ,sector_data, sector_data_len);

	if ((sector==15) && (rc==9))
	{
		/* key invalid tenta com key inicial, pode ser cartao novo*/
		key=key_initial;
		log_printf(fplog,LOG_DEBUG,"Going Read sector=%d with key of unsecured cards (new cards never secured)",sector);
		rc=scard_mifare_read_sector(fplog, hContext, (char*)szReader, sector, key ,sector_data, sector_data_len);
	}
	return(rc);
}

void mobie_mifare_parse_pin_from_sector_15_data(FILE *fplog, unsigned char *sector_data, int *pin)
{
	*pin=*(sector_data+8)+((*(sector_data+9))*256);
	log_printf(fplog,LOG_DEBUG,"PIN=%d",*pin);
}

void mobie_mifare_parse_cardid_from_sector_0_data(FILE *fplog, unsigned char *sector_data, unsigned char *cardid)
{
	memcpy(cardid,sector_data,16);
	sprintf(stemp,"CARD_ID=");
	log_printf_hex_title(fplog,LOG_DEBUG,stemp,cardid,16);
}

int mobie_mifare_write_pin(FILE *fplog, SCARDCONTEXT hContext, const char *szReader,unsigned int new_pin)
{

	/* Sector 15 offset=8 2bytes LSB first */

	SCARDHANDLE hCard;
	DWORD dwProtocol;
	LONG rc;
	unsigned char *key;

	key=key_mobie;

	/* Connect to the card */
	rc = SCardConnect(hContext, szReader, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T1, &hCard, &dwProtocol);
	if (rc != SCARD_S_SUCCESS)
	{
		log_printf(fplog,LOG_ERROR,"SCardConnect error %08lX\n", rc);
		rc=-1; goto failed_opening_card;
	}

	/*Sector 15 Addr=4*15*/
	rc = MifareClassicRead(fplog,hCard, 4*15, data, sizeof(data), key);
	if (rc)
	{
		if (rc==-9) //try another the initial key
		{
			log_printf(fplog,LOG_DEBUG,"Goinfg to use key of unsecured cards (new cards never secured)");
			key=key_initial;
			rc = MifareClassicRead(fplog,hCard, 4*15, data, sizeof(data), key);
			if (rc)
			{	
				log_printf(fplog,LOG_ERROR,"MifareClassicRead(1) error %08lX\n", rc);
				rc=-2; goto done;
			}
		}
		else
		{
			log_printf(fplog,LOG_ERROR,"MifareClassicRead error %08lX\n", rc);
			rc=-2; goto done;
		}
	}
	data[8]=new_pin%256;
	data[9]=new_pin/256;
	rc=MifareClassicWrite(fplog, hCard, 4*15, data, 16, key);
done:
	//SCardDisconnect(hCard, SCARD_RESET_CARD);
	SCardDisconnect(hCard, SCARD_UNPOWER_CARD );
	return rc;

failed_opening_card:
	return rc;
}


int mobie_mifare_write_data(FILE *fplog, SCARDCONTEXT hContext, const char *szReader, char sdata[4])
{

	/* Sector 15 offset=8 2bytes LSB first */

	SCARDHANDLE hCard;
	DWORD dwProtocol;
	LONG rc;
	unsigned char *key;

	key=key_mobie;

	/* Connect to the card */
	rc = SCardConnect(hContext, szReader, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T1, &hCard, &dwProtocol);
	if (rc != SCARD_S_SUCCESS)
	{
		log_printf(fplog,LOG_ERROR,"SCardConnect error %08lX\n", rc);
		rc=-1; goto failed_opening_card;
	}

	/*Sector 15 Addr=4*15*/
	rc = MifareClassicRead(fplog,hCard, 4*1, data, sizeof(data), key);
	if (rc)
	{
		if (rc==-9) //try another the initial key
		{
			log_printf(fplog,LOG_DEBUG,"Goinfg to use key of unsecured cards (new cards never secured)");
			key=key_initial;
			rc = MifareClassicRead(fplog,hCard, 4*1, data, sizeof(data), key);
			if (rc)
			{	
				log_printf(fplog,LOG_ERROR,"MifareClassicRead(1) error %08lX\n", rc);
				rc=-2; goto done;
			}
		}
		else
		{
			log_printf(fplog,LOG_ERROR,"MifareClassicRead error %08lX\n", rc);
			rc=-2; goto done;
		}
	}
	//data[8]=new_pin%256;
	//data[9]=new_pin/256;
	data[0]=sdata[0];
	data[1]=sdata[1];
	data[2]=sdata[2];
	data[3]=sdata[3];
	rc=MifareClassicWrite(fplog, hCard, 4*1, data, 16, key);
done:
	//SCardDisconnect(hCard, SCARD_RESET_CARD);
	SCardDisconnect(hCard, SCARD_UNPOWER_CARD );
	return rc;

failed_opening_card:
	return rc;
}

int mobie_mifare_write_new_id(FILE *fplog, SCARDCONTEXT hContext, const char *szReader, char *sdata)
{

	/* Sector 15 offset=8 2bytes LSB first */

	SCARDHANDLE hCard;
	DWORD dwProtocol;
	LONG rc;
	unsigned char *key;

	key=key_mobie;

	/* Connect to the card */
	rc = SCardConnect(hContext, szReader, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T1, &hCard, &dwProtocol);
	if (rc != SCARD_S_SUCCESS)
	{
		log_printf(fplog,LOG_ERROR,"SCardConnect error %08lX\n", rc);
		rc=-1; goto failed_opening_card;
	}

	/*Sector 15 Addr=4*15*/
	rc = MifareClassicRead(fplog,hCard, 4*1, data, sizeof(data), key);
	if (rc)
	{
		if (rc==-9) //try another the initial key
		{
			log_printf(fplog,LOG_DEBUG,"Goinfg to use key of unsecured cards (new cards never secured)");
			key=key_initial;
			rc = MifareClassicRead(fplog,hCard, 4*1, data, sizeof(data), key);
			if (rc)
			{	
				log_printf(fplog,LOG_ERROR,"MifareClassicRead(1) error %08lX\n", rc);
				rc=-2; goto done;
			}
		}
		else
		{
			log_printf(fplog,LOG_ERROR,"MifareClassicRead error %08lX\n", rc);
			rc=-2; goto done;
		}
	}
	//data[8]=new_pin%256;
	//data[9]=new_pin/256;
	/*data[0]=sdata[0];
	data[1]=sdata[1];
	data[2]=sdata[2];
	data[3]=sdata[3];*/
//	strcpy(data,"9E91D9BE68880400468F34D15D001910");//sdata;
	rc=MifareClassicWrite(fplog, hCard, 4*1, (BYTE *)"9E91D9BE68880400468F34D15D001910", 16, key);
done:
	//SCardDisconnect(hCard, SCARD_RESET_CARD);
	SCardDisconnect(hCard, SCARD_UNPOWER_CARD );
	return rc;

failed_opening_card:
	return rc;
}

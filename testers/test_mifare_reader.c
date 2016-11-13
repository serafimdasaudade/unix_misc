
#include "test_mifare_reader.h"

/*
extern struct calypsoAtr cAtr;
extern struct calypsoFCI  cFCI;
extern struct cardEnvironment cEnv;
*/
extern struct cardContracts  cContracts;

FILE *fplog;
char stemp[1000];

main()
{
	int rc,i;
	unsigned char response[1000];
	int response_len;
	char reader_used[1000];
	SCARDCONTEXT  hContext;
	unsigned char sdata[100];
	int sdlen;

	unsigned char card_id[16];
	int pin;

	fplog=log_start("T",LOG_DEBUG2);
while(1)
{
	rc=mc_scard_startup(fplog, &hContext);
//	if (rc) return(-1);

//	while (1)
//	{
	  
		sleep (1);

		rc=mc_scard_find_first_reader_with_a_card(fplog,hContext,reader_used);
		if (rc)
		{
			goto end;
		}

		log_printf(fplog,LOG_INFO,"READ MIFARE card \tReader Used=%ld\n",reader_used);

		int k;
		for (k=0; k<=15 ; k++)
		{
			rc=mc_mobie_mifare_read_sector(fplog, hContext, reader_used, k, sdata, &sdlen);
			if (rc)
			{
				goto end;
			}
			sprintf(stemp,"Sector %d:",k);
			log_printf_hex_title(fplog,LOG_INFO,stemp,sdata,sdlen);

			if (k==0)
			{
				mc_mobie_mifare_parse_cardid_from_sector_0_data(fplog, sdata,&card_id);
			}
			if (k==15)
			{
				//verificar se comeca por OCTAL etc
				//mc_mobie_mifare_parse_does_sector_15_look_good(fplog, sdata,&pin);
				mc_mobie_mifare_parse_pin_from_sector_15_data(fplog, sdata,&pin);
			}
		}
		log_printf(fplog,LOG_INFO,"----- Mobie data -----");
		log_printf_hex_title(fplog,LOG_INFO,"Mobie CARD_ID=",card_id,16);
		log_printf(fplog,LOG_INFO,"Mobie PIN=%d",pin);
		

//	}
end:
	(void)mc_scard_shutdown(fplog,&hContext);
}
	log_end("T");
}

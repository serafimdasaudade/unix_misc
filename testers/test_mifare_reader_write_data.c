
#include "test_mifare_reader.h"

extern struct cardContracts  cContracts;

FILE *fplog;
char stemp[1000];

main()
{
	int rc;
	char reader_used[1000];
	SCARDCONTEXT  hContext;
	char data[4];
	int i=0;
	//mc2012
	data[0]=0x4d;
	data[1]=0x43;
	data[2]=0x14;
	data[3]=0x0c;
	fplog=log_start("T",LOG_DEBUG);

	printf("1 - Write Data\n");
	printf("2 - Clean Data\n");
	printf("3 - Write New ID\n");

	scanf("%d",&i);
	
	rc=scard_startup(fplog, &hContext);
	if (rc) return(-1);
	
	rc=scard_find_first_reader_with_a_card(fplog,hContext,reader_used);
	if (rc)
	{
		goto end;
	}
	
	switch(i)
	{
	  case 1:
	rc=mobie_mifare_write_data(fplog, hContext, reader_used, data);
	if (rc) log_printf(fplog,LOG_ERROR,"write data FAIL\n");
	else	log_printf(fplog,LOG_ERROR,"Write Data OK \n");
	  break;
	  case 2:
	    data[0]=0x00;
	    data[1]=0x00;
	    data[2]=0x00;
	    data[3]=0x00;
	    rc=mobie_mifare_write_data(fplog, hContext, reader_used, data);
	if (rc) log_printf(fplog,LOG_ERROR,"clean data FAIL\n");
	else	log_printf(fplog,LOG_ERROR,"Clean Data OK \n");
	    break;
		case 3:
		rc=mobie_mifare_write_new_id(fplog, hContext, reader_used, "aa");
		log_printf(fplog,LOG_ERROR,"rc=%d\n",rc);
		break;
	}
end:
	(void)scard_shutdown(fplog,&hContext);
	log_end("T");
}

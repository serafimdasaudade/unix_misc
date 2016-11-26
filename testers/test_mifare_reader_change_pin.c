
#include "test_mifare_reader.h"

extern struct cardContracts  cContracts;

FILE *fplog;
char stemp[1000];

main()
{
	int rc;
	char reader_used[1000];
	SCARDCONTEXT  hContext;
	int newpin=0;

	while (1)
	{
		printf("Introduce New non zero PIN? "); fflush(stdout);
		scanf("%d",&newpin);
		if (newpin) break;
	}

	fplog=log_start("T",LOG_DEBUG);

	rc=scard_startup(fplog, &hContext);
	if (rc) return(-1);

	rc=scard_find_first_reader_with_a_card(fplog,hContext,reader_used);
	if (rc)
	{
		goto end;
	}

	rc=mobie_mifare_write_pin(fplog, hContext, reader_used, newpin);
	if (rc) log_printf(fplog,LOG_ERROR,"DID NOT CHANGE PIN\n");
	else	log_printf(fplog,LOG_ERROR,"PIN changed to %d\n",newpin);

end:
	(void)scard_shutdown(fplog,&hContext);
	log_end("T");
}

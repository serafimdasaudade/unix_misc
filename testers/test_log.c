#include "mc_log.h"
FILE *fplog;

main()
{
	printf("In DEBUG2\n");
	fplog=log_open_file(LOG_DEBUG2,"log.log");
	logme();
while(1);
	log_close_file(fplog);

	printf("In DEBUG\n");
	fplog=log_open_file(LOG_DEBUG,"log.log");
	logme();
	log_close_file(fplog);

	printf("In INFO\n");
	fplog=log_open_file(LOG_INFO,"log.log");
	logme();
	log_close_file(fplog);

	printf("In WARNING\n");
	fplog=log_open_file(LOG_WARNING,"log.log");
	logme();
	log_close_file(fplog);

	printf("In ERROR\n");
	fplog=log_open_file(LOG_ERROR,"log.log");
	logme();
	log_close_file(fplog);
}

logme()
{
	log_printf(fplog,LOG_DEBUG2,"DEBUG2");
	log_printf(fplog,LOG_DEBUG,"DEBUG");
	log_printf(fplog,LOG_INFO,"INFO");
	log_printf(fplog,LOG_WARNING,"WARNING");
	log_printf(fplog,LOG_ERROR,"ERROR");
}

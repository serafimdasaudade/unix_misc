#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include "mc_misc.h"

#define PORT 		0x1234
#define DIRSIZE 	8192
FILE* LOGFILE=NULL;
int log_level;

#include <stdio.h>
#include <sys/types.h>

int do_work(FILE *fplog,int sd)
{
	char buf[100];
	mc_soc_recv(fplog,sd,buf,100);
	buf[10]=0;
	printf("recvd=%s\n",buf);
}

main()
{
	int sd,sdsocket;
	FILE *fplog;

	fplog=log_open_file(0,"aa.log");

	sdsocket=mc_soc_serv_open(fplog,PORT);
	if (sdsocket==-1)
	{
                log_printf(fplog,LOG_ERROR,"Open socket %s",strerror(errno));
		goto end;
	}
	else
	{
		sd=mc_soc_serv_wait_for_client(fplog,sdsocket,PORT);
		if (sd==-1)
		{
                	log_printf(fplog,LOG_ERROR,"Wait socket %s",strerror(errno));
			goto end;
		}
        	log_printf(fplog,LOG_DEBUG2,"Client connected");
		do_work(fplog,sd);
		mc_soc_serv_close(fplog,sdsocket,sd);
	}
end:
 	log_close_file(fplog);
        /* give client a chance to properly shutdown */
        sleep(1);
}

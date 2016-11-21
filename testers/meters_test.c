/*
        B9600
        B19200
        B38400
        B57600
        B115200
        B230400
*/

#include <unistd.h>
#include <stdlib.h>
#include "../include/log.h"
#include "../include/rs232.h"
#include "../include/meters.h"

FILE *fplog;

int read_vi(FILE *fplog, int fd, int *v, int *i);

int main(int argc,char *argv[])
{
	double stime,etime;
	int fd;
	int v=123,i=321;
	long e=456;
	int cntt=0;
	int rc;

	if ( (argc!=3) )
	{
		printf("Usage: %s <Device File> <testtype>\n",argv[0]);
		exit(0);
	}
	
	fplog=log_start("T",LOG_DEBUG2);

	log_printf(fplog,LOG_INFO,"Device File: %s. Starting in 1 seconds.\n",argv[1]);
	if (*argv[2]=='1') log_printf(fplog,LOG_INFO,"Teste Type: READINGS.\n");
	if (*argv[2]=='2') log_printf(fplog,LOG_INFO,"Teste Type: SPEED.\n");

	sleep(1);

	fd=open_rs232(fplog,argv[1],B115200);
	//fd=open_rs232(fplog,argv[1],B19200);
	if (fd<=0)
	{
		log_printf(fplog,LOG_ERROR,"%s: Error opening meter dev=%s","TESTER",argv[1]);
		return(-1);
	}
	
	change_log_level(LOG_INFO);
	cntt=0;
	while (1)
	{
		gettime_ms(fplog,&stime);
		if ((rc=meter_read_v_i_e(fplog,fd, &v, &i,&e))<0)
		{
			log_printf(fplog,LOG_ERROR,"%s: Error reading meter RC=%d","MT",rc);
		}
		else
		{
			gettime_ms(fplog,&etime);
			if (*argv[2]=='1') log_printf(fplog,LOG_INFO,"%s: meter V=%2.2f I=%2.2f E=%d\n","MT",(float)v/100,(float)i/100,e);
			if (*argv[2]=='2')
			{
				cntt++;
				if (cntt==50)
				{
					cntt=0;
					log_printf(fplog,LOG_INFO,"%s: meter V=%2.2f I=%2.2f E=%d TIME=%lfms\n","MT",(float)v/100,(float)i/100,e,etime-stime);
				}
				
			}
		}

//		usleep(50000);
	}
	close_rs232(fplog,fd);
	log_end("T");
}


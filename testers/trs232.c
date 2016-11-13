

#include "../include/mc_rs232.h"
#include "../include/mc_log.h"
#include "../include/mc_timers.h" 

main (int argc,char **argv)
{
	int i,delay=50;
	int rc,fd;
	FILE *fplog;
	int len=3;
	char bufvi[len];
	char c;

	fplog=log_start("T",LOG_DEBUG2);
	fd=mc_open_rs232(fplog,argv[1],B9600);
//c0 d6 01 cc

	bufvi[0]=0xa1;
	bufvi[1]=0x20;
	bufvi[2]=0xcc;
	bufvi[3]='\0';
	mc_write_rs232(fplog,fd, bufvi,len);
	mc_sleep_ms(fplog,delay);
	while(mc_read_rs232_timeout(fplog,fd,bufvi,1,delay)==1);
	
	
bufvi[0]=0xa1;
bufvi[1]=0x10;
bufvi[2]=0xcc;
bufvi[3]='\0';
mc_write_rs232(fplog,fd, bufvi,len);
mc_sleep_ms(fplog,delay);
while(mc_read_rs232_timeout(fplog,fd,bufvi,1,delay)==1);

	while (1)
	{

	  bufvi[0]=0xa1;
	  bufvi[1]=0x70;
	  bufvi[2]=0xcc;
	  bufvi[3]='\0';
	  
mc_write_rs232(fplog,fd, bufvi,len);
//sleep(1);
mc_sleep_ms(fplog,delay);
while(mc_read_rs232_timeout(fplog,fd,bufvi,1,delay)==1);
		
		/*
		for (i=0;i<rc;i++)
		{
			if (bufvi[i]==0xaa)  printf("\n");
			if (bufvi[i]==0xab)  printf("\n");
			printf("%02x",bufvi[i]);	
			fflush(stdout);
		}*/
		//log_printf_hex(fplog,LOG_INFO,bufvi,rc);

	}
	mc_close_rs232(fplog,fd);
	log_end("T");
}


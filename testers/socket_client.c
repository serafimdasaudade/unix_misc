#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include "../include/socket.h"

#define PORT        1234
             /* REPLACE with your server machine name*/
#define HOST        "localhost"
#define DIRSIZE     8192
FILE* LOGFILE=NULL;
int log_level;

main(argc, argv)
int argc; char **argv;
{

        char hostname[100];
	int	fd;
	char data[100];
	FILE *fplog;

	fplog=log_open_file(0,"aa.log");
        strcpy(hostname,HOST);
while(1){
	fd=soc_open(fplog,hostname,PORT);
//while(1){
	data[0]=0x01;
	data[1]=0x05;
	data[2]=0x01;
	data[3]='0';
	data[4]=0xaa;
	data[5]=0xbb;
	data[6]='\0';
printf("send data\n");
	soc_send(fplog,fd,data,strlen(data));
	soc_recv(fplog,fd,data,100);
	soc_close(fplog,fd);
	sleep(5);
}
	log_close_file(fplog);
	return(0);
}

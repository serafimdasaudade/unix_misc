#include "log.h"
#include "socket.h"
#include "timers.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int soc_open(FILE *fplog,char *hostname, int port)
{
	int sd;
	struct sockaddr_in pin;
	struct hostent *hp;

	if ((hp = gethostbyname(hostname)) == 0) {
		log_printf(fplog,LOG_ERROR,"soc_open: gethostbyname %s",strerror(errno));
		return(-1);
	}

	/* fill in the socket structure with host information */
        memset(&pin, 0, sizeof(pin));
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	pin.sin_port = htons(port);

        /* grab an Internet domain socket */
        sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		log_printf(fplog,LOG_ERROR,"socket (%s)",strerror(errno));
		return(-1);
	}
	/* connect to PORT on HOST */
	if (connect(sd,(struct sockaddr *)  &pin, sizeof(pin)) == -1) {
		log_printf(fplog,LOG_ERROR,"Sockectconnect (%s)",strerror(errno));
		return(-1);
	}
	
	return(sd);
}

int soc_send(FILE *fplog,int sd,unsigned char *buf,int buflen)
{
	int nb;
	/* send a message to the server PORT on machine HOST */
	log_printf(fplog,LOG_DEBUG2,"soc_send %d bytes",buflen);
	nb=send(sd, buf, buflen, 0);
	if (nb == -1) log_printf(fplog,LOG_ERROR,"SocketSend %s(%d)",strerror(errno),errno);
	if (nb != buflen) log_printf(fplog,LOG_WARNING,"Socket wrote=%d expected=%d",nb,buflen);
	return nb;
}

/*CS Not thread safe */
static timer_t tt;
int rstimeout;
void functout(int sig )
{
	rstimeout=1;
}

int soc_recv_timeout(FILE *fplog,int sd,unsigned char *buf,int buflen,int timeout_ms)
{
	int nb;
	
	rstimeout=0;
	
	nb=timer_start(NULL,timeout_ms/1000.0,functout,&tt);
        if (nb)
        {
		log_printf(fplog,LOG_ERROR,"Timer Start error rc=%d\n",nb);
                printf("Timer Start error rc=%d\n",nb);
                return(-1);
        }
        
        nb=recv(sd, buf, buflen, 0);
	timer_stop(NULL,tt);

	if (nb==0)
	{
    		log_printf(fplog,LOG_ERROR,"SocketRecv Remote Peer has shutdown");
    		return -3;
	}
	if (rstimeout)
	{
    		log_printf(fplog,LOG_ERROR,"SocketRecv Timeout ms=%d",timeout_ms);
    		return -2;
	}
	if (nb==-1)
	{
    		log_printf(fplog,LOG_ERROR,"SocketRecv (fd=%d) %s",sd,strerror(errno));
    		return -1;
	}
	
	return(nb);		
	
}

int soc_recv(FILE *fplog,int sd,unsigned char *buf,int buflen)
{
	int nb;
        nb=recv(sd, buf, buflen, 0);
	if (nb==0)
	{
    		log_printf(fplog,LOG_ERROR,"SocketRecv Remote Peer has shutdown");
    		return -3;
	}
	if (nb==-1)
	{
    		log_printf(fplog,LOG_ERROR,"SocketRecv (fd=%d) %s",sd,strerror(errno));
    		return -1;
	}
	return(nb);
}

void soc_close(FILE *fplog,int sd)
{
	close(sd);
}

int soc_serv_open(FILE *fplog,int port)
{
	int sdsock;
	struct   sockaddr_in sin;
 
	/* get an internet domain socket */
	if ((sdsock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                log_printf(fplog,LOG_ERROR,"Socket cannot create (%s)",strerror(errno));
		return(-1);
	}

	/* complete the socket structure */
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);

	/* bind the socket to the port number */
	if (bind(sdsock, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
                log_printf(fplog,LOG_ERROR,"Socket cannot bind (%s)",strerror(errno));
		return(-1);
	}

	/* show that we are willing to listen */
	if (listen(sdsock, 5) == -1) {
                log_printf(fplog,LOG_ERROR,"Socket cannot listen (%s)",strerror(errno));
		return(-1);
	}
	return(sdsock);
}

int soc_serv_wait_for_client_ret_ip(FILE *fplog,int sdsock,int port,long *ip)
{
        int      sd;
        socklen_t addrlen;
        struct sockaddr_in pin;

        /* fill in the socket structure with host information */
        memset(&pin, 0, sizeof(pin));
        pin.sin_family = AF_INET;
        pin.sin_addr.s_addr = INADDR_ANY;
        pin.sin_port = htons(port);
        addrlen = sizeof(pin);
        if ((sd = accept(sdsock, (struct sockaddr *)  &pin, &addrlen)) == -1) {
                log_printf(fplog,LOG_ERROR,"Socket cannot accept (%s)",strerror(errno));
                return(-1);
        }
        *ip=pin.sin_addr.s_addr;
        log_printf(fplog,LOG_DEBUG,"Socket accepted (fd=%d)",sd);
        return(sd);
}

int soc_serv_wait_for_client(FILE *fplog,int sdsock,int port)
{
	int 	 sd;
	socklen_t addrlen;
	struct sockaddr_in pin;

	/* fill in the socket structure with host information */
	memset(&pin, 0, sizeof(pin));
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = INADDR_ANY;
	pin.sin_port = htons(port);
        addrlen = sizeof(pin); 
	if ((sd = accept(sdsock, (struct sockaddr *)  &pin, &addrlen)) == -1) {
                log_printf(fplog,LOG_ERROR,"Socket cannot accept (%s)",strerror(errno));
		return(-1);
	}
        log_printf(fplog,LOG_DEBUG,"Socket accepted (fd=%d)",sd);
	return(sd);
}

void soc_serv_wait_for_client_close(FILE *fplog,int sd)
{
	close(sd);
}
void soc_serv_close(FILE *fplog,int sd)
{
	close(sd);
}
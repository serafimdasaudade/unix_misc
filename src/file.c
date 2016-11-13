#include "log.h"
#include "file.h"
#include "timers.h"

static int timerexpired=0;
void timeout_happened(int sig)
{
        timerexpired=1;
}

int read_and_wait_for_num_bytes(FILE *fplog, int fd, unsigned char *buf, int numbytes,double timeout)
{
	int nbr=0,totalnbr=0;
	timer_t timer_id=0;

	if (timeout) (void)timer_start (fplog, timeout, timeout_happened, &timer_id);

	while ( (totalnbr!=numbytes) && (!timerexpired) )
	{
		nbr=read(fd,buf,numbytes-totalnbr);
		totalnbr+=nbr;
		if (nbr==-1)
		{	
			log_printf(fplog,LOG_ERROR,"Cannot read event device. (%s)\n",strerror(errno));
			return(-1);
		}
	}
	if (timer_id) (void)timer_stop (fplog,timer_id);
	if (timerexpired)
	{
		log_printf(fplog,LOG_ERROR,"Timeout on read. Expected nb=%d Recvd=%d\n",numbytes,totalnbr);
		return (-2);
	}
	return (totalnbr);
}
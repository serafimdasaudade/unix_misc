#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include "log.h"
#include "i2c.h"
#include "timers.h"
#include <linux/i2c-dev.h>

static timer_t tt;

int close_i2c(FILE *fplog,int fd)
{
	close(fd);
	return(fd);
}

int flush_i2c(FILE *fplog,int fd)
{
	if (tcflush(fd, TCIOFLUSH))
	{
		log_printf(fplog,LOG_ERROR,"flush_i2c: tc_flush fd=%d err=%s",fd,strerror(errno));
		return(-1);
	}
	return(0);
}

int open_i2c(FILE *fplog,char *porta)
{
	int fd;

	log_printf(fplog,LOG_DEBUG2,"I2c: Opening");
	/* open the device to be non-blocking (read will return immediatly) */
	fd=open(porta, O_RDWR);
	//fd=open(porta, O_RDWR | O_NOCTTY );
	if (fd <0) {
		log_printf(fplog,LOG_ERROR,"I2C: %s (%s)",strerror(errno),porta);
		return(-1);
	}
	else{
		log_printf(fplog,LOG_DEBUG2,"I2C: %s Is Open fd=%d",porta,fd);
	}
	return(fd);
}

int set_address_slave_i2c(FILE *fplog,int fd,char addr)
{
	//When you have opened the device, you must specify with what device
	//address you want to communicate:

  	if (ioctl(fd, I2C_SLAVE, addr) < 0) {
    		log_printf(fplog,LOG_ERROR,"I2C: ERROR HANDLING; you can check errno to see what went wrong (%s)",strerror(errno));
    		return -1;
  	}
	log_printf(fplog,LOG_DEBUG2,"I2C: set address slave i2c (%s)",strerror(errno));
  	return 0;
}

int read_i2c(FILE *fplog,int fd, unsigned char* buf,int nb)
{
	int rlen;
	char stemp[200];

	log_printf(fplog,LOG_DEBUG2,"I2C: going to read");
	rlen = read(fd,buf,nb);
	if (rlen==-1)
	{
    		log_printf(fplog,LOG_ERROR,"I2C: read error %s",strerror(errno));
    		return -1;
	}
	else
	{
		sprintf(stemp,"I2C: %d bytes",rlen);
		log_printf_hex_title(fplog,LOG_DEBUG,stemp,buf,rlen);
	}
	return rlen;
}

//////// NOT THREAD SAFE  ///////////////////////////////////////////////
int rstimeout;
void functout1(int sig )
{
	rstimeout=1;
}

int read_i2c_timeout(FILE *fplog,int fd, unsigned char* buf,int nb,int timeout_ms)
{
	int res;

	rstimeout=0;

	log_printf(fplog,LOG_DEBUG2,"I2C: going to read I2C ");

	res=timer_start(NULL,timeout_ms/1000.0,functout1,&tt);
        if (res)
        {
                printf("Timer Start error rc=%d\n",res);
                return(-1);
        }

	res = read(fd,buf,nb);
	timer_stop(NULL,tt);

	if (rstimeout)
	{
    		log_printf(fplog,LOG_ERROR,"I2C: read I2C Timeout ms=%d",timeout_ms);
    		return -2;
	}
	if (res==-1)
	{
    		log_printf(fplog,LOG_ERROR,"I2C: read i2c %s",strerror(errno));
    		return -1;
	}
	else
	{
		log_printf(fplog,LOG_DEBUG,"I2C: read i2c %d bytes",res);
		log_printf_hex(fplog,LOG_DEBUG,buf,res);
	}
	return res;
}
/////////////////////////////////////////////////////////////////////////////////////////////
int  write_i2c(FILE *fplog,int fd, unsigned char* send_bytes,int number_bytes)
{
    log_printf(fplog,LOG_DEBUG2,"I2C: send fd=%d nb=%d",fd,number_bytes);
    int wr=write(fd,send_bytes, number_bytes);
    if(wr<=0)
    {
        log_printf(fplog,LOG_ERROR,"I2C: Error sending rc=%d (%s)",wr,strerror(errno));

        return(-1);
    }
	log_printf(fplog,LOG_DEBUG2,"I2C: Sent %d bytes (%s)",wr,strerror(errno));
	log_printf_hex(fplog,LOG_DEBUG2,send_bytes,wr);

	return(wr);
}
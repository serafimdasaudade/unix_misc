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
#include "rs232.h"
#include "timers.h"

static timer_t tt;

int close_rs232(FILE *fplog,int fd)
{
	close(fd);
	return(fd);
}

int flush_rs232(FILE *fplog,int fd)
{
	if (tcflush(fd, TCIOFLUSH))
	{
		log_printf(fplog,LOG_ERROR,"flush_rs232: tc_flush fd=%d err=%s",fd,strerror(errno));
		return(-1);
	}
	return(0);
}

int open_rs232(FILE *fplog,char *porta,int speed)
{
	int fd;

	log_printf(fplog,LOG_DEBUG2,"RS232: Opening");
	/* open the device to be non-blocking (read will return immediatly) */
	fd=open(porta, O_RDWR | O_NOCTTY | O_NDELAY);
	//fd=open(porta, O_RDWR | O_NOCTTY );
	if (fd ==-1) {
		log_printf(fplog,LOG_ERROR,"RS232: %s (%s)",strerror(errno),porta);
		return(-1);
	}
	else{
		log_printf(fplog,LOG_DEBUG2,"RS232: %s Is Open fd=%d",porta,fd);
		fcntl(fd, F_SETFL, 0);
	}

	struct termios port_settings;      // structure to store the port settings in
    tcgetattr(fd, &port_settings);

    /*
     * Set the baud rates to 19200...
     */

    cfsetispeed(&port_settings, speed);
    cfsetospeed(&port_settings, speed);
/*
   	B9600
        B19200
        B38400
        B57600
        B115200
        B230400
*/

	port_settings.c_cflag |= CS8 | CLOCAL | CREAD ;
	port_settings.c_cflag &= ~PARENB;
	port_settings.c_cflag &= ~CSTOPB;
	port_settings.c_cflag &= ~CSIZE;
	port_settings.c_cflag &= ~CRTSCTS;


	port_settings.c_iflag &= ~ISTRIP;
	port_settings.c_iflag &= ~INPCK;
	port_settings.c_iflag &= ~IGNPAR;
	port_settings.c_iflag &= ~IGNBRK;
	port_settings.c_iflag &= ~INLCR;
	port_settings.c_iflag &= ~ICRNL;
	port_settings.c_iflag &= ~IGNCR;
	port_settings.c_iflag &= ~IUCLC;
	port_settings.c_iflag &= ~IXON;
	port_settings.c_iflag &= ~IXANY;
	port_settings.c_iflag &= ~IXOFF;
	port_settings.c_iflag &= ~IEXTEN;


	port_settings.c_lflag &= ~ICANON;
	port_settings.c_lflag &= ~ECHO;
	port_settings.c_lflag &= ~ECHOE;
	port_settings.c_lflag &= ~ISIG;
	port_settings.c_lflag &= ~ECHOK;
	port_settings.c_lflag &= ~ECHOKE;
	port_settings.c_lflag &= ~ECHONL;
	port_settings.c_lflag &= ~ECHOPRT;
	port_settings.c_lflag &= ~ECHOCTL;

	port_settings.c_oflag &= ~OPOST;

	/*These have no function when opened O_NDELAY*/
	port_settings.c_cc[VMIN]=1;
	port_settings.c_cc[VTIME]=0;


	//tcsetattr(fd, TCSANOW, &port_settings);
	tcsetattr(fd, TCSAFLUSH, &port_settings);
	return(fd);
}

int rtsdtr_rs232(FILE *fplog,int fd)
{
	int serstat;

	ioctl(fd, TIOCMGET, &serstat);

/*
	if (serstat & TIOCM_RTS) 
		log_printf(fplog,LOG_DEBUG,"RS232: RTS        ON");
	else	log_printf(fplog,LOG_DEBUG,"RS232: RTS        OFF");
	if (serstat & TIOCM_DTR) 
		log_printf(fplog,LOG_DEBUG,"RS232: DTR        ON");
	else	log_printf(fplog,LOG_DEBUG,"RS232: DTR        OFF");
*/

	/* Switch off RTS and DTR */
	serstat &= ~TIOCM_RTS;
	serstat &= ~TIOCM_DTR;
	ioctl(fd, TIOCMSET, &serstat);
	ioctl(fd, TIOCMGET, &serstat);

	if (serstat & TIOCM_RTS) 
		log_printf(fplog,LOG_DEBUG,"RS232: RTS        ON");
	else	log_printf(fplog,LOG_DEBUG,"RS232: RTS        OFF");
	if (serstat & TIOCM_DTR) 
		log_printf(fplog,LOG_DEBUG,"RS232: DTR        ON");
	else	log_printf(fplog,LOG_DEBUG,"RS232: DTR        OFF");
	//CS   sleep_s(fplog,1);

	/* Liga DTR */
	serstat |= TIOCM_DTR;
	ioctl(fd, TIOCMSET, &serstat);
	ioctl(fd, TIOCMGET, &serstat);

	if (serstat & TIOCM_DTR) 
		log_printf(fplog,LOG_DEBUG,"RS232: DTR        ON");
	else	log_printf(fplog,LOG_DEBUG,"RS232: DTR        OFF");

	sleep_s(fplog,1);
	/* DESLiga DTR */
	serstat &= ~TIOCM_DTR;
	ioctl(fd, TIOCMSET, &serstat);
	ioctl(fd, TIOCMGET, &serstat);
	if (serstat & TIOCM_DTR) 
		log_printf(fplog,LOG_DEBUG,"RS232: DTR        ON");
	else	log_printf(fplog,LOG_DEBUG,"RS232: DTR        OFF");

	return(0);
}

int read_rs232(FILE *fplog,int fd, unsigned char* buf,int nb)
{
	int rlen;
	char stemp[200];

	log_printf(fplog,LOG_DEBUG2,"RS232: going to read");
	rlen = read(fd,buf,nb);
	if (rlen==-1)
	{
    		log_printf(fplog,LOG_ERROR,"RS232: read error %s",strerror(errno));
    		return -1;
	}
	else
	{
		sprintf(stemp,"RS232: %d bytes",rlen);
		log_printf_hex_title(fplog,LOG_DEBUG,stemp,buf,rlen);
	}
	return rlen;
}

int rstimeout;
void functout1(int sig )
{
	rstimeout=1;
}

int read_rs232_timeout(FILE *fplog,int fd, unsigned char* buf,int nb,int timeout_ms)
{
	int res;

	rstimeout=0;

	log_printf(fplog,LOG_DEBUG2,"RS232: going to read rs232 ");

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
    		log_printf(fplog,LOG_ERROR,"RS232: read rs232 Timeout ms=%d",timeout_ms);
    		return -2;
	}
	if (res==-1)
	{
    		log_printf(fplog,LOG_ERROR,"RS232: read rs232 %s",strerror(errno));
    		return -1;
	}
	else
	{
		log_printf(fplog,LOG_DEBUG,"RS232: read rs232 %d bytes",res);
		log_printf_hex(fplog,LOG_DEBUG,buf,res);
	}
	return res;
}

int read_rs232_wait_numbytes(FILE *fplog,int fd, unsigned char* buf,int numbytes)
{
	unsigned char buft[255];
	volatile int STOP=0;
	int bytesrecv=0;
	int res;

	/* loop while waiting for input. normally we would do something
	useful here */
	while (STOP==0)
	{
		log_printf(fplog,LOG_DEBUG2,"RS232: going to read rs232 totalnb=%d ",numbytes);
		res = read(fd,buft,numbytes-bytesrecv);
		if (res==-1)
		{
    			log_printf(fplog,LOG_ERROR,"RS232: read rs232 %s",strerror(errno));
    			return -1;
		}
		else
		{
   			log_printf(fplog,LOG_DEBUG2,"RS232: read rs232 res=%d totalrcvd=%d",res, bytesrecv);
			memcpy(buf+bytesrecv,buft,res);
			bytesrecv+=res;

			if (bytesrecv>=numbytes)
			{
				STOP=1;
			}
		}
	}
	return bytesrecv;
}

int write_rs232(FILE *fplog,int fd, unsigned char* send_bytes,int number_bytes)
{
    log_printf(fplog,LOG_DEBUG2,"RS232: send fd=%d nb=%d",fd,number_bytes);
    int wr=write(fd,send_bytes, number_bytes);
    switch(wr)
    {
    case -1:
        log_printf(fplog,LOG_ERROR,"RS232: Error sending rc=%s",strerror(errno));

        return(-1);
    }
	log_printf(fplog,LOG_DEBUG2,"RS232: Sent %d bytes",wr);
	log_printf_hex(fplog,LOG_DEBUG2,send_bytes,wr);

	return(wr);
}

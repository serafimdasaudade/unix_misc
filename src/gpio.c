/*
 * This code is for gpio on the Friendly arm
 */

/*
 * On Linux do:
 *   mount -t debugfs none /sys/kernel/debug
 *   cat /sys/kernel/debug/gpio
 *  To show how GPIOs are configured in kernel
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "log.h"
#include "gpio.h"
#include "file.h"


int gpio_wait_for_uevent(FILE *fplog, double timeout)
{
	int fd,nb;
	unsigned char buf[1000];
	fd=open(EVENT_DEVICE,O_RDONLY);
	if (fd==-1)
	{	
		log_printf(fplog,LOG_ERROR,"GPIO: Cannot open event device. (%s) (%s)",EVENT_DEVICE,strerror(errno));
		return(-1);
	}
	nb=read_and_wait_for_num_bytes(fplog, fd, buf, FRIENDLY_ARM_EVENT_NUM_BYTES,timeout);
	close(fd);
	log_printf(fplog,LOG_DEBUG2,"GPIO: Event arrived.");
	if (nb<0) return(nb); /* Returns -1 or -2 */
	if (nb!=FRIENDLY_ARM_EVENT_NUM_BYTES) return (-3);
	
	switch (buf[FRIENDLY_ARM_EVENT_OFFSET_INTERRUPT_NUM])
	{
		case 0x3B: return(K1);
		case 0x3C: return(K2);
		case 0x3D: return(K3);
		case 0x74: return(K4);
		case 0x3F: return(K5);
	}
	char stemp[1000];
	sprintf(stemp,"GPIO: Undefined Event arrived. 0x%X",buf[FRIENDLY_ARM_EVENT_OFFSET_INTERRUPT_NUM]);
	log_printf_hex_title(fplog,LOG_ERROR,stemp,buf,FRIENDLY_ARM_EVENT_OFFSET_INTERRUPT_NUM+5);
	return(-4);
}

int gpio_init_port(FILE *fplog,int pnum)
{
	FILE *fp;
	//This will create the folder /sys/class/gpio/gpio<pnum>
	if ((fp = fopen("/sys/class/gpio/export", "ab")) == NULL)
	{
		log_printf(fplog,LOG_ERROR,"GPIO: Cannot open export file.");
		return(1);
	}
	rewind(fp);
	if (fprintf(fp,"%d",pnum) < 0)
	{
		log_printf(fplog,LOG_ERROR,"GPIO: Cannot open gpio file /sys/class/gpio/gpio%d",pnum);
		return(2);
	}
	fclose(fp);
	return(0);
}
	
int gpio_set_direction_IN(FILE *fplog,int pnum)
{
	FILE *fp;
	char stemp[100];
	sprintf(stemp,"/sys/class/gpio/gpio%d/direction",pnum);
	if ((fp = fopen(stemp, "rb+")) == NULL)
	{
		log_printf(fplog,LOG_ERROR,"GPIO: IN Cannot open direction file. (%d)",pnum);
		return(1);
	}
	rewind(fp); fprintf(fp,"in"); fclose(fp); return(0);
}
	
int gpio_set_direction_OUT(FILE *fplog,int pnum)
{
	FILE *fp;
	char stemp[100];
	sprintf(stemp,"/sys/class/gpio/gpio%d/direction",pnum);
	if ((fp = fopen(stemp, "rb+")) == NULL)
	{
		log_printf(fplog,LOG_ERROR,"GPIO: OUT Cannot open direction file. (%d)",pnum);
		return(1);
	}
	rewind(fp); fprintf(fp,"out"); fclose(fp); return(0);
}
	
int gpio_set_value_LOW(FILE *fplog,int pnum)
{
	FILE *fp;
	char stemp[100];

	log_printf(fplog,LOG_DEBUG,"GPIO: set LOW port=%d",pnum);

	sprintf(stemp,"/sys/class/gpio/gpio%d/value",pnum);
	if ((fp = fopen(stemp, "rb+")) == NULL)
	{
		log_printf(fplog,LOG_ERROR,"GPIO: gpio_set_value_HIGH Cannot open value file. Port=%d err=%s",pnum,strerror(errno));
		return(1);
	}
	rewind(fp); fprintf(fp,"0"); fclose(fp); return(0);
}
	
int gpio_set_value_HIGH(FILE *fplog,int pnum)
{
	FILE *fp;
	char stemp[100];
	log_printf(fplog,LOG_DEBUG,"GPIO: set HIGH port=%d",pnum);
	sprintf(stemp,"/sys/class/gpio/gpio%d/value",pnum);
	if ((fp = fopen(stemp, "rb+")) == NULL)
	{
		log_printf(fplog,LOG_ERROR,"GPIO: gpio_set_value_HIGH Cannot open value file. Port=%d err=%s",pnum,strerror(errno));
		return(1);
	}
	rewind(fp); fprintf(fp,"1"); fclose(fp); return(0);
}
	
int gpio_get_value(FILE *fplog,int pnum)
{
	int fd,rc;
	char stemp[100],data[100];
	log_printf(fplog,LOG_DEBUG,"GPIO: get value port=%d",pnum);
	sprintf(stemp,"/sys/class/gpio/gpio%d/value",pnum);
	if ((fd = open(stemp, O_RDONLY)) == -1)
	{
		log_printf(fplog,LOG_ERROR,"GPIO: gpio_get_value. Cannot open value file. Port=%d err=%s",pnum,strerror(errno));
		return(1);
	}
	rc=read(fd,&data,10); close(fd);
	/* returns 2 bytes second byte=10 (LF) ignore*/
	if (rc!=2) log_printf(fplog,LOG_ERROR,"GPIO: Not normal rcv=%d bytes b1=%d b2=%d",rc,data[0],data[1]);
	return(data[0]);
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "fifo.h"

int open_or_create_fifo_for_read(FILE *fplog,char *name)
{
	int fd;
	if ((fd=open(name,O_RDONLY|O_NONBLOCK))==-1)
	{
		if (errno==ENOENT)
		{
			if ((fd=mkfifo(name,S_IRUSR|S_IWUSR ))==-1)
			{
				log_printf(fplog,LOG_ERROR,"FIFO: Cannot create fifo. (%s) file=%s",strerror(errno),name);
			}
			else
			{
				if ((fd=open(name,O_RDONLY|O_NONBLOCK))==-1)
					log_printf(fplog,LOG_ERROR,"FIFO: Cannot open fifo (1). (%s) file=%s",strerror(errno),name);
			}
		}
		else log_printf(fplog,LOG_ERROR,"FIFO: Cannot open fifo (2). (%s) File=%s",strerror(errno),name);
	}
	fcntl(fd, F_SETFL, O_NONBLOCK);  // set to non-blocking
	return(fd);
}


int open_or_create_fifo_for_write(FILE *fplog,char *name)
{
	int fd;
	if ((fd=open(name,O_WRONLY|O_NONBLOCK))==-1)
	{
		if (errno==ENOENT)
		{
			if ((fd=mkfifo(name,S_IRUSR|S_IWUSR ))==-1)
			{
				log_printf(fplog,LOG_ERROR,"FIFO: Cannot create fifo(%s). (%s)",name,strerror(errno));
			}
			else
			{
				if ((fd=open(name,O_WRONLY|O_NONBLOCK))==-1)
					log_printf(fplog,LOG_ERROR,"FIFO: Cannot open pipe (1). (%s)",strerror(errno));
			}
		}
		else log_printf(fplog,LOG_ERROR,"FIFO: Cannot open fifo (2). (%s)",strerror(errno));
	}
	fcntl(fd, F_SETFL, O_NONBLOCK);  // set to non-blocking
	return(fd);
}

#include <sys/time.h>
#include <stdlib.h>
#include "timers.h"
#include "log.h"
#include "signal.h"

/*
 * Sleeps in seconds. 
 */
void sleep_s(FILE *fplog,int t_secs)
{
	sleep_ms(fplog,t_secs*1000);
}

void sleep_s_r(FILE *fplog,int t_secs,struct timespec *remaining)
{
	int r;
	struct timespec ts;
	ts.tv_sec=t_secs;
	ts.tv_nsec=0;
	//r=sleep(t_secs);
	//r=usleep(t_secs*1000000);
	r=nanosleep(&ts,remaining);
	if (r==0) return;
	else
	{
		switch (errno)
		{
			case EINTR:
				log_printf(fplog,LOG_WARNING,"TIME: Interrupted. Remaining time=%lfs",((double)remaining->tv_sec+((double)remaining->tv_nsec/(double)1000000000)));
				break;
			case EFAULT:
				log_printf(fplog,LOG_ERROR,"TIME: User space problem");
				break;
			case EINVAL:
				log_printf(fplog,LOG_ERROR,"TIME: Input value wrong=%d",ts.tv_sec);
				break;
			default:
				log_printf(fplog,LOG_ERROR,"TIME: Internal Error T001");
				break;
		}
	}
}

/*
 *  Sleeps in milli seconds.
 */
void sleep_ms(FILE *fplog,int tt)
{
	int r;
	struct timespec ts;
	struct timespec remaining;
	if (tt>=1000)
	{	ts.tv_sec=tt/1000;
		ts.tv_nsec=(tt%1000)*1000000;
	}
	else
	{	ts.tv_sec=0;
		ts.tv_nsec=tt*1000000;
	}
	//r=usleep(tt*1000);
	remaining.tv_sec=0;
	remaining.tv_nsec=0;


complete_this_sleep:

	if ( (remaining.tv_sec!=0) || (remaining.tv_nsec!=0) )
	{
		ts.tv_sec=remaining.tv_sec;
		ts.tv_nsec=remaining.tv_nsec;
	}

//log_printf(fplog,LOG_WARNING,"TIME: Going sleep. time=%lfms",((double)ts.tv_sec*(double)1000+((double)ts.tv_nsec/(double)1000000)));

	r=nanosleep(&ts,&remaining);


	if (r==0) return;
	else
	{
		switch (errno)
		{
			case EINTR:
				if ( (remaining.tv_sec==0) && (remaining.tv_nsec<1000000) )
					break;
				else
				{
					log_printf(fplog,LOG_WARNING,"TIME: CS Interrupted. Remaining time=%lfms",((double)remaining.tv_sec*(double)1000+((double)remaining.tv_nsec/(double)1000000)));
					goto complete_this_sleep;
				}
				break;
			case EFAULT:
				log_printf(fplog,LOG_ERROR,"TIME: User space problem");
				break;
			case EINVAL:
				log_printf(fplog,LOG_ERROR,"TIME: Input value wrong=%d",ts.tv_sec);
				break;
			default:
				log_printf(fplog,LOG_ERROR,"TIME: Internal Error T001");
				break;
		}
	}
}

void sleep_ms_r(FILE *fplog,int tt,struct timespec *remaining)
{
	int r;
	struct timespec ts;
	if (tt>=1000)
	{	ts.tv_sec=tt/1000;
		ts.tv_nsec=(tt%1000)*1000000;
	}
	else
	{	ts.tv_sec=0;
		ts.tv_nsec=tt*1000000;
	}
	//r=usleep(tt*1000);
	r=nanosleep(&ts,remaining);
	if (r==0) return;
	else
	{
		switch (errno)
		{
			case EINTR:
				log_printf(fplog,LOG_WARNING,"TIME: Interrupted. Remaining time=%lfms",((double)remaining->tv_sec*(double)1000+((double)remaining->tv_nsec/(double)1000000)));
				break;
			case EFAULT:
				log_printf(fplog,LOG_ERROR,"TIME: User space problem");
				break;
			case EINVAL:
				log_printf(fplog,LOG_ERROR,"TIME: Input value wrong=%d",ts.tv_sec);
				break;
			default:
				log_printf(fplog,LOG_ERROR,"TIME: Internal Error T001");
				break;
		}
	}
}

int setTime(FILE *fplog,const char* timeStr)  // format like YYYYMMDDHHMISS
{
	int rc;
	char buf[5];
	struct tm tm_ptr;

	if (strlen(timeStr)!=14)
	{
		log_printf(fplog,LOG_ERROR,"Date len invalid (%s)",timeStr);
		return(-1);
	}
	strncpy(buf, timeStr + 0, 4); buf[4]=0;
	unsigned short year = atoi(buf);

	strncpy(buf, timeStr + 4, 2); buf[2]=0;
	unsigned short month = atoi(buf);

	strncpy(buf, timeStr + 6, 2); buf[2]=0;
	unsigned short day = atoi(buf);

	strncpy(buf, timeStr + 8, 2); buf[2]=0;
	unsigned short hour = atoi(buf);

	strncpy(buf, timeStr + 10, 2); buf[2]=0;
	unsigned short min = atoi(buf);

	strncpy(buf, timeStr + 12, 2); buf[2]=0;
	unsigned short sec = atoi(buf);

	tm_ptr.tm_mon  = month-1;
	tm_ptr.tm_mday = day;
	tm_ptr.tm_year = year-1900;
	tm_ptr.tm_hour = hour;
	tm_ptr.tm_min = min;
	tm_ptr.tm_sec = sec;

	struct timeval tv;
	tv.tv_sec = mktime(&tm_ptr);
	if ((tv.tv_sec) == -1)
	{
		log_printf(fplog,LOG_ERROR,"mktime failed errno=%d,errstr=%s",errno,strerror(errno));
		exit(-2);
	}
	tv.tv_usec = 0;

	struct timezone tz;
	tz.tz_minuteswest=0;
	tz.tz_dsttime=0;

	rc=settimeofday(&tv, &tz);
	if (rc==-1)
	{
		log_printf(fplog,LOG_ERROR,"Error setting Time errno=%s",strerror(errno));
		return(-3);
	}
	else log_printf(fplog,LOG_WARNING,"TIME changed %s",timeStr);
	return(0);
}

/*
 *  Returns milli seconds since EPOCH
 */
void gettime_ms(FILE *fplog,double *time1)
{
	struct timespec t1;
	if (clock_gettime(CLOCK_REALTIME, &t1) )
	{
		log_printf(fplog,LOG_ERROR,"TIMER: clock_gettime (ms) err=%s",strerror(errno));
	}
	*time1 = (double)t1.tv_sec*1000+(double)t1.tv_nsec/1000000;
}

/*
 *  Returns seconds since EPOCH
 */
void gettime(FILE *fplog,double *time1)
{
	struct timespec t1;
	if (clock_gettime(CLOCK_REALTIME, &t1))
	{
		log_printf(fplog,LOG_ERROR,"TIMER: clock_gettime err=%s",strerror(errno));
	}
	*time1 = (double)(t1.tv_sec+t1.tv_nsec*1.e-9);
}

/* final_date must have at least 15 bytes length */
void gettime_str(FILE *fplog,char *final_date)
{
	time_t t;
	struct tm * timeinfo;
	time(&t);
	timeinfo = localtime (&t);

	char temp_date[30];
	strftime(temp_date,30,"%Y%m%d%H%M%S",timeinfo);
	strncpy(final_date,temp_date,14);
	final_date[14]=0;
}

/*
 *  Sets an interval timer
 *  return 0 OK  -1 Error 
 */
#include <signal.h>

int timer_start (FILE *fplog, double sec, void (*sighandler)(int), timer_t *timer_id)
{
	int rc;
	struct itimerspec       itime;
	struct sigevent event;
	double nano;
	double seconds;

	struct sigaction sigact;

	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;

	if (sigaction(SIGALRM, &sigact, (struct sigaction *)NULL) == -1)
	{
		log_printf(fplog,LOG_ERROR,"timer_start:sigaction: %s (%d)",strerror(errno),errno);
		return(-1);
	}

	event.sigev_notify = SIGEV_SIGNAL;
	event.sigev_signo = SIGALRM;

	nano=modf(sec,&seconds); 
	nano*=1000000000;

	itime.it_value.tv_sec = (long)seconds;
	itime.it_value.tv_nsec = nano;
	itime.it_interval.tv_sec = (long)seconds;
	itime.it_interval.tv_nsec =  nano;

	rc=timer_create(CLOCK_REALTIME, &event, timer_id);
	if (rc==-1)
	{
		log_printf(fplog,LOG_ERROR,"timer_start:timer_create: %s (%d)",strerror(errno),errno);
		return(-1);
	}
	rc=timer_settime(*timer_id, 0, &itime, NULL);
	if (rc==-1)
	{
		log_printf(fplog,LOG_ERROR,"timer_start:timer_settime: %s (%d)",strerror(errno),errno);
		return(-1);
	}
	unblock_signal(fplog,SIGALRM);
	return(0);
}

int timer_stop (FILE *fplog,timer_t timerid)
{
	int rc;
	rc=timer_delete(timerid);

	if (rc==-1)
	{
		log_printf(fplog,LOG_ERROR,"timer_stop:timer_delete: %s (%d)",strerror(errno),errno);
		return(-1);
	}
	return(0);
}

/*
timer_status
	On return these values are set
	itime->it_value.tv_sec 		seconds until timer expires
	itime->it_value.tv_nsec  	nanoseconds until timer expires
	itime->it_interval.tv_sec 	seconds interval set
	itime->it_interval.tv_nsec	nanoseconds interval set
 */
int timer_status (FILE *fplog,timer_t timerid,struct itimerspec *itime)
{
	int rc;

	rc=timer_gettime(timerid, itime);
	if (rc==-1)
	{
		log_printf(fplog,LOG_ERROR,"timer_status:timer_gettime: %s (%d)",strerror(errno),errno);
		return(-1);
	}
	return(0);
}

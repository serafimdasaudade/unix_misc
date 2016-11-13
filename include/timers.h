
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <math.h>

int setTime(FILE *fplog,const char* timeStr);  // format like YYYYMMDDHHMISS
void gettime_ms(FILE *fplog,double *time1);
void gettime(FILE *fplog,double *time1);
void gettime_str(FILE *fplog,char *final_date); /*final_date[15]*/
void sleep_s(FILE *fplog,int t_secs);
void sleep_s_r(FILE *fplog,int t_secs,struct timespec *remaining);
void sleep_ms(FILE *fplog,int tt);
void sleep_ms_r(FILE *fplog,int tt,struct timespec *remaining);
int timer_start (FILE *fplog, double sec, void (*sighandler)(int), timer_t *timer_id);
int timer_stop (FILE *fplog,timer_t timerid);
int timer_status (FILE *fplog,timer_t timerid,struct itimerspec *itime);

/*
timer_status
        On return these values are set

	itime->it_value.tv_sec 		seconds until timer expires
	itime->it_value.tv_nsec  	nanoseconds until timer expires
	itime->it_interval.tv_sec 	seconds interval set
	itime->it_interval.tv_nsec	nanoseconds interval set
 */
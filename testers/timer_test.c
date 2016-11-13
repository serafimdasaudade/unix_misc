#include <errno.h>
#include <stdio.h>
#include <time.h>

static timer_t tt;
static timer_t tt1;
static int mais=1;
static int mais1=1;

void func(int sig)
{
	static int ii=0;
	printf("Called func\n");
	ii++;
	if ( (ii%5)==0 )
	{
		mais=0;
		mc_timer_stop(NULL,tt);
	}
}
void func1(int sig)
{
	static int ii=0;
	printf("Called func1\n");
	ii++;
	if ( (ii%5)==0 )
	{
		mais1=0;
		mc_timer_stop(NULL,tt1);
	}
}

main()
{
	int rc;
	double timeNow;

/*
	mc_gettime(&timeNow);
	printf("TIME since EPOCH=%lf Going to sleep 2 secs\n",timeNow);
	mc_sleep_s(NULL,2);
	printf("Going to sleep 2000 millisecs\n");
	mc_sleep_ms(NULL,2000);
*/
redo:
	printf("Arming timer for 1 secs should call func 5 times \n");

	rc=mc_timer_start(NULL,1.000000001,func,&tt);
	if (rc)
	{
		printf("Exiting...rc=%d\n",rc);
		return(1);
	}

	while (mais) ;
	printf("Stopped timer\n");

mais=1;
	/* Lets test second call to timer */
	rc=mc_timer_start(NULL,0.500000001,func1,&tt1);
	if (rc)
	{
		printf("Exiting...rc=%d\n",rc);
		return(1);
	}

	while (mais1) mc_sleep_ms(NULL,1000);
	printf("Stopped second timer\n");
mais1=1;

goto redo;

	printf("Program keeps running to check that timer has really stopped\n");
	printf("Program Shuld not print anything else\n");

	while(1); /* Ver se timer parou mesmo
			programa deve ficar aqui preso */
}

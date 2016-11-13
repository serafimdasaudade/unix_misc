
#include <stdio.h>
#include <pthread.h>
#include "log.h"
#include "thread.h"

pthread_t create_thread_detached(FILE *fplog, void *func(void*),void *param)
{
	int rc;
	pthread_attr_t attr;
	pthread_t ptt;

	log_printf(fplog,LOG_DEBUG2,"THREAD: start detached thread");
       	pthread_attr_init(&attr);
       	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	rc = pthread_create(&ptt, &attr, func, param);
	if (rc){
		log_printf(fplog,LOG_ERROR,"THREAD: pthread_create() rc=%d",rc);
		return(-1);
	}
	return(ptt);
}

pthread_t create_thread_joinable(FILE *fplog, void *func(void*),void *param)
{
	int rc;
	pthread_attr_t attr;
	pthread_t ptt;

	log_printf(fplog,LOG_DEBUG2,"THREAD: start joinable thread");
       	pthread_attr_init(&attr);
       	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	rc = pthread_create(&ptt, &attr, func, (void*)param);
	if (rc){
		log_printf(fplog,LOG_ERROR,"THREAD: pthread_create() rc=%d",rc);
		return(-1);
	}
	return(ptt);
}
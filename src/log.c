
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.h"
//#include "sat_control.h"

/*
const char ANSI_RESET[] = "\x1b[0m";
const char ANSI_RED[]     = "\x1b[31m";
const char ANSI_GREEN[]   = "\x1b[32m";
const char ANSI_YELLOW[]  = "\x1b[33m";
const char ANSI_BLUE[]    = "\x1b[34m";
const char ANSI_MAGENTA[] = "\x1b[35m";
const char ANSI_CYAN[]    = "\x1b[36m";
const char ANSI_WHITE[]   = "\x1b[37m";
*/


/* Nt thread safe, each thread should have its level
   Future: struct with fplog, level_chosen
   Fr now: First log open defines for all threads
 */
int log_level_chosen=LOG_UNKNOWN;

int log_printf_hex_title(FILE *fp,int level, char *title, unsigned char *buf, int buflen)
{
  char buf_formated[100000];
  int i;
  int logpos=0;
	unsigned int tsec,tmilli;
	struct timespec t1;
        (void)clock_gettime(CLOCK_REALTIME, &t1);
        tsec=(unsigned int)t1.tv_sec;
	tmilli=(int)t1.tv_nsec/1000000;
  switch (log_level_chosen)
  {
	case LOG_DEBUG2: if (level==LOG_DEBUG2)
			 {	sprintf(buf_formated,"%u.%03d DEB2  ",tsec,tmilli);
				logpos=1;
			 }
	case LOG_DEBUG: if (level==LOG_DEBUG)
			{	sprintf(buf_formated,"%u.%03d DEB1  ",tsec,tmilli);
				logpos=1;
			}
	case LOG_INFO: if (level==LOG_INFO)
			{	sprintf(buf_formated,"%u.%03d INFO  ",tsec,tmilli);
				logpos=1;
			}
	case LOG_WARNING: if (level==LOG_WARNING)
			{	sprintf(buf_formated,"%u.%03d WARN  ",tsec,tmilli);
				logpos=1;
			}
	case LOG_ERROR: if (level==LOG_ERROR)
			{	sprintf(buf_formated,"%u.%03d ERRO  ",tsec,tmilli);
				logpos=1;
			}
  }

  if (!logpos)  goto ended;
  sprintf(buf_formated,"%s%s 0x",buf_formated,title);
  for (i=0;i<buflen;i++) sprintf(buf_formated,"%s%02X",buf_formated,buf[i]);
  strcat(buf_formated,"\n");

  if (fp!=NULL)
  {
	fprintf(fp,"%s",buf_formated);
	fflush(fp);
  }

ended:
  return 0;
}

int log_printf_hex(FILE *fp,int level, unsigned char *buf, int buflen)
{
  char buf_formated[100000];
  int i,logpos=0;
	unsigned int tsec,tmilli;
	struct timespec t1;
        (void)clock_gettime(CLOCK_REALTIME, &t1);
        tsec=(unsigned int)t1.tv_sec;
	tmilli=(int)t1.tv_nsec/1000000;
  switch (log_level_chosen)
  {
	case LOG_DEBUG2: if (level==LOG_DEBUG2)
			 {	sprintf(buf_formated,"%u.%03d DEB2  ",tsec,tmilli);
				logpos=1;
			 }
	case LOG_DEBUG: if (level==LOG_DEBUG)
			{	sprintf(buf_formated,"%u.%03d DEB1  ",tsec,tmilli);
				logpos=1;
			}
	case LOG_INFO: if (level==LOG_INFO)
			{	sprintf(buf_formated,"%u.%03d INFO  ",tsec,tmilli);
				logpos=1;
			}
	case LOG_WARNING: if (level==LOG_WARNING)
			{	sprintf(buf_formated,"%u.%03d WARN  ",tsec,tmilli);
				logpos=1;
			}
	case LOG_ERROR: if (level==LOG_ERROR)
			{	sprintf(buf_formated,"%u.%03d ERRO  ",tsec,tmilli);
				logpos=1;
			}
  }

  if (!logpos)  goto ended;
  sprintf(buf_formated,"0x");
  for (i=0;i<buflen;i++) sprintf(buf_formated,"%s%02X",buf_formated,buf[i]);
  strcat(buf_formated,"\n");

  if (fp!=NULL)
  {
	fprintf(fp,"%s",buf_formated);
	fflush(fp);
  }

ended:
  return 0;
}

int log_printf(FILE *fp,int level, const char *format, ...)
{
#define CUT_LOG_LINE_TO 2048
  va_list al;
  char bufPre[100]; /* allow for time, LOG_... etc */
  char buf[CUT_LOG_LINE_TO+1];
  unsigned len_buf;
  int logpos=0;
  unsigned int tsec,tmilli;
  struct timespec t1;
  (void)clock_gettime(CLOCK_REALTIME, &t1);
  tsec=(unsigned int)t1.tv_sec;
  tmilli=(int)t1.tv_nsec/1000000;

  va_start(al,format);
  len_buf = vsnprintf(buf,CUT_LOG_LINE_TO,format,al);
  va_end(al);

  switch (log_level_chosen)
  {
	case LOG_DEBUG2: if (level==LOG_DEBUG2)
			 {	sprintf(bufPre,"%u.%03d DEB2  ",tsec,tmilli);
				logpos=1;
			 }
	case LOG_DEBUG: if (level==LOG_DEBUG)
			{	sprintf(bufPre,"%u.%03d DEB1  ",tsec,tmilli);
				logpos=1;
			}
	case LOG_INFO: if (level==LOG_INFO)
			{	sprintf(bufPre,"%u.%03d INFO  ",tsec,tmilli);
				logpos=1;
			}
	case LOG_WARNING: if (level==LOG_WARNING)
			{	sprintf(bufPre,"%u.%03d WARN  ",tsec,tmilli);
				logpos=1;
			}
	case LOG_ERROR: if (level==LOG_ERROR)
			{	sprintf(bufPre,"%u.%03d ERRO  ",tsec,tmilli);
				logpos=1;
			}
			break;
	default:	if (fp) fprintf(fp,"LOG_LEVEL_INCOMPATIBLE s=%s\n",buf);
  }

  if (logpos) if (fp) {fprintf(fp,"%s%s\n",bufPre,buf);fflush(fp);}

  return len_buf;
}

/*
    Use errors only to stderr
 */
FILE *log_start(char *who,int level_chosen)
{
  time_t now = time(NULL);
  
  if (log_level_chosen==LOG_UNKNOWN) log_level_chosen=level_chosen;

  log_printf(stderr,LOG_WARNING,"%s: log start: %s",who,ctime(&now));

  return(stderr);
}
void log_end(char *who)
{
  time_t now = time(NULL);
  log_printf(stderr,LOG_DEBUG2,"%s: log ends: %s",who,ctime(&now));
}

FILE *log_open_file(int level_chosen, char *file)
{
  char st[500];
  time_t now = time(NULL);
  FILE *fp;
  
  if (log_level_chosen==LOG_UNKNOWN) log_level_chosen=level_chosen;

  sprintf(st,"%s",file);
  fp = fopen(st,"a+");

  if (fp)
	{
    	fprintf(fp,"log start: %s",ctime(&now));
	fflush(fp);

	return(fp);
	}
  else
	{
    fprintf(stderr,"Could not open %s for output. Proceeding without a log file.\n",st);
	return(NULL);
	}
}

void change_log_level(int level_chosen)
{
  /* Allows changing log level during run */
  log_level_chosen=level_chosen;
}

void log_close_file(FILE *fp)
{
  time_t now = time(NULL);

  if (fp) 
  {
    fprintf(fp,"log ends: %s",ctime(&now));
    fclose(fp);
  }
}

void bin_to_hex_str(unsigned char *b,int blen, char *s)
{
	int i;
	for (i=0;i<blen;i++) sprintf(s,"%s%02X",s,b[i]);
}

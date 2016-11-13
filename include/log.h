
#include <stdio.h>

typedef void (*logfunc_t)(int,const char*);

void fc_strerr(int errorcode,char *return_str,int return_str_len);
int log_printf(FILE *,int , const char *, ...);
int log_printf_hex_title(FILE *,int , char *, unsigned char *, int);
int log_printf_hex(FILE *,int , unsigned char *, int);

void log_set_function(logfunc_t logfunc);

/*used for errors to stderr only*/
FILE * log_start(char*,int);
void log_end(char*);

FILE * log_open_file(int,char*);
void log_close_file(FILE *);

void bin_to_hex_str(unsigned char *b, int blen, char *s);

enum {
  LOG_DEBUG2,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR,
  LOG_UNKNOWN
};

extern const char ANSI_RESET[];
extern const char ANSI_RED[];
extern const char ANSI_GREEN[];
extern const char ANSI_YELLOW[];
extern const char ANSI_BLUE[];
extern const char ANSI_MAGENTA[];
extern const char ANSI_CYAN[];
extern const char ANSI_WHITE[];

#define print_line_num() fprintf(stderr,"%s[%i]\n",__FILE__,__LINE__);
/*
FILE * log_start(char*,int);
void log_end(char*);

FILE * log_open_file(int,char*);
void log_close_file(FILE *);

void bin_to_hex_str(unsigned char *b, int blen, char *s);

enum {
  LOG_DEBUG2,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR,
  LOG_UNKNOWN
};

extern const char ANSI_RESET[];
extern const char ANSI_RED[];
extern const char ANSI_GREEN[];
extern const char ANSI_YELLOW[];
extern const char ANSI_BLUE[];
extern const char ANSI_MAGENTA[];
extern const char ANSI_CYAN[];
extern const char ANSI_WHITE[];

#define print_line_num() fprintf(stderr,"%s[%i]\n",__FILE__,__LINE__);
*/

#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int block_all_signals(FILE *fplog);

int block_signal(FILE *fplog,int sig);

int unblock_signal(FILE *fplog,int sig);

int set_signal(FILE *fplog,int sig,void (*func)(int));


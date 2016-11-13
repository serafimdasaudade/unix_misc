#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "log.h"
int open_or_create_fifo_for_read(FILE *fplog,char *name);
int open_or_create_fifo_for_write(FILE *fplog,char *name);


#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

extern int read_and_wait_for_num_bytes(FILE *fplog, int fd, unsigned char *buf, int numbytes,double timeout);


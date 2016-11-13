/*****************************************************
 * rs232.h
 *
 *  Created on: Oct 07, 2013
 *      Author: Luis Santos
 *      email: luis.santos@ngns-is.com
 *		Project: 
 *
 ******************************************************/

#include <termios.h> 
#include <stdio.h> 



int writeport(int fd, char *chars);
int readport(int fd, char *result);
int getbaud(int fd);
int initport(int fd) ;
int initport_57600(int fd);

int close_rs232(FILE *fplog,int fd);
int open_rs232(FILE *fplog,char *porta,int speed);
int flush_rs232(FILE *fplog,int fd);
int rtsdtr_rs232(FILE *fplog,int fd);
int read_rs232(FILE *fplog,int fd, unsigned char* buf,int nb);
int read_rs232_wait_numbytes(FILE *fplog,int fd, unsigned char* buf,int numbytes);
int write_rs232(FILE *fplog,int fd, unsigned char* send_bytes,int number_bytes);
int read_rs232_timeout(FILE *fplog,int fd, unsigned char* buf,int nb,int timeout_ms);

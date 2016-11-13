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

int close_i2c(FILE *fplog,int fd);
int open_i2c(FILE *fplog,char *porta);
int set_address_salve_i2c(FILE *fplog,int fd,char addr);
int flush_i2c(FILE *fplog,int fd);
int rtsdtr_i2c(FILE *fplog,int fd);
int read_i2c(FILE *fplog,int fd, unsigned char* buf,int nb);
int read_i2c_wait_numbytes(FILE *fplog,int fd, unsigned char* buf,int numbytes);
int write_i2c(FILE *fplog,int fd, unsigned char* send_bytes,int number_bytes);
int read_i2c_timeout(FILE *fplog,int fd, unsigned char* buf,int nb,int timeout_ms);
/*
Buffer
	\param nb number of bytes want to read
	\param timeout_ms Timeout in ms
	\retval -1 Timer start error or read i2c error
	\retval -2 Timeout
	\retval res OK
	\warning this timeout is not thread safe
*/
/*! \fn int write_i2c(FILE *fplog,int fd, unsigned char* send_bytes,int number_bytes)
	\brief Write to i2c 
	\param fplog The handler for logs
	\param fd The descriptor of i2c
	\param buf Buffer
	\param nb number of bytes want to write
	\retval -1 Error
	\retval wr OK
*/


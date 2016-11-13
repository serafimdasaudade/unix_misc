
/*
 * can.c
 * For CANUSB
 *
 */

#define FT_OK 0
#define FT_ERROR -1
#define FT_PURGE_RX         1
#define FT_PURGE_TX         2

#include <stdio.h>
#include <string.h>

#include "can.h"
#include "log.h"
#include "timers.h"

int FT_Purge (struct ftdi_context *pftdi_d, DWORD dwMask)
{
	ftdi_usb_purge_buffers(pftdi_d);	//limpa buffer RX e TX
	return (FT_OK);
}

int FT_Write (FILE *fplog,struct ftdi_context *pftdi_d, LPVOID lpBuffer, int dwBytesToWrite, int *lpdwBytesWritten)
{
	int f;
	char stemp[1000];

	log_printf(fplog,LOG_DEBUG,"CAN: Write. %d bytes",dwBytesToWrite );
	f = ftdi_write_data(pftdi_d, (unsigned char *)lpBuffer, dwBytesToWrite);
	if (f < 0)
	{
		log_printf(fplog,LOG_ERROR,"FT_Write failed, error %d (%s)\n",(int)f, ftdi_get_error_string(pftdi_d));
                return FT_ERROR;
	}
	log_printf(fplog,LOG_DEBUG,"CAN: CS After Write. %d bytes",dwBytesToWrite );
	*lpdwBytesWritten=f;
	sprintf(stemp,"CAN: Written. %d (%d)bytes: ",*lpdwBytesWritten ,dwBytesToWrite);
	log_printf_hex_title(fplog,LOG_DEBUG,stemp,(unsigned char *)lpBuffer,(int)*lpdwBytesWritten );
	if (dwBytesToWrite!=*lpdwBytesWritten)
	{
		log_printf(fplog,LOG_ERROR,"FT_Write failed, nb_write=%d nb_written=%d",dwBytesToWrite,*lpdwBytesWritten);
                return FT_ERROR;
	}
	return (FT_OK);
}

void FT_Close (FILE *fplog, struct ftdi_context *pftdi_d){

  int ret = 0;

	if ((ret = ftdi_usb_close(pftdi_d)) < 0){
		log_printf(fplog,LOG_ERROR, "unable to close ftdi device: %d (%s)\n", ret, ftdi_get_error_string(pftdi_d));
	    return;
	}

	ftdi_deinit(pftdi_d);
}

int FT_New_GetStatus (FILE *fplog, struct ftdi_context *pftdi_d,LPDWORD lpdwAmountInRxQueue)
{

	/*
        	//ftdi_poll_read(pftdi_d, 0);
        	//ftdi_poll_write(pftdi_d, 0);

        	//lpdwAmountInRxQueue = ftdib_size(pftdi_d->readbuffer);
        	//log_printf(fplog,LOG_DEBUG,"CAN CS x=%ld",*lpdwAmountInRxQueue);
        	if (*lpdwAmountInRxQueue < 0) {
                	log_printf(fplog,LOG_ERROR,"Failed to check RX data (%s)\n", ftdi_get_error_string(pftdi_d));
                	return FT_ERROR;
        	}
	*/

        log_printf(fplog,LOG_DEBUG,"CAN CS readbuf_remain=%ld readbuf_chunksize=%ld writebuf_chunksize=%ld",
			pftdi_d->readbuffer_remaining,
			pftdi_d->readbuffer_chunksize,
			pftdi_d->writebuffer_chunksize
			);
	*lpdwAmountInRxQueue=pftdi_d->readbuffer_remaining;

        return FT_OK;
}

int FT_GetStatus (FILE *fplog, struct ftdi_context *pftdi_d,LPDWORD lpdwAmountInRxQueue, LPDWORD lpdwAmountInTxQueue, LPDWORD lpdwEventStatus){



	if (((void*)pftdi_d == NULL)){
		 //ftdi_error_return(-2, "USB device unavailable 1");
		 log_printf(fplog,LOG_ERROR,"CAN: USB device unavailable 1 \n");
		 return 1;
	 }

	 if ( ((void*)pftdi_d->usb_dev == NULL)){
	 		 //ftdi_error_return(-2, "USB device unavailable 2");
	 		 //printf("USB device unavailable");
		 log_printf(fplog,LOG_ERROR,"USB device unavailable 2 \n");
				 return 1;
	 	 }

	//ftdi_read_data(struct ftdi_context *ftdi, unsigned char *buf, int size)
	 log_printf(fplog,LOG_DEBUG2,"CAN: leu %d bytes do buffer\n",pftdi_d->readbuffer_remaining);
	 *lpdwAmountInRxQueue = pftdi_d->readbuffer_remaining;
	 *lpdwAmountInTxQueue = 0;
	 *lpdwEventStatus = 0;
	 return (FT_OK);
}

int FT_Read (struct ftdi_context *pftdi_d,LPVOID lpBuffer, DWORD dwBytesToRead, int *lpdwBytesReturned, unsigned int *lpdwBytesRemaining)

{
	/* NOTE: non blocking read
		 (actually it is blocking but with latency of 16ms
         */
	int f;

	f = ftdi_read_data(pftdi_d, (unsigned char *)lpBuffer, dwBytesToRead);
	*lpdwBytesReturned=f;
	*lpdwBytesRemaining=pftdi_d->readbuffer_remaining;

	return (FT_OK);
}

///////////////////////////////////////////////////////////////////////////////
// open_can
//   SerialNum can be NULL if only one device available
///////////////////////////////////////////////////////////////////////////////

int open_can(FILE *fplog, struct ftdi_context *pftdi_d,char *SerialNum, int nSpeed )
{
	unsigned char stat_can;
	int nBytesWritten;
	int nBytesRead;
	int status;
	int ret;
	unsigned int nRemain;
	char stemp[1000];

	// Note!
	// The second ver of open should work from ver0.4.9 it may be prefered
	// in many situations. On Fedora Core 4, kernal 2.6.15 it fails however.

	if (ftdi_init(pftdi_d) < 0){

		log_printf(fplog,LOG_ERROR,"ftdi_init failed");
		return 1;
	}

/*
	//if ((ret = ftdi_usb_open(pftdi_d, 0x0403, 0x6001)) < 0)
	//if ((ret = ftdi_usb_open(pftdi_d, 0x0403, 0xffa8)) < 0)
*/
	if ((ret = ftdi_usb_open_desc(pftdi_d, 0x0403, 0xffa8,NULL,SerialNum)) < 0){
		/*
		This can fail if the ftdi_sio driver is loaded
		use lsmod to check this and rmmod ftdi_sio to remove
		also rmmod usbserial
		*/
		log_printf(fplog,LOG_ERROR,"unable to open ftdi device: %d (%s)\n", ret, ftdi_get_error_string(pftdi_d));
		return 2;
	}

	FT_Purge( pftdi_d, FT_PURGE_RX | FT_PURGE_TX );

	int cnt=100;
	char buf[100];
	while (cnt--)
	{
		sprintf( buf, "\r\r\r" );
		if ( FT_OK != ( status = FT_Write( fplog, pftdi_d, buf, strlen( buf ), &nBytesWritten ) ) ) {
			log_printf(fplog,LOG_ERROR,"can: Failed to write '\r\r' command. return code = %d\n", (int)status );
		return 3;
		}

		if ( !( FT_OK == FT_Read( pftdi_d, buf, 100, &nBytesRead, &nRemain ) ) )
		{
			log_printf(fplog,LOG_ERROR,"CAN: Read failed\n");
			return 4;
		}
		if (nBytesRead==3) goto continuar;
	}
	log_printf(fplog,LOG_ERROR,"CAN: cannot sincronize\n");
	return 5;
continuar:
	sprintf(stemp,"CAN: After sending CRCRCR recvd %d bytes: ", nBytesRead);
	log_printf_hex_title(fplog,LOG_INFO,stemp,(unsigned char*)buf,nBytesRead);
	

	// Set baudrate
	sprintf( buf, "S%d\r", nSpeed );
	if ( !( FT_OK == FT_Write( fplog, pftdi_d, buf, strlen(buf), &nBytesWritten) ) ) {
		log_printf(fplog,LOG_ERROR,"Write failed of Speed '%s'",buf);
		return 5;
	}
	log_printf(fplog,LOG_INFO,"Escreveu %d bytes: %s", nBytesWritten, buf );

	if ( !( FT_OK == FT_Read( pftdi_d, buf, 1, &nBytesRead, &nRemain ) ) )
	{
		log_printf(fplog,LOG_ERROR,"CAN: Read failed response speed");
		return 6;
	}
	if ( (nBytesRead>1) || (buf[0]!=13) )
	{
		sprintf(stemp,"CAN: After sending speed recvd %d bytes: ", nBytesRead);
		log_printf_hex_title(fplog,LOG_INFO,stemp,(unsigned char*)buf,nBytesRead);
		//CS ver mais tarde return 7;
	}
	
	strcpy( buf, "O\r" );
	if ( !( FT_OK == FT_Write( fplog, pftdi_d, buf, strlen(buf), &nBytesWritten ) ) ) {
		log_printf(fplog,LOG_ERROR,"Write failed of Open can '%s'",buf);
		return 8;
	}

	if ( !( FT_OK == FT_Read( pftdi_d, buf, 1, &nBytesRead, &nRemain ) ) )
	{
		log_printf(fplog,LOG_ERROR,"CAN: Read failed response to Open");
		return 9;
	}
	if ( (nBytesRead>1) || (buf[0]!=13) )
	{
		sprintf(stemp,"CAN: After sending open recvd %d bytes: ", nBytesRead);
		log_printf_hex_title(fplog,LOG_ERROR,stemp,(unsigned char*)buf,nBytesRead);
		//CS ver mais tarde return 10;
	}

	stat_can=status_can(fplog, pftdi_d); // clears error flag

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// close_can
///////////////////////////////////////////////////////////////////////////////

int close_can(FILE *fplog, struct ftdi_context *pftdi_d )
{
  char buf[80];
  unsigned long size;
  int retLen;

  // Close device
  	  FT_Purge( pftdi_d, FT_PURGE_RX | FT_PURGE_TX );
  	  strcpy( buf, "C\r" );
  	  size = 2;
  	  if ( !( FT_OK == FT_Write(fplog,  pftdi_d, buf, size, &retLen ) ) ) {
  		  return -1;
  	  }

  	  FT_Close(fplog, pftdi_d);

  	  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// send_can
///////////////////////////////////////////////////////////////////////////////

int send_can(FILE *fplog, struct ftdi_context *pftdi_d, CANMsg *pmsg )
{
	int i;
	unsigned char txbuf[80];
	int retLen;
	//DWORD nb_RX;

  	if ( pmsg->flags & CANMSG_EXTENDED )
	{
  		if ( pmsg->flags & CANMSG_RTR )
		{
  			  //CARLOS ALTEROU dia 24 de Dezembro
  			  //sprintf( txbuf, "R%08.8lX%i", pmsg->id, pmsg->len );
  			  //CS alter 17 janeiro sprintf( txbuf, "R%08.8ld%i", pmsg->id, pmsg->len );
  			  sprintf( (char*)txbuf, "R%8.8lX%i", pmsg->id, pmsg->len );
  			  pmsg->len = 0;
  		}
  		else
		{
  			  //CARLOS ALTEROU dia 24 de Dezembro
  			  //sprintf( txbuf, "T%08.8lX%i", pmsg->id, pmsg->len );
  			  //CS 17 Jan sprintf( txbuf, "T%08.8ld%i", pmsg->id, pmsg->len );
  			  sprintf( (char*)txbuf, "T%8.8lX%i", pmsg->id, pmsg->len );
  		}
  	}
  	else
	{
  		if ( pmsg->flags & CANMSG_RTR )
		{
  			  //CARLOS ALTEROU dia 24 de Dezembro
  			  //sprintf( txbuf, "r%03.3lX%i", pmsg->id, pmsg->len );
  			  //CS 17 Jan sprintf( txbuf, "r%03.3ld%i", pmsg->id, pmsg->len );
  			  sprintf( (char*)txbuf, "r%3.3ld%i", pmsg->id, pmsg->len );
  			  pmsg->len = 0; // Just dlc no data for RTR
  		}

  		else
		{
  			  //CARLOS ALTEROU dia 24 de Dezembro
  			  //sprintf( txbuf, "t%03.3lX%i", pmsg->id, pmsg->len );
  			  //CS 17 Jan sprintf( txbuf, "t%03.3ld%i", pmsg->id, pmsg->len );
  			  sprintf( (char*)txbuf, "t%3.3ld%i", pmsg->id, pmsg->len );
  		}
  	}

  	if ( pmsg->len )
	{
  		  char hex[5];
  		  for ( i= 0; i< pmsg->len; i++ ) {
  			  sprintf( hex, "%02X", (unsigned char)pmsg->data[i] );
  			  strcat( (char*)txbuf, hex );
  		  }
	}

	// Add CR
	strcat( (char*)txbuf, "\r" );

	//if (FT_OK==FT_New_GetStatus (fplog, pftdi_d,&nb_RX))
	//	log_printf(fplog,LOG_DEBUG2,"CAN: In Queue nb_RX=%d",nb_RX);

	log_printf(fplog,LOG_DEBUG,"CAN: Sending  = %s",txbuf);

	// Transmit fram
  	if ( FT_OK != FT_Write(fplog,pftdi_d,txbuf,strlen((char*)txbuf),&retLen))
	{
		log_printf(fplog,LOG_ERROR,"CAN: ERROR......Writing to CAN");
		return -1;
	}
  	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// recv_can
// if msg_read<>0 then msg has valid message
// if msg_send_resp<>0 during receiving of message also received resp to send
////////////////////////////////////////////////////////////////////////////////

int recv_can(FILE *fplog, struct ftdi_context *pftdi_d, CANMsg *msg,int *msg_read,int *msg_send_resp,char *who,int timeout_ms)
{
	//int i,j;
	unsigned int nRemain;
	int nRcvCnt;
	char c;
	char gbufferRx[ BUF_SIZE ];
	//DWORD nb_RX;
	char msgReceiveBuf[80];
	int cntMsgRcv = 0;
	int state = CANUSB_STATE_NONE;
	int msglen;
	int tcount_cs;
	double start_time,timenow;

	*msg_read=0;
	*msg_send_resp=0;
	memset (msg,'\0',sizeof(CANMsg));

	//if (FT_OK==FT_New_GetStatus (fplog, pftdi_d,&nb_RX))
	//	log_printf(fplog,LOG_DEBUG2,"CAN: (%s) In Queue nb_RX=%d",who,nb_RX);

	msglen=0;
	tcount_cs=0;

	nRcvCnt=0;
	gettime_ms(fplog,&start_time);	
	while (1)
	{
		if (nRcvCnt==0)
		{
			/* In loop and not receiving */
			gettime_ms(fplog,&timenow);	
			if ((timenow-start_time)>timeout_ms)
			{
				log_printf(fplog,LOG_WARNING,"CAN: Timeout. now=%ld start=%ld Timeout_ms=%d\n",timenow,(long int)start_time,timeout_ms);
				goto timeout;
			}
			//sleep_ms(fplog,5); /*descansar CPU CS ?? */
		}

		if (FT_OK != FT_Read(pftdi_d,gbufferRx,1,&nRcvCnt,&nRemain))
		{
			log_printf(fplog,LOG_ERROR,"CAN: (%s) Nao leu character",who);
			goto fail;
		}
		if (nRcvCnt==0)
		{
			goto next;
		}
		else
		{
			/*too slow for car
			*/
			//if (strcmp(who,"VM"))
			//log_printf(fplog,LOG_WARNING,"CANCS: (%s) %02X",who,gbufferRx[ 0 ]);
		}
		if (nRcvCnt>1)
		{
			log_printf(fplog,LOG_ERROR,"CAN: (%s) ERRO Too much bytes (%d) ",who,nRcvCnt);
			goto fail;
		}
		c = gbufferRx[ 0 ];

		if ( CANUSB_STATE_NONE == state )
		{
			//CS acho q funciona assim if (('t'==c)||('T'==c) )
			if (('t'==c)||('T'==c)||('r'==c)||('R'==c) )
			{
			  state = CANUSB_STATE_MSG;
			  memset( msgReceiveBuf, 0, sizeof( msgReceiveBuf ) );
			  msgReceiveBuf[ 0 ] = c;
			  cntMsgRcv = 1;
 			  tcount_cs=4;
			}
			else
			{
				if (*msg_send_resp==-999)
				{
					log_printf(fplog,LOG_WARNING,"CANCS: (%s) IGNORA_POR_PEDIDO %02X",who,c);
					nRcvCnt=0; /* Processa timeout */
				}
				else
				{
					if ('z'==c)
					{
						*msg_send_resp='z';
						/* Next should receive CR (0d) but
						   it will be ignored
						 */
						goto ok;
					}
					else if (7==c) 
					{
						*msg_send_resp=7;
						goto ok;
					}
					else
					{
						//CSlog_printf(fplog,LOG_WARNING,"CANCS: (%s) IGNORA %02X",who,c);
						nRcvCnt=0; /* Processa timeout */
					}
				}
			}
		}

		else if ( CANUSB_STATE_MSG == state )
		{
			if (tcount_cs==0)
			{
				msglen=c-'0';
			}
			else tcount_cs--;

			msgReceiveBuf[ cntMsgRcv++ ] = c;
			if (cntMsgRcv==80)
			{
			 	log_printf(fplog,LOG_ERROR,"CAN: (%s) SERIOUS Internal Error 9012\n",who);
				goto fail;
			}

			/* Tolera receber mais caracteres do que ditos no len
			   mas nao menos
				Implementar timeout
				CS
			 */
			if ( ( 0x0d == c ) && ((cntMsgRcv-6)>=msglen) )
			{ //CR
			
			 	log_printf(fplog,LOG_DEBUG,"CAN: (%s) IMPORTANT captured = %s\n",who,  msgReceiveBuf);

				if ( canusbToCanMsg(fplog,msgReceiveBuf, msg ) )
				{
					log_printf(fplog,LOG_ERROR,"CAN: (%s) Message conversion failed!\n",who);

					state = CANUSB_STATE_NONE;

					goto fail;
				}

				if ( msg->flags & CANMSG_EXTENDED  )
				{
					  log_printf(fplog,LOG_DEBUG2,"CAN: (%s) Extended ",who);

				}
				else{
				  log_printf(fplog,LOG_DEBUG2,"CAN: (%s) Standard ",who);
				}

				if ( (!strcmp(who,"VM")) && ( msg->id==0x102 ) && ((msg->data[4]>31) || (msg->data[5]>7)) )
				{
				char stemp[1000];
				sprintf(stemp, "CAN: (%s) IMPORTANT received: id=%lu(0x%02X) len=%d timestamp=%02X ",who, msg->id, (unsigned int)msg->id, msg->len, (unsigned int)msg->timestamp);
			  	log_printf_hex_title(fplog,LOG_INFO,stemp, msg->data, msg->len);
				}

				*msg_read=1;
				state = CANUSB_STATE_NONE;
				goto ok;

			} // full message

		} // STATE_MSG

next:		;
	} // for each char

ok:
	log_printf(fplog,LOG_DEBUG,"CAN: (%s) Recv CAN OK",who);
	return 0;
fail:	log_printf(fplog,LOG_ERROR,"CAN: (%s) Recv CAN Fail",who);
	return -1;
timeout:
	//Timeout is not always an Error, dont print
	//log_printf(fplog,LOG_ERROR,"CAN: (%s) Recv CAN Timeout",who);
	return -2;
}

///////////////////////////////////////////////////////////////////////////////
// canusbToCanMsg
////////////////////////////////////////////////////////////////////////////////

int canusbToCanMsg(FILE *fplog, char *p, CANMsg *pMsg )
{
  int val;
  int i;
  short data_offset;   // Offset to dlc byte
  char save;

  	  if ( 't' == *p ) {
  		  // Standard frame
  		  pMsg->flags = 0;
  		  data_offset = 5;
  		  pMsg->len = p[ 4 ] - '0';
  		  p[ 4 ] = 0;
  		  sscanf( p + 1, "%lx", &pMsg->id  );
  	  }
  	  else if ( 'r' == *p ) {
  		  // Standard remote  frame
  		  pMsg->len = p[ 4 ] - '0';
  		  pMsg->flags = CANMSG_RTR;
  		  //data_offset = 5 - 1;// To make timestamp work
  		  data_offset = 5;
  		  //save = p[ 4 ];
  		  p[ 4 ] = 0;
  		  sscanf( p + 1, "%lx", &pMsg->id  );
  		  //p[ 4 ] = save;
  	  }
  	  else if ( 'T' == *p ) {
  		  // Extended frame
  		  pMsg->flags = CANMSG_EXTENDED;
  		  data_offset = 10;
  		  pMsg->len = p[ 9 ] - '0';
  		  p[ 9 ] = 0;
  		  sscanf( p + 1, "%lx", &pMsg->id );
  	  }
  	  else if ( 'R' == *p ) {
  		  // Extended remote frame
  		  pMsg->flags = CANMSG_EXTENDED | CANMSG_RTR;
  		  //data_offset = 10 - 1;// To make timestamp work
  		  data_offset = 10;
  		  pMsg->len = p[ 9 ] - '0';
  		  //save = p[ 9 ];
  		  p[ 9 ] = 0;
  		  sscanf( p + 1, "%lx", &pMsg->id );
  		  //p[ 9 ] = save;
  	  }

  	  save = *(p + data_offset + 2 * pMsg->len );

  	  // Fill in data
  	  if ( !( pMsg->flags & CANMSG_RTR ) ) {
  		  for ( i= MIN( pMsg->len, 8); i > 0; i-- ) {
  			  *(p + data_offset + 2 * (i-1) + 2 )= 0;
  			  sscanf( p + data_offset + 2 * (i-1), "%x", &val );
  			  pMsg->data[ i - 1 ] = val;
  		  }
  	  }

  	  *(p + data_offset + 2 * pMsg->len ) = save;

  	  if ( !( pMsg->flags & CANMSG_RTR ) ) {
  		  // If timestamp is active - fetch it
  		  if ( 0x0d != *( p + data_offset + 2 * pMsg->len ) ) {
  			  p[ data_offset + 2 * ( pMsg->len ) + 4 ] = 0;
  			  sscanf( ( p + data_offset + 2 * ( pMsg->len ) ), "%x", &val );
  			  pMsg->timestamp = val;
  		  }
  		  else {
  			  pMsg->timestamp = 0;
  		  }
  	  }
  	  else {

  		  if ( 0x0d != *( p + data_offset ) ) {
  			  p[ data_offset + 4 ] = 0;
  			  sscanf( ( p + data_offset ), "%x", &val );
  			  pMsg->timestamp = val;
  		  }
  		  else {
  			  pMsg->timestamp = 0;
  		  }
  	  }

  	  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
//

static void getVersionInfo(FILE *fplog, struct ftdi_context *pftdi_d)
{
  int status;
  char buf[80];
  char *p;
  int nBytes;
  unsigned long eventStatus;
  unsigned long nRxCnt;// Number of characters in receive queue
  unsigned long nTxCnt;// Number of characters in transmit queue
unsigned int nRemain;

  memset( buf, 0, sizeof( buf ) );
  printf("Get version.\n");
  printf("============\n");

  FT_Purge( pftdi_d, FT_PURGE_RX | FT_PURGE_TX );

  sprintf( buf, "V\r" );
  if ( FT_OK != ( status = FT_Write(fplog,  pftdi_d, buf, strlen( buf ), &nBytes ) ) ) {
    log_printf(fplog,LOG_ERROR,"Error: Failed to write command. return code = %d\n", (int)status );
    return;
  }

  // Check if there is something to receive
  while ( 1 ){

    if ( FT_OK == FT_GetStatus( fplog, pftdi_d, &nRxCnt, &nTxCnt, &eventStatus ) ) {

      // If there are characters to receive
      if ( nRxCnt ) {

	if ( FT_OK != ( status = FT_Read( pftdi_d, buf, nRxCnt, &nBytes, &nRemain ) ) ) {
	  log_printf(fplog,LOG_ERROR,"Error: Failed to read data. return code = %d\n", (int)status );
	  return;
	}

	p = buf;
	while ( *p ) {
	  if ( 0x0d == *p ) {
	    *p = 0;
	    break;
	  }
	  p++;
	}
	printf( "Version = %s\n", buf );
	break;

      }

    }
    else {
      log_printf(fplog,LOG_ERROR,"Error: Failed to get status. return code = %d\n", (int)status );
      return;
    }

  }

}

///////////////////////////////////////////////////////////////////////////////////////
//

static void getSerialNumber( FILE *fplog, struct ftdi_context *pftdi_d, char *buf )
{
  int status;
  //char buf[80];
  char *p;
  int nBytesWritten;
  unsigned int nRemain;
  unsigned long eventStatus;
  unsigned long nRxCnt;// Number of characters in receive queue
  unsigned long nTxCnt;// Number of characters in transmit queue

  memset( buf, 0, sizeof( buf ) );
  printf("Get serial number.\n");
  printf("==================\n");

  FT_Purge( pftdi_d, FT_PURGE_RX | FT_PURGE_TX );

  sprintf( buf, "N\r" );
  if ( FT_OK != ( status = FT_Write(fplog,  pftdi_d, buf, strlen( buf ), &nBytesWritten ) ) ) {
    log_printf(fplog,LOG_ERROR,"Error: Failed to write command. return code = %d\n", (int)status );
    return;
  }

  // Check if there is something to receive
  while ( 1 ){

    if ( FT_OK == FT_GetStatus( fplog, pftdi_d, &nRxCnt, &nTxCnt, &eventStatus ) ) {

      // If there are characters to receive
      if ( nRxCnt ) {

	if ( FT_OK != ( status = FT_Read( pftdi_d, buf, nRxCnt, &nBytesWritten, &nRemain) ) ) {
	  log_printf(fplog,LOG_ERROR,"Error: Failed to read data. return code = %d\n", (int)status );
	  return;
	}

	p = buf;
	while ( *p ) {
	  if ( 0x0d == *p ) {
	    *p = 0;
	    break;
	  }
	  p++;
	}

	printf( "Serial = %s \n", buf  );
	break;

      }

    }
    else {
      log_printf(fplog,LOG_ERROR,"Error: Failed to get status. return code = %d\n", (int)status );
      return;
    }

  }

}

unsigned char status_can(FILE *fplog, struct ftdi_context *pftdi_d)
{
	char buf[80];
	unsigned char ctemp;
	int nBytesWritten;
	int nBytesRead;
	unsigned int nRemain;
	char stemp[1000];

	strcpy(buf, "F\r" );
	if ( !( FT_OK == FT_Write(fplog,  pftdi_d, buf, strlen(buf), &nBytesWritten ) ) ) {
		log_printf(fplog,LOG_ERROR,"Write failed of can status '%s'",buf);
		return -1;
	}

	if ( !( FT_OK == FT_Read( pftdi_d, buf, 4, &nBytesRead, &nRemain ) ) )
	{
		log_printf(fplog,LOG_ERROR,"CAN: Read failed response to F");
		return -1;
	}
	if ( (nBytesRead!=4) )
	{
		sprintf(stemp,"CAN: (1)After sending F recvd %d bytes: ", nBytesRead);
		log_printf_hex_title(fplog,LOG_ERROR,stemp,(unsigned char*)buf,nBytesRead);
	}
	if ( (nBytesRead!=4) || (buf[3]!=13)  || (buf[0]!='F') )
	{
		sprintf(stemp,"CAN: (2)After sending F recvd %d bytes: ", nBytesRead);
		log_printf_hex_title(fplog,LOG_ERROR,stemp,(unsigned char*)buf,nBytesRead);
	}
	log_printf(fplog,LOG_INFO,"Can open status string=%c%c", buf[1],buf[2]);
	sscanf(buf,"%*c%02x%*c",(unsigned int *)&ctemp);
	if (ctemp) log_printf(fplog,LOG_ERROR,"Can open status=%02x", ctemp);
	return(ctemp);
}
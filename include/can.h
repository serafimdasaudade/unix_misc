/*
 * can.h
 *
 *  Created on: Nov 07, 2013
 *      Author: Luis Santos
 *      email: luis.santos@ngns-is.com
 *		Project: 
 *
 *  Develped for CAN USB www.canusb.com
 *		*/

#include "FTDI/WinTypes.h"

#include "FTDI/ftdi.h"

#define ABS( a )(( (int) (a) < 0 ) ? ((a) ^ 0xffffffff) + 1 : (a) )

#define CANUSB_STATE_NONE  0
#define CANUSB_STATE_MSG   1


// CAN Frame
typedef struct {
  unsigned long id;         // Message id
  unsigned long timestamp;  // timestamp in milliseconds
  unsigned char flags;      // [extended_id|1][RTR:1][reserver:6]
  unsigned char len;        // Frame size (0.8)
  unsigned char data[ 8 ];  // Databytes 0..7
} CANMsg;

#define SPEED_10Kbit	0
#define SPEED_20KBITS	1
#define SPEED_50KBITS	2
#define SPEED_100KBITS	3
#define SPEED_125KBITS	4
#define SPEED_250KBITS	5
#define SPEED_500KBITS	6
#define SPEED_800KBITS	7
#define SPEED_1MBITS	8

#define BUF_SIZE 0x1000

// Message flags
#define CANMSG_NORMAL   0x00 // Extended CAN id
#define CANMSG_EXTENDED   0x80 // Extended CAN id
#define CANMSG_RTR        0x40 // Remote frame

// Prototypes
int canusbToCanMsg(FILE *fplog, char * p, CANMsg *pMsg );
int recv_can(FILE *fplog, struct ftdi_context *pftdi_d, CANMsg *msg,int *msg_read,int *msg_send_resp,char *who,int timeout_ms);
int open_can(FILE *fplog, struct ftdi_context *pftdi_d, char *SerialNum, int nSpeed );
int close_can(FILE *fplog, struct ftdi_context *pftdi_d );
unsigned char status_can(FILE *fplog, struct ftdi_context *pftdi_d);
int send_can(FILE *fplog, struct ftdi_context *pftdi_d, CANMsg *pmsg );
//void getSerialNumber( struct ftdi_context *pftdi_d, char *buf );

static void getSerialNumber(FILE *fplog, struct ftdi_context *pftdi_d, char *buf );


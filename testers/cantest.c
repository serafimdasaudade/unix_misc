#include "log.h"
//#include "misc.h"
#include "can.h"
FILE *fplog;

int main(int argc, char *argv[])
{
	int i;
	char c, buf[80];
	int log_level;
	CANMsg msgRcv;
	CANMsg msgSend;
	struct ftdi_context ftdi_d;

	fplog=log_open_file(LOG_DEBUG,"a.log");

	if ( !open_can( NULL, &ftdi_d, SPEED_500KBITS) ) {
		printf("Failed to open channel\n");
	}


	while ( 1 ) 
	{

		printf("\n\n\n");
		printf("=====================================================================\n");
		printf("               Lawicel AB -- CANUSB test application\n");
		printf("=====================================================================\n\n");

		/*   printf("S - Send test frames.\n");
		printf("R - Read five frames.\n");
		printf("N - Get Serial number.\n");
		printf("V - Get Version Information.\n");*/
		printf("S - Send frames.\n");
		printf("P - Get DATA CAN.\n");
		printf("Q - Quit application.\n");

		while ( 0x0a == ( c = getchar() ));

		if ( 'q' == c || 'Q' == c )		break;
		else if ( 'p' == c || 'P' == c  ) 
		{
		while(1)
		{
			recv_can(NULL, &ftdi_d, &msgRcv);
			if (msgRcv.len>0)
			{
				log_printf(NULL,LOG_INFO,"-------------RECEBIDO------------");
				log_printf(NULL,LOG_INFO, "ID=%X", msgRcv.id);
				log_printf(NULL,LOG_INFO,"LEN=%X",msgRcv.len);
				log_printf_hex(NULL,LOG_INFO,msgRcv.data,msgRcv.len);
			}
		}
		}

		else if ( 's' == c || 'S' == c  )
		{
			unsigned char data_send[9];
			strcpy(data_send,"12345678");
			msgSend.id=101;
			msgSend.len=8;
			memcpy(msgSend.data,data_send,8);
			send_can(fplog, &ftdi_d, &msgSend );
		}


	}

	printf("Bye,bye....\n");
	close_can(NULL,&ftdi_d);

	return 0;
}


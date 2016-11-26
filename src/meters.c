#include "log.h"
#include "rs232.h"
#include "meters.h"

/*
	Frame
	0xAA,I1,I2,V1,V2,E1,E2,E3,E4,0xAB,I1,I2,V1,V2,E1,E2,E3,E4
*/
#define METER_DATA_SIZE		8
#define METER_FRAME_SIZE	18
#define HALF_METER_FRAME_SIZE	9

int meter_read_v_i_e(FILE *fplog, int fd, int *v, int *i, long *e)
{


	int cur,volt;
	long energ;
	int rc=0,rc1;
	unsigned char bufvi[9000],*p;

	cur=0;
	volt=0;	

	//flush buffer with old data*
	if (flush_rs232(fplog,fd)==-1) return(-1);

	while ( rc < METER_FRAME_SIZE*2 )
	{
		// 14 Setembro 2011 estva assim 
		//rc1=read_rs232(fplog,fd,bufvi+rc,1);
		rc1=read_rs232_timeout(fplog,fd,bufvi+rc,1,100);
		if (rc1<0)
		{
			log_printf(fplog,LOG_ERROR,"ME: Error reading Meter RC=%d\n",rc1);
			return(-1);
		}
		rc+=rc1;
	}
	log_printf(fplog,LOG_DEBUG,"ME: Read %d bytes\n",rc);

	p=bufvi+rc-1;


	while ( p>=(bufvi+METER_FRAME_SIZE) )
	{
		if ( (*p==0xAA)
			&& (*(p-HALF_METER_FRAME_SIZE)	==0xAB)
			&& (*(p-METER_FRAME_SIZE)	==0xAA)
		   )
		{
			if ( memcmp(
				p-METER_DATA_SIZE,
				p-(HALF_METER_FRAME_SIZE+METER_DATA_SIZE),
				METER_DATA_SIZE) )
			{
				log_printf(fplog,LOG_ERROR,"ME: 1\n");
				goto fail;
			}
			if ( *(p-METER_DATA_SIZE)==0xF0 )
			{
				// Note: Cur=F0F0 means AFE cannot Init 
				log_printf(fplog,LOG_ERROR,"ME: Error AFE cannot init\n");
				return(-3);
			}
			cur=*(p-METER_DATA_SIZE)<<8;
			cur+=*(p-METER_DATA_SIZE+1);
			//while Testing
			//if (*i==0) log_printf_hex_title(fplog,LOG_ERROR,"FINDBUG",bufvi,rc);
			volt=*(p-METER_DATA_SIZE+2)<<8;
			volt+=*(p-METER_DATA_SIZE+3);

			if (e!=NULL)
			{
				energ=*(p-METER_DATA_SIZE+4)<<24;
				energ=*(p-METER_DATA_SIZE+5)<<16;
				energ=*(p-METER_DATA_SIZE+6)<<8;
				energ+=*(p-METER_DATA_SIZE+7);
				*e=energ;
				//*e=round(energ/100);
			}

			*i=cur;
			*v=volt;
			//*v=round(volt/100);
			//*i=round(cur/100);

			return(0);
		}
fail:		p--;
	}
	log_printf(fplog,LOG_ERROR,"ME: Error Not enough data available(2) bytes=%d\n",rc);
	log_printf_hex(fplog,LOG_ERROR,bufvi,rc);
	return(-2); // Not found in frame, not enough data available 
}


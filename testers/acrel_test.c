#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <modbus.h>
#include "log.h"

#define LOG_FILE_METERS "a.log"
#define SLAVE_ADDRESS_ID 1

#define ADDR_VOLTAGE	0x0000
#define ADDR_CURRENT	0x0002
#define ADDR_POWER	0x0008
#define ADDR_ENERGY	0x000c

static char log_file_name[]=LOG_FILE_METERS;
static FILE* fplog=NULL;

char devname[200];

modbus_t *mc_config_modbus(char *device,int speed)
{
	log_printf(fplog,LOG_INFO ,"Device=%s\n",device);
	/* Configure connection */
	modbus_t *ctx;
	ctx = modbus_new_rtu(device,speed, 'N', 8, 1);
	modbus_set_debug(ctx, FALSE);
	if (ctx == NULL) {
	    log_printf(fplog,LOG_ERROR,"Unable to create the libmodbus context (%s) at (%d).\n",device,speed);
	    return (NULL);
	}

	/*CS if want broadcast send to
		MODBUS_BROADCAST_ADDRESS
	 */
	modbus_set_slave(ctx, SLAVE_ADDRESS_ID); 
	return(ctx);
}

float mc_read_modbus_acrel(modbus_t *ctx, int addr)
{
	uint16_t *tab_rp_registers;
	int nb, rc;
	float v=-1;

	if (modbus_connect(ctx) == -1)
	{
		log_printf(fplog,LOG_ERROR, "Addr=%d. Connection failed: %s\n",addr, modbus_strerror(errno));
		return -1;
	}

	/* Allocate and initialize the different memory spaces */
	nb = 2; // Read 2 bytes
	tab_rp_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
	memset(tab_rp_registers, 0, nb * sizeof(uint16_t));

	/* Read Register */
	rc = modbus_read_registers(ctx, addr, nb, tab_rp_registers);
	if (rc != nb)
	{
		log_printf(fplog,LOG_ERROR, "Addr=%d. Read Register failed: %s\n", addr,modbus_strerror(errno));
		return -1;
	}

	if (	(addr==ADDR_VOLTAGE) ||
		(addr==ADDR_CURRENT) ||
		(addr==ADDR_POWER)
	   )
	{
		v=(float)tab_rp_registers[0]*pow(10.0,(double)tab_rp_registers[1]-3);
	}
	if (addr==ADDR_ENERGY)
	{
		v=(tab_rp_registers[0]<<8)+tab_rp_registers[1];
	}
	
//printf("\t\tFirst %d\n",tab_rp_registers[0]);
//printf("\t\tSecond %d\n",tab_rp_registers[1]);

	/* Free the memory */
	free(tab_rp_registers);

	/* Close the connection */
	modbus_close(ctx);
	return(v);
}

void mc_free_modbus(modbus_t *ctx)
{
	if (ctx!=NULL) modbus_free(ctx);
}

int main(int argc, char *argv[])
{
	modbus_t *mb_ctx;
	float read_value;

	if (argc==2)
	{
		sprintf(devname,"/dev/ttyUSB%s",argv[1]);
	}
	else
	{
		printf("Usage\n");
		exit(0);
	}
	
	fplog=log_open_file(0,log_file_name);

	mb_ctx=mc_config_modbus(devname,9600);
	if (mb_ctx==NULL) return(-1);

	read_value=mc_read_modbus_acrel(mb_ctx,ADDR_VOLTAGE);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("VOLTAGE=%f\n",read_value);
	}

	read_value=mc_read_modbus_acrel(mb_ctx,ADDR_CURRENT);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("CURRENT=%f\n",read_value);
	}

	read_value=mc_read_modbus_acrel(mb_ctx,ADDR_ENERGY);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("ENERGY=%f\n",read_value);
	}

	mc_free_modbus(mb_ctx);
	return 0;
}

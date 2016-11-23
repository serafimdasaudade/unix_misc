#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <modbus.h>
#include "log.h"

#define LOG_FILE_METERS "a.log"
#define BAUDRATE        2400

#define SLAVE_ADDRESS_ID 1
#define ADDR_VOLTAGE    0x0000
#define ADDR_CURRENT	0x0006
#define ADDR_POWER      0x000C
#define ADDR_APOWER     0x0012
#define ADDR_RAPOWER    0x0018
#define ADDR_PFACTOR    0x001E
#define ADDR_PANGLE     0x0024
#define ADDR_FREQUENCY  0x0046
#define ADDR_IAENERGY   0x0048 //Imported active energy
#define ADDR_EAENERGY   0x004A //Exported active energy
#define ADDR_IRAENERGY  0x004C //Imported reactive energy
#define ADDR_ERAENERGY  0x004E //Exported reactive energy
#define ADDR_TAENERGY   0x0156 //Total activde Energy
#define ADDR_TRENERGY   0x0158 //Total reactive Energy

/*
typedef struct {
    
}SPowerMeterValues;
*/
modbus_t *config_modbus(FILE* fplog,char *device,int speed)
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

	modbus_set_slave(ctx, SLAVE_ADDRESS_ID); 
	return(ctx);
}

float read_modbus(FILE* fplog,modbus_t *ctx, int addr, int nb)
{
	uint16_t *tab_rp_registers;
	int rc;
	float v=-1;

	if (modbus_connect(ctx) == -1)
	{
		log_printf(fplog,LOG_ERROR, "Addr=%d. Connection failed: %s\n",addr, modbus_strerror(errno));
		return -1;
	}

	/* Allocate and initialize the different memory spaces */
	tab_rp_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));	
	memset(tab_rp_registers, addr, nb * sizeof(uint16_t));

	/* Read Register */
	rc = modbus_read_input_registers(ctx, addr, nb, tab_rp_registers);
    //rc = modbus_read_registers(ctx, addr, nb, tab_rp_registers);
	if (rc != nb)
	{
		log_printf(fplog,LOG_ERROR, "Addr=%d. Read Register failed: %s\n", addr,modbus_strerror(errno));
		return -1;
	}

	// swap LSB and MSB
    uint16_t tmp1 = tab_rp_registers[0];
    uint16_t tmp2 = tab_rp_registers[1];
    tab_rp_registers[0] = tmp2;
    tab_rp_registers[1] = tmp1;

    
    v=modbus_get_float(&tab_rp_registers[0]);
	
	/* Free the memory */
	free(tab_rp_registers);

	/* Close the connection */
	modbus_close(ctx);
	return(v);
}

void free_modbus(modbus_t *ctx)
{
	if (ctx!=NULL) modbus_free(ctx);
}

int main(int argc, char *argv[])
{
    static char log_file_name[]=LOG_FILE_METERS;
    static FILE* fplog=NULL;
    char devname[200];
	float read_value;

    modbus_t *mb_ctx;
    
	if (argc==2)
	{
		sprintf(devname,"/dev/ttyUSB%s",argv[1]);
	}
	else
	{
		printf("Usage\n");
		exit(0);
	}
	
	fplog=log_open_file(LOG_DEBUG2,log_file_name);

	mb_ctx=config_modbus(fplog,devname,BAUDRATE);
	if (mb_ctx==NULL) return(-1);

	read_value=read_modbus(fplog,mb_ctx,ADDR_VOLTAGE,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		//return -1;
	}
	else 
	{
		printf("VOLTAGE=%f Vac\n",read_value);
	}

	read_value=read_modbus(fplog,mb_ctx,ADDR_CURRENT,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("CURRENT=%f A\n",read_value);
	}

	read_value=read_modbus(fplog,mb_ctx,ADDR_POWER,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("ACTIVE POWER=%f W\n",read_value);
	}
	
	read_value=read_modbus(fplog,mb_ctx,ADDR_APOWER,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("APPARENT POWER=%f VA\n",read_value);
	}
	
	read_value=read_modbus(fplog,mb_ctx,ADDR_RAPOWER,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("REACTIVE POWER=%f VAR\n",read_value);
	}
	
	read_value=read_modbus(fplog,mb_ctx,ADDR_PFACTOR,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("POWER FACTOR=%f\n",read_value);
	}
	
	read_value=read_modbus(fplog,mb_ctx,ADDR_PANGLE,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("PHASE ANGLE=%f Deg\n",read_value);
	}
	
	read_value=read_modbus(fplog,mb_ctx,ADDR_FREQUENCY,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("FREQUENCY=%f Hz\n",read_value);
	}
	
	read_value=read_modbus(fplog,mb_ctx,ADDR_IAENERGY,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("IMPORTED ACTIVE ENERGY=%f Wh\n",read_value);
	}
	
	read_value=read_modbus(fplog,mb_ctx,ADDR_EAENERGY,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("EXPORTED ACTIVE ENERGY=%f Wh\n",read_value);
	}
	
	read_value=read_modbus(fplog,mb_ctx,ADDR_IRAENERGY,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("IMPORTED REACTIVE ENERGY=%f VARh\n",read_value);
	}
	
	read_value=read_modbus(fplog,mb_ctx,ADDR_ERAENERGY,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("EXPORTED REACTIVE ENERGY=%f VARh\n",read_value);
	}
	
	read_value=read_modbus(fplog,mb_ctx,ADDR_TAENERGY,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("TOTAL ACTIVE ENERGY=%f Wh\n",read_value);
	}

	read_value=read_modbus(fplog,mb_ctx,ADDR_TRENERGY,2);
	if (read_value==-1) 
	{
		printf("*****ERROR*****\n");
		return -1;
	}
	else 
	{
		printf("TOTAL REACTIVE ENERGY=%f VARh\n",read_value);
	}
	
	free_modbus(mb_ctx);
	return 0;
}

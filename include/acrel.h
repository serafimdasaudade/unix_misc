#include "../src/libmodbus/src/modbus.h"

#define ADDR_VOLTAGE    0x0000
#define ADDR_CURRENT    0x0002
#define ADDR_POWER      0x0008
#define ADDR_ENERGY     0x000c


modbus_t *mc_config_modbus(FILE *fplog,char *device,int speed);
float mc_read_modbus_acrel(FILE *fplog,modbus_t *ctx, int addr);
void mc_free_modbus(FILE *fplog,modbus_t *ctx);

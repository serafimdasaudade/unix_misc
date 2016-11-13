#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int gpio_init_port(FILE *fplog,int pnum);
extern int gpio_set_direction_IN(FILE *fplog,int pnum);
extern int gpio_set_direction_OUT(FILE *fplog,int pnum);
extern int gpio_set_value_LOW(FILE *fplog,int pnum);
extern int gpio_set_value_HIGH(FILE *fplog,int pnum);
extern int gpio_get_value(FILE *fplog,int pnum);
extern int gpio_wait_for_uevent(FILE *fplog, double timeout);

#define LOW	48
#define HIGH	49

#define EVENT_DEVICE "/dev/input/by-path/platform-gpio-keys-event"
#define FRIENDLY_ARM_EVENT_NUM_BYTES 64
#define FRIENDLY_ARM_EVENT_OFFSET_INTERRUPT_NUM 10

#define GPB0	(32+0)
#define GPB5	(32+5)
#define GPB6	(32+6)
#define GPB7	(32+7)
#define GPB8	(32+8)

#define GPE11	(128+11)
#define GPE13	(128+13)
#define GPE14	(128+14)
#define GPE15	(128+15)

#define GPF0	(160+0)
#define GPF1	(160+1)
#define GPF2	(160+2)
#define GPF3	(160+3)
#define GPF4	(160+4)
#define GPF5	(160+5)
#define GPF6	(160+6)

#define GPG0	(192+0)
#define GPG1	(192+1)
#define GPG2	(192+2)
#define GPG3	(192+3)
#define GPG5	(192+5)
#define GPG6	(192+6)

#define GPG7	(192+7)
#define GPG9	(192+9)
#define GPG10	(192+10)
#define GPG11	(192+11)
#define GPG13	(192+13)

#define GPH9    (224+9)
#define GPH10   (224+10)

#define BUZZER	GPB0	
#define LED1	GPB5
#define LED2	GPB6
#define LED3	GPB7
#define LED4	GPB8
#define K1	GPG0
#define K2	GPG3
#define K3	GPG5
#define K4	GPG6
#define K5	GPG7
//#define K6	GPG11 nunca consegui detectar um evento aqui


#define ADC0	0
#define ADC1	1
#define ADC2	2
#define ADC3	3

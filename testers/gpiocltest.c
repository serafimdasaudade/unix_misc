#include "mc_gpio.h"

int main(int argc,char **argv)
{
	int val;

	mc_gpio_init_port(NULL,GPF5);
	mc_gpio_set_direction_OUT(NULL,GPF5);

	if (argc!=2)
	{
		printf("Usage: %s 1|0\n",argv[0]);
	}
	if (argv[1][0]=='1')
	{
		printf("CONNECTOR LOCK ON\n");
		mc_gpio_set_value_HIGH(NULL,GPF5);
	}
	else
	{
		printf("CONNECTOR LOCK OFF\n");
		mc_gpio_set_value_LOW(NULL,GPF5);
	}
}


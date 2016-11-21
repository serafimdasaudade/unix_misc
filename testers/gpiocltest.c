#include "gpio.h"

int main(int argc,char **argv)
{
	int val;

	gpio_init_port(NULL,GPF5);
	gpio_set_direction_OUT(NULL,GPF5);

	if (argc!=2)
	{
		printf("Usage: %s 1|0\n",argv[0]);
	}
	if (argv[1][0]=='1')
	{
		printf("CONNECTOR LOCK ON\n");
		gpio_set_value_HIGH(NULL,GPF5);
	}
	else
	{
		printf("CONNECTOR LOCK OFF\n");
		gpio_set_value_LOW(NULL,GPF5);
	}
}


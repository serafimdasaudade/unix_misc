#include "gpio.h"

int main()
{
	int val;

	gpio_init_port(NULL,BUZZER);
	gpio_init_port(NULL,LED1);
	gpio_init_port(NULL,LED2);
	gpio_init_port(NULL,LED3);
	gpio_init_port(NULL,LED4);
	gpio_init_port(NULL,K1);
	gpio_set_direction_OUT(NULL,BUZZER);
	gpio_set_direction_OUT(NULL,LED1);
	gpio_set_direction_OUT(NULL,LED2);
	gpio_set_direction_OUT(NULL,LED3);
	gpio_set_direction_OUT(NULL,LED4);
	gpio_set_direction_IN(NULL,K1);

	gpio_set_value_HIGH(NULL,BUZZER);

	gpio_set_value_HIGH(NULL,LED1);
	sleep(1);
	gpio_set_value_HIGH(NULL,LED2);
	sleep(1);
	gpio_set_value_HIGH(NULL,LED3);
	sleep(1);
	gpio_set_value_HIGH(NULL,LED4);
	sleep(1);
	gpio_set_value_LOW(NULL,LED1);
	gpio_set_value_LOW(NULL,LED2);
	gpio_set_value_LOW(NULL,LED3);
	gpio_set_value_LOW(NULL,LED4);

	gpio_set_value_LOW(NULL,BUZZER);

	printf("Carrega no Botão K1\n"); fflush(stdout);
	while (1)
	{
		/*
		val=mc_gpio_get_value(NULL,K1);
		if (val==LOW)
		{
			printf("OK já carregaste\n"); fflush(stdout);
			exit(0);
		}*/
		int rc=gpio_wait_for_uevent(NULL,0);
		printf("OK já carregaste , rc=%d\n",rc); fflush(stdout);
		
	}
}


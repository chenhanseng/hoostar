#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
void delay_us(int us)
{
	int i;
	while(us--)
	{
		i =5;   
		while(i--);
	}
}
void delay_ms(int ms)
{
		while(ms--)
	{ 
		delay_us(1000);
	}
}



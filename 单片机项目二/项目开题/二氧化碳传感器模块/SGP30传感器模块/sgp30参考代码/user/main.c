#include "Systick.h"
#include "USART_drv.h"
#include "SGP30_drv.h"
#include "iic_drv.h"

int main(void)
{	
	SysTic_Config(72);
	USART_Myconfig(9600);
	while(1)
	{}
}

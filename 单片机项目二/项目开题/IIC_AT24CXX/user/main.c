#include "iic_drv.h"
#include "Systick.h"
#include "at24c04_drv.h"
#include "USART_drv.h"

int main(void)
{
	SysTic_Config(72);
	USART_Myconfig(9600);
	IIC_Config();

	WriteByte_AT24CXX(0x21,0x55);
	Delay_Ms(500);
	printf("ReadByte_AT24CXX : 0x%x\n",ReadByte_AT24CXX(0x21));
	
	while(1)
	{
		
	}
}

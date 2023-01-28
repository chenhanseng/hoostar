#include "Systick.h"



static uint16_t f_us = 0;//用来存放1us的次数
static uint16_t f_ms = 0;//用来存放1ms的次数

/************************************************
***函数名：SysTic_Config
***功   能：滴答定时器的配置（时钟源的选择）
***参   数:SYSCLK
***返回值：无
************************************************/
void SysTic_Config(uint16_t SYSCLK)
{
	//选择systic时钟
	//SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick->CTRL &= ~(0x01<<2);
	f_us =SYSCLK/8;
	f_ms =(uint16_t)f_us*1000;
}



/************************************************
***函数名：Delay_Us
***功   能：us延时的实现
***参   数:	nums填自己想计数的值
***返回值：无
************************************************/
void Delay_Us(uint16_t nums)
{
	uint32_t temp;
	//2.配置装载值
	SysTick->LOAD = f_us*nums;
	
	SysTick->VAL = 0x00;//优化，清空当前计数寄存器的值
	
	//3.使能systic
	SysTick->CTRL |= 0x01;
	
	//4.等待计时完成
	do
		{
			temp = SysTick->CTRL;
		}
	while ((temp & 0x01) && !(temp &(1<<16)));
		
	//5.关闭systic
	SysTick->CTRL &= 0x00;
	SysTick->VAL = 0x00;


}
/************************************************
***函数名：Delay_Ms
***功   能：ms延时的实现
***参   数:	nums填自己想计数的值
***返回值：无
************************************************/
void Delay_Ms(uint16_t nums)
{
	uint32_t temp;
	
	//2.配置装载值
	SysTick->LOAD = f_ms*nums;
		
	SysTick->VAL = 0x00;//优化，清空当前计数寄存器的值
		
	//3.使能systic
	SysTick->CTRL |= 0x01;
		
	//4.等待计时完成
	do
		{
			temp = SysTick->CTRL;
		}
	while ((temp & 0x01) && !(temp &(1<<16)));
			
	//5.关闭systic
	SysTick->CTRL &= 0x00;
	SysTick->VAL = 0x00;

}





































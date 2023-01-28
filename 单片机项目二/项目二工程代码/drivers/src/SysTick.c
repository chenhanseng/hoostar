#include "SysTick.h"

static uint32_t f_ms = 0;
static uint32_t f_us = 0;

/*
**@函数名：SysTick_InitConfig
**@函数功能：滴答定时器初始化配置
**@函数参数：AHBCLCK
			这个参数表示系统时钟频率 将系统时钟频率的八分频作为滴答定时器时钟频率
**@函数返回值：无
*/

void SysTick_InitConfig(uint8_t AHBCLCK)
{
	//1.操作 CTRL 寄存器 设置系统时钟频率为 8 MHZ
	SysTick->CTRL &= ~(1<<2);
	
	
	//实现 1ms 的延时和 1us 的延时
	f_us = AHBCLCK/8; 
	f_ms = f_us*1000;
	
}

/*
**@函数名：delay_ms
**@函数功能：实现 ms 级的延时
**@函数参数：nms
		这个参数是用来确定延时的具体时间 单位是 ms
		这个参数的最大值为1864 超过则会溢出
**@函数返回值：无
*/

void delay_ms(uint32_t nms)
{
	
	uint32_t temp;
	//1.向 LOAD 寄存器中装入需要计数的值
	SysTick->LOAD = f_ms*nms;//9000*nms nms 最大装填值为 16777215/9000
	
	//2.对 VAL 寄存器进行清空优化
	SysTick->VAL = 0x00;
	
	//3.打开嘀嗒定时器 开始计时
	SysTick->CTRL |= 0x01;
	
	//4.等待计数完成
	do
	{
		temp = SysTick->CTRL;//获取CTRL寄存器的值
		
	}while(!(temp &(0x1<<16)));//当 temp 第16位的值为1 即 CTRL 计时标志位为1时 计时完成退出循环
	
	//5.清空当前值寄存器 关闭嘀嗒计时器
	SysTick->VAL = 0x00;
	SysTick->CTRL &= 0x00;
}



/*
**@函数名：delay_us
**@函数功能：实现 us 级的延时
**@函数参数：nus
		这个参数是用来确定延时的具体时间 单位是 us
		这个参数的最大值为1864 超过则会溢出
**@函数返回值：无
*/

void delay_us(uint32_t nus)
{
	
	uint32_t temp;
	//1.向 LOAD 寄存器中装入需要计数的值
	SysTick->LOAD = f_us*nus;
	
	//2.对 VAL 寄存器进行清空优化
	SysTick->VAL = 0x00;
	
	//3.打开嘀嗒定时器 开始计时
	SysTick->CTRL |= 0x01;
	
	//4.等待计数完成
	do
	{
		temp = SysTick->CTRL;//获取CTRL寄存器的值
		
	}while(!(temp &(0x1<<16)));//当 temp 第16位的值为1 即 CTRL 计时标志位为1时 计时完成退出循环
	
	//5.清空当前值寄存器 关闭嘀嗒计时器
	SysTick->VAL = 0x00;
	SysTick->CTRL &= 0x00;
}



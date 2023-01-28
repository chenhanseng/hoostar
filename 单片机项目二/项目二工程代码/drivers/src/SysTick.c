#include "SysTick.h"

static uint32_t f_ms = 0;
static uint32_t f_us = 0;

/*
**@��������SysTick_InitConfig
**@�������ܣ��δ�ʱ����ʼ������
**@����������AHBCLCK
			���������ʾϵͳʱ��Ƶ�� ��ϵͳʱ��Ƶ�ʵİ˷�Ƶ��Ϊ�δ�ʱ��ʱ��Ƶ��
**@��������ֵ����
*/

void SysTick_InitConfig(uint8_t AHBCLCK)
{
	//1.���� CTRL �Ĵ��� ����ϵͳʱ��Ƶ��Ϊ 8 MHZ
	SysTick->CTRL &= ~(1<<2);
	
	
	//ʵ�� 1ms ����ʱ�� 1us ����ʱ
	f_us = AHBCLCK/8; 
	f_ms = f_us*1000;
	
}

/*
**@��������delay_ms
**@�������ܣ�ʵ�� ms ������ʱ
**@����������nms
		�������������ȷ����ʱ�ľ���ʱ�� ��λ�� ms
		������������ֵΪ1864 ����������
**@��������ֵ����
*/

void delay_ms(uint32_t nms)
{
	
	uint32_t temp;
	//1.�� LOAD �Ĵ�����װ����Ҫ������ֵ
	SysTick->LOAD = f_ms*nms;//9000*nms nms ���װ��ֵΪ 16777215/9000
	
	//2.�� VAL �Ĵ�����������Ż�
	SysTick->VAL = 0x00;
	
	//3.����શ�ʱ�� ��ʼ��ʱ
	SysTick->CTRL |= 0x01;
	
	//4.�ȴ��������
	do
	{
		temp = SysTick->CTRL;//��ȡCTRL�Ĵ�����ֵ
		
	}while(!(temp &(0x1<<16)));//�� temp ��16λ��ֵΪ1 �� CTRL ��ʱ��־λΪ1ʱ ��ʱ����˳�ѭ��
	
	//5.��յ�ǰֵ�Ĵ��� �ر���઼�ʱ��
	SysTick->VAL = 0x00;
	SysTick->CTRL &= 0x00;
}



/*
**@��������delay_us
**@�������ܣ�ʵ�� us ������ʱ
**@����������nus
		�������������ȷ����ʱ�ľ���ʱ�� ��λ�� us
		������������ֵΪ1864 ����������
**@��������ֵ����
*/

void delay_us(uint32_t nus)
{
	
	uint32_t temp;
	//1.�� LOAD �Ĵ�����װ����Ҫ������ֵ
	SysTick->LOAD = f_us*nus;
	
	//2.�� VAL �Ĵ�����������Ż�
	SysTick->VAL = 0x00;
	
	//3.����શ�ʱ�� ��ʼ��ʱ
	SysTick->CTRL |= 0x01;
	
	//4.�ȴ��������
	do
	{
		temp = SysTick->CTRL;//��ȡCTRL�Ĵ�����ֵ
		
	}while(!(temp &(0x1<<16)));//�� temp ��16λ��ֵΪ1 �� CTRL ��ʱ��־λΪ1ʱ ��ʱ����˳�ѭ��
	
	//5.��յ�ǰֵ�Ĵ��� �ر���઼�ʱ��
	SysTick->VAL = 0x00;
	SysTick->CTRL &= 0x00;
}



#include "UART.h"
#include "gizwits_protocol.h"


#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
    int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ 

void _sys_exit(int x) 
{ 
    x = x; 
} 

#endif

/**
* @ �� �� ����fputc
* @ �������ܣ�printf�ض��򣨽�printf���·������Ϊʹ�ô���1�����ͳ�ȥ��
* @ ����������1.ch     2.�ļ���
* @ �� �� ֵ����
*/
int fputc(int ch ,FILE *fp)
{
    USART_SendData(USART1,ch);
   
    while(SET != USART_GetFlagStatus(USART1,USART_FLAG_TXE));
    return 0;
}


/*
**@��������UART_InitConfig
**@�������ܣ����ô���1 ���д���ͨ��
**@������������
**@��������ֵ����
*/

void UART_InitConfig(void)
{
	
	
	//1.������ʱ�� ���� ���� GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	
	GPIO_InitTypeDef GPIO_InitStruct;//����һ���ṹ�� ����GPIO������Ϣ
	
	//�ṹ���Ա��ʼ��  PA9 ����Ϊ�����������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO��ʼ�� ���� PA9 �������ģʽ
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//�ṹ���Ա��ʼ��  PA10 ����Ϊ��������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	/*����ģʽ�²���Ҫ�������� ȡ�����ⲿ������*/
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO��ʼ������ ���� PA10 �������ģʽ
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//����һ�� USART_InitStruct �ṹ��
	USART_InitTypeDef USART_InitStruct;
	
	//�ṹ���Ա��ʼ��
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	
	//3.��ʼ������
	USART_Init(USART1,&USART_InitStruct);
	
	//4.ʹ�ܴ���
	USART_Cmd(USART1,ENABLE);
	
	//5.ʹ�ô��ڽ��ջ�������
	
	
}

/*
**@��������UART2_InitConfig
**@�������ܣ����ô���2 ���д���ͨ��
**@������������
**@��������ֵ����
*/

void UART2_InitConfig(void)
{
	
	
	//1.������ʱ�� ���� ���� GPIOA
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);//����
	
	GPIO_InitTypeDef GPIO_InitStruct;//����һ���ṹ�� ����GPIO������Ϣ
	
	//�ṹ���Ա��ʼ��  PA2 ����Ϊ�����������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO��ʼ�� ���� PA2 �������ģʽ
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//�ṹ���Ա��ʼ��  PA3 ����Ϊ��������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	/*����ģʽ�²���Ҫ�������� ȡ�����ⲿ������*/
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO��ʼ������ ���� PA3 �������ģʽ
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//����һ�� USART_InitStruct �ṹ��
	USART_InitTypeDef USART_InitStruct;
	
	//�ṹ���Ա��ʼ��
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	
	//3.��ʼ������
	USART_Init(USART2,&USART_InitStruct);
	
	//4.ʹ�ܴ���
	USART_Cmd(USART2,ENABLE);
	
	//5.ʹ�ô��ڽ��ջ�������
	
	
}




/*
//��ʼ��IO ����3
//��ESP8266ģ��ͨ�Ų����ʣ�9600
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	 
*/
void UART3_InitConfig(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //����3ʱ��ʹ��

 	USART_DeInit(USART3);  //��λ����3
		 //USART3_TX   PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PB10
   
    //USART3_RX	  PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ�� PB11
	
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART3, &USART_InitStructure); //��ʼ������3
  

	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
	
	//ʹ�ܽ����ж�
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�   
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}

//�жϴ�����

/*
**@��������USART3_IRQHandler
**@�������ܣ� ����3��ESP8266ͨ�� ���յ����ݽ��ж� ���� gizPutData()����ʵ�ִ������ݵĽ��ղ���д��Э������ݻ�����
**@������������
**@��������ֵ����
*/

void USART3_IRQHandler(void)
{
	uint8_t value = 0;
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)//���жϱ�־λ��λ��ִ�����²���
	{
		value = USART_ReceiveData(USART3);
		gizPutData(&value, 1);//����д�뻺����
	}

}



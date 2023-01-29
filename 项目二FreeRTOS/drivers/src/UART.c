#include "UART.h"
#include "gizwits_protocol.h"


#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
    int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式 

void _sys_exit(int x) 
{ 
    x = x; 
} 

#endif

/**
* @ 函 数 名：fputc
* @ 函数功能：printf重定向（将printf输出路径更改为使用串口1来发送出去）
* @ 函数参数：1.ch     2.文件流
* @ 返 回 值：有
*/
int fputc(int ch ,FILE *fp)
{
    USART_SendData(USART1,ch);
   
    while(SET != USART_GetFlagStatus(USART1,USART_FLAG_TXE));
    return 0;
}


/*
**@函数名：UART_InitConfig
**@函数功能：配置串口1 进行串口通信
**@函数参数：无
**@函数返回值：无
*/

void UART_InitConfig(void)
{
	
	
	//1.打开外设时钟 串口 复用 GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	
	GPIO_InitTypeDef GPIO_InitStruct;//定义一个结构体 保存GPIO配置信息
	
	//结构体成员初始化  PA9 配置为复用推挽输出
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO初始化 配置 PA9 输入输出模式
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//结构体成员初始化  PA10 配置为浮空输入
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	/*输入模式下不需要配置速率 取决于外部的速率*/
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO初始化配置 配置 PA10 输入输出模式
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//定义一个 USART_InitStruct 结构体
	USART_InitTypeDef USART_InitStruct;
	
	//结构体成员初始化
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	
	//3.初始化串口
	USART_Init(USART1,&USART_InitStruct);
	
	//4.使能串口
	USART_Cmd(USART1,ENABLE);
	
	//5.使用串口接收或发送数据
	
	
}

/*
**@函数名：UART2_InitConfig
**@函数功能：配置串口2 进行串口通信
**@函数参数：无
**@函数返回值：无
*/

void UART2_InitConfig(void)
{
	
	
	//1.打开外设时钟 串口 复用 GPIOA
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);//复用
	
	GPIO_InitTypeDef GPIO_InitStruct;//定义一个结构体 保存GPIO配置信息
	
	//结构体成员初始化  PA2 配置为复用推挽输出
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO初始化 配置 PA2 输入输出模式
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//结构体成员初始化  PA3 配置为浮空输入
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	/*输入模式下不需要配置速率 取决于外部的速率*/
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO初始化配置 配置 PA3 输入输出模式
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//定义一个 USART_InitStruct 结构体
	USART_InitTypeDef USART_InitStruct;
	
	//结构体成员初始化
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	
	//3.初始化串口
	USART_Init(USART2,&USART_InitStruct);
	
	//4.使能串口
	USART_Cmd(USART2,ENABLE);
	
	//5.使用串口接收或发送数据
	
	
}




/*
//初始化IO 串口3
//与ESP8266模块通信波特率：9600
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	 
*/
void UART3_InitConfig(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //串口3时钟使能

 	USART_DeInit(USART3);  //复位串口3
		 //USART3_TX   PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB10
   
    //USART3_RX	  PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化 PB11
	
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
  

	USART_Cmd(USART3, ENABLE);                    //使能串口 
	
	//使能接收中断
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断   
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}

//中断处理函数

/*
**@函数名：USART3_IRQHandler
**@函数功能： 串口3与ESP8266通信 接收到数据进中断 调用 gizPutData()函数实现串口数据的接收并且写入协议层数据缓冲区
**@函数参数：无
**@函数返回值：无
*/

void USART3_IRQHandler(void)
{
	uint8_t value = 0;
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)//若中断标志位置位则执行以下操作
	{
		value = USART_ReceiveData(USART3);
		gizPutData(&value, 1);//数据写入缓存区
	}

}



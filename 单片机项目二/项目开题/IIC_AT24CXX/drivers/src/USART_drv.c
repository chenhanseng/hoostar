#include "USART_drv.h"

/*******************************************************
****函数名:Led_ctrl
****功   能:LED灯的控制
****参   数:ledval
****返回值:无
*******************************************************/
void USART_Myconfig(uint32_t baudrate)
{	
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1,  ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(  GPIOA, &GPIO_InitStruct);

	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(	GPIOA, &GPIO_InitStruct);


	USART_InitStruct.USART_BaudRate = baudrate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(  USART1,  &USART_InitStruct);

	USART_Cmd( USART1,  ENABLE);
	
}

/************************************************
***函 数 名：USARTx_Putchar
***功    能：发送一个字符
***参    数: 1.USARTx  2.ch
***返 回 值：无
************************************************/

void USARTx_Putchar(USART_TypeDef * USARTx,uint8_t ch)
{
	USART_SendData( USARTx,  ch);
	while(RESET == USART_GetFlagStatus( USARTx,  USART_FLAG_RXNE));
}

/************************************************
***函 数 名：USARTx_Putstr
***功    能：发送一个字符串
***参    数: 1.USARTx  2.str
***返 回 值：无
************************************************/


void USARTx_Putstr(USART_TypeDef * USARTx,uint8_t *str)
{
	while(*str != '\0')
	{
		USARTx_Putchar(USARTx,  *str++);
	}
		
}


/************************************************
***函数名：fputc
***功   能：printf重定向
***参   数: 1.ch      2.文件流
***返回值：无
************************************************/
int fputc(int ch, FILE *fp)
{
	USART_SendData(USART1, ch);
	while(SET != USART_GetFlagStatus( USART1, USART_FLAG_TXE));
	return 0;
}




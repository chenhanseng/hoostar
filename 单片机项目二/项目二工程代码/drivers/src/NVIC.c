#include "NVIC.h"
#include "string.h"
#include "gizwits_product.h"

extern uint32_t RED_FLAG;
extern uint32_t BLUE_FLAG;
extern uint32_t GREEN_FLAG;

/*
**@函数名：NVIC_InitConfig_USART1
**@函数功能：USART1中断 优先级初始化配置 调用后打开USART1中断通道
**@函数参数：无
**@函数返回值：无
*/

void NVIC_InitConfig_USART(void)
{
	
	
	//2.针对每个中断 设置对应的抢占优先级和响应优先级
	NVIC_InitTypeDef NVIC_InitStruct;//定义一个与 NVIC 相关的结构体
	
	//结构体成员初始化
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;//中断通道选择为 USART1
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//响应优先级
	
	NVIC_Init(&NVIC_InitStruct);//进行 NVIC 初始化
	
	//3.配置串口中断源
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//接收数据不为空则进入中断

}


////中断处理函数

///*
//**@函数名：USART1_IRQHandler
//**@函数功能： 串口2与ESP8266通信 接收到数据进中断 调用 gizPutData()函数实
//**现串口数据的接收并且写入协议层数据缓冲区
//**@函数参数：无
//**@函数返回值：无
//*/

//void USART1_IRQHandler(void)
//{
//	uint8_t value = 0;
//	if(RESET != USART_GetITStatus(USART1,USART_IT_RXNE))//若中断标志位置位则执行以下操作
//	{
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//清除中断标志位
//		value = USART_ReceiveData(USART1);
//		gizPutData(&value, 1);//数据写入缓存区
//	}

//}




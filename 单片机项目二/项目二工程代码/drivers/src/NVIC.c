#include "NVIC.h"
#include "string.h"
#include "gizwits_product.h"

extern uint32_t RED_FLAG;
extern uint32_t BLUE_FLAG;
extern uint32_t GREEN_FLAG;

/*
**@��������NVIC_InitConfig_USART1
**@�������ܣ�USART1�ж� ���ȼ���ʼ������ ���ú��USART1�ж�ͨ��
**@������������
**@��������ֵ����
*/

void NVIC_InitConfig_USART(void)
{
	
	
	//2.���ÿ���ж� ���ö�Ӧ����ռ���ȼ�����Ӧ���ȼ�
	NVIC_InitTypeDef NVIC_InitStruct;//����һ���� NVIC ��صĽṹ��
	
	//�ṹ���Ա��ʼ��
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;//�ж�ͨ��ѡ��Ϊ USART1
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//��Ӧ���ȼ�
	
	NVIC_Init(&NVIC_InitStruct);//���� NVIC ��ʼ��
	
	//3.���ô����ж�Դ
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//�������ݲ�Ϊ��������ж�

}


////�жϴ�����

///*
//**@��������USART1_IRQHandler
//**@�������ܣ� ����2��ESP8266ͨ�� ���յ����ݽ��ж� ���� gizPutData()����ʵ
//**�ִ������ݵĽ��ղ���д��Э������ݻ�����
//**@������������
//**@��������ֵ����
//*/

//void USART1_IRQHandler(void)
//{
//	uint8_t value = 0;
//	if(RESET != USART_GetITStatus(USART1,USART_IT_RXNE))//���жϱ�־λ��λ��ִ�����²���
//	{
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//����жϱ�־λ
//		value = USART_ReceiveData(USART1);
//		gizPutData(&value, 1);//����д�뻺����
//	}

//}




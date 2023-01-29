#include "EXTI.h"
#include "UART.h"
#include "TIM.h"
#include "oled.h"

#define RUN 1
#define STOP 0
extern int work_mode;


/*
**@��������NVIC_InitConfig_EXTI
**@�������ܣ��ⲿ�ж�9 ���ú���
**@������������
**@��������ֵ����
*/
void NVIC_InitConfig_EXTI(void)
{
	//1.�� GPIOC �ⲿʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	//2.��ʼ�� PC9 ����
	GPIO_InitTypeDef GPIO_InitStruct;//����һ��GPIO�ṹ�� �����ʼ��״̬
	
	//GPIO ���ų�ʼ������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; //����ģʽ����Ϊ��������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//����ѡ��PC9
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//������������Ϊ50MHZ
	
	//���г�ʼ��
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//3.ѡ���ⲿ�ж�Դ ѡ��˿�C �� ����PC9
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource9);
	
	
	//4.����NVIC��ΪEXTI�����ж����ȼ���
	
	//���ÿ���ж� ���ö�Ӧ����ռ���ȼ�����Ӧ���ȼ�
	NVIC_InitTypeDef NVIC_InitStruct;//����һ���� NVIC ��صĽṹ��
	
	//�ṹ���Ա��ʼ��
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;//�ж�ͨ��ѡ��Ϊ �ⲿ�ж���5-9
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//��Ӧ���ȼ�
	
	NVIC_Init(&NVIC_InitStruct);//���� NVIC ��ʼ��
	
	
	//5.EXTI����
	EXTI_InitTypeDef EXTI_InitStruct;//����һ�����ⲿ�ж���صĽṹ��
	
	//��ʼ���ṹ��
	EXTI_InitStruct.EXTI_Line = EXTI_Line9;//ѡ���ⲿ�ж���9
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//ʹ��
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//������������Ϊ�ж�
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//����Ϊ�½��ش���
	
	//���г�ʼ��
	EXTI_Init(&EXTI_InitStruct);
	

	//6.�����жϴ�����
}

/*
**@��������NVIC_InitConfig_EXTI
**@�������ܣ��ⲿ�ж�8 ���ú���
**@������������
**@��������ֵ����
*/
void NVIC_InitConfig_EXTI8(void)
{
	//1.�� GPIOC �ⲿʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	//2.��ʼ�� PC9 ����
	GPIO_InitTypeDef GPIO_InitStruct;//����һ��GPIO�ṹ�� �����ʼ��״̬
	
	//GPIO ���ų�ʼ������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; //����ģʽ����Ϊ��������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;//����ѡ��PC8
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//������������Ϊ50MHZ
	
	//���г�ʼ��
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//3.ѡ���ⲿ�ж�Դ ѡ��˿�C �� ����PC8
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource8);
	
	
	//4.����NVIC��ΪEXTI�����ж����ȼ���
	
	//���ÿ���ж� ���ö�Ӧ����ռ���ȼ�����Ӧ���ȼ�
	NVIC_InitTypeDef NVIC_InitStruct;//����һ���� NVIC ��صĽṹ��
	
	//�ṹ���Ա��ʼ��
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;//�ж�ͨ��ѡ��Ϊ �ⲿ�ж���5-9
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//��Ӧ���ȼ�
	
	NVIC_Init(&NVIC_InitStruct);//���� NVIC ��ʼ��
	
	
	//5.EXTI����
	EXTI_InitTypeDef EXTI_InitStruct;//����һ�����ⲿ�ж���صĽṹ��
	
	//��ʼ���ṹ��
	EXTI_InitStruct.EXTI_Line = EXTI_Line8;//ѡ���ⲿ�ж���8
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//ʹ��
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//������������Ϊ�ж�
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//����Ϊ�½��ش���
	
	//���г�ʼ��
	EXTI_Init(&EXTI_InitStruct);
	

	//6.�����жϴ�����
}


//�жϴ�����
/*
**@��������EXTI9_5_IRQHandler
**@�������ܣ������ж��ϰ벿��  �жϰ���������������ʱ�� ��������ʱ��ת�붨ʱ���ж��� 
**@������������
**@��������ֵ����
*/
void EXTI9_5_IRQHandler(void)
{
	if(SET == EXTI_GetITStatus(EXTI_Line9))//�ⲿ�жϱ�־λ ��λ ��ʾ�ⲿ�жϴ���
	{
		if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)){
		TIM_Cmd(TIM7,ENABLE); //ʹ�ܶ�ʱ��7 ��������ʱ������°벿��
		}
		EXTI_ClearITPendingBit(EXTI_Line9);//����ⲿ�жϱ�־λ
	}
	
	else if(SET == EXTI_GetITStatus(EXTI_Line8))//�ⲿ�жϱ�־λ ��λ ��ʾ�ⲿ�жϴ���
	{
		if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)){
		TIM_Cmd(TIM7,ENABLE); //ʹ�ܶ�ʱ��7 ��������ʱ������°벿��
		OLED_Clear();//����	
		}
		EXTI_ClearITPendingBit(EXTI_Line8);//����ⲿ�жϱ�־λ
	}
}


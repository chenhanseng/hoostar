#include "TIM.h"
#include "UART.h"
#include <stdio.h>
#include "gizwits_product.h"
#include "gizwits_protocol.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#define TIM_ARR 255//TIM3��װ��ֵ
#define TIM_PSC 720//TIM3��Ƶϵ��
/* 40ms�Ķ�ʱ */
#define TIM7_ARR 400//TIM3��װ��ֵ
#define TIM7_PSC 7200//TIM3��Ƶϵ��	

#define RUN 1
#define STOP 0

/***�����ⲿ����***/
extern int work_mode;
extern int Flag_Task_close;
extern TaskHandle_t GETVALUE_Task_Handle;
extern TaskHandle_t OLED_Task_Handle;
extern TaskHandle_t GIZWITS_Task_Handle;

int TIM4_PSC = 7200;
int count = -1;

/*
**@��������TIM_InitConfig
**@�������ܣ���ʱ�� TIMx �жϵ����� ÿ��һ��ʱ�����һ���ж�
**@������������
**@��������ֵ����
*/

void TIM_InitConfig(void)
{
	//1.�� TIM6 ������ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	
	//2.��ʼ����ʱ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//������TIM��صĽṹ��
	
	//�ṹ���ʼ��
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200-1;//����PSC Ԥ��Ƶϵ�� ȷ������Ƶ��
	TIM_TimeBaseInitStruct.TIM_Period = 10-1;//����ARR �Զ���װ��ֵ ȷ�������Ĵ���
	//ȷ��������ʽ ������ʱ�����Բ����� Ĭ�����ϼ���
	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);//TIM��ʼ�� ��ʼ����ɺ�ͻ����һ�������¼���־λ
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);//��ʼ����ɺ� ����¼����±�־λ ȷ����ʱ��ɲŽ����ж�
	
	//3.ʹ�ܶ�ʱ��
	TIM_Cmd(TIM6,ENABLE);
	
	//4.��ʼ��NVIC
	NVIC_InitTypeDef NVIC_InitStruct;//����һ���� NVIC ��صĽṹ��
	
	//�ṹ���Ա��ʼ��
	NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;//�ж�ͨ��ѡ��Ϊ ��ʱ��6
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//��Ӧ���ȼ�
	
	NVIC_Init(&NVIC_InitStruct);//���� NVIC ��ʼ��
	
	
	//5.�����ж�Դ
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	
	//6.����жϺ���
	
}

//�жϺ���
/*
**@��������TIM6_IRQHandler
**@�������ܣ�ÿ1ms�����һ���ж� ʵ��Э���ϵͳʱ���ά��
**@������������
**@��������ֵ����
*/

void TIM6_IRQHandler(void)
{

		if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
		{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update );
		gizTimerMs();
		}

}


/*
**@��������TIM7_InitConfig
**@�������ܣ���ʱ�� TIMx �жϵ�����
**@������������
**@��������ֵ����
*/

void TIM7_InitConfig(void)
{
	//1.�� TIM7 ������ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	
	//2.��ʼ����ʱ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//������TIM��صĽṹ��
	
	//�ṹ���ʼ��
	TIM_TimeBaseInitStruct.TIM_Prescaler = TIM7_ARR-1;//����PSC Ԥ��Ƶϵ�� ȷ������Ƶ��
	TIM_TimeBaseInitStruct.TIM_Period = TIM7_PSC-1;//����ARR �Զ���װ��ֵ ȷ�������Ĵ���
	//ȷ��������ʽ ������ʱ�����Բ����� Ĭ�����ϼ���
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStruct);//TIM��ʼ�� ��ʼ����ɺ�ͻ����һ�������¼���־λ
	TIM_ClearFlag(TIM7,TIM_FLAG_Update);//��ʼ����ɺ� ����¼����±�־λ ȷ����ʱ��ɲŽ����ж�
	
	//3.ʹ�ܶ�ʱ��
	//TIM_Cmd(TIM7,ENABLE); //���밴���ж����� ʵ�ְ����жϿ��ƶ�ʱ
	
	//4.��ʼ��NVIC
	NVIC_InitTypeDef NVIC_InitStruct;//����һ���� NVIC ��صĽṹ��
	
	//�ṹ���Ա��ʼ��
	NVIC_InitStruct.NVIC_IRQChannel = TIM7_IRQn;//�ж�ͨ��ѡ��Ϊ ��ʱ��6
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;//��Ӧ���ȼ�
	
	NVIC_Init(&NVIC_InitStruct);//���� NVIC ��ʼ��
	
	
	//5.�����ж�Դ
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
	
	//6.����жϺ���
	
}

//�жϺ���
/*
**@��������TIM7_IRQHandler
**@�������ܣ������ж��°벿�� ��ʱ����ʱ���� ���붨ʱ���ж�
**@������������
**@��������ֵ����
*/

void TIM7_IRQHandler(void)
{

		if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM7, TIM_IT_Update );
			if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9))//���������������ǰ���״̬ ִ�а����ж�������Ҫִ�е�����
			{
				TIM_Cmd(TIM7,DISABLE);//ʧ�ܶ�ʱ�� �ȴ���һ�ΰ����жϵ��� ���رյĻ���ÿ��һ������ʱ�����һ��TIM7_IRQHandler
				printf("����ģʽ��\n");
				Flag_Task_close = -1;//����������־λ�ָ�Ĭ��״̬
				work_mode = RUN;
				if(pdFALSE == xTaskResumeFromISR( GETVALUE_Task_Handle)) // ���Ѳ�����ȡ����
				{
					printf("GETVALUE_Task Resume failed!\n");
				}
				
				if(pdFALSE == xTaskResumeFromISR( OLED_Task_Handle)) // ����OLED����
				{
					printf("OLED_Task Resume failed!\n");
				}
				
				if(pdFALSE == xTaskResumeFromISR( GIZWITS_Task_Handle)) // ����GIZWIT������������
				{
					printf("GIZWITS_Task Resume failed!\n");
				}
			}
			
			if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8))//���������������ǰ���״̬ ִ�а����ж�������Ҫִ�е�����
			{
				TIM_Cmd(TIM7,DISABLE);//ʧ�ܶ�ʱ�� �ȴ���һ�ΰ����жϵ��� ���رյĻ���ÿ��һ������ʱ�����һ��TIM7_IRQHandler
				printf("ֹͣģʽ��\n");
				work_mode = STOP;//��ǰϵͳ����ģʽ��Ϊֹͣ
				Flag_Task_close = 1;//����������־λ��λ �ڸ��Ե����������Ὣ�����������
			//	__WFI();//STM32����˯��ģʽ �ر��ں�ʱ��
			}
			TIM_Cmd(TIM7,DISABLE);
		}
		
}



/*
**@��������TIM4_InitConfig
**@�������ܣ�ʹ�ö�ʱ���ĶԲ���������п��� ����ÿ������������Ƶ��ת��
**@������������
**@��������ֵ����
*/
//void TIM4_InitConfig(void)
//{
//	//GPIO��ʼ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	GPIO_InitTypeDef GPIO_InitStruct;
//	
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
//	
//	
//	//1.�� TIM3 ������ʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
//	
//	//2.��ʼ����ʱ��
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//������TIM��صĽṹ��
//	
//	//�ṹ���ʼ��
//	TIM_TimeBaseInitStruct.TIM_Prescaler = TIM4_PSC-1;//����PSC Ԥ��Ƶϵ�� ȷ������Ƶ��
//	TIM_TimeBaseInitStruct.TIM_Period = 30-1;//����ARR �Զ���װ��ֵ ȷ�������Ĵ���
//	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//ȷ��������ʽ ���ϼ���
//	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//	
//	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);//TIM��ʼ�� ��ʼ����ɺ�ͻ����һ�������¼���־λ
//	TIM_ClearFlag(TIM4,TIM_FLAG_Update);//��ʼ����ɺ� ����¼����±�־λ ȷ����ʱ��ɲŽ����ж�
//	
//	//3.ʹ�ܶ�ʱ��
//	TIM_Cmd(TIM4,ENABLE);
//	
//	//4.��ʼ��NVIC
//	NVIC_InitTypeDef NVIC_InitStruct;//����һ���� NVIC ��صĽṹ��
//	
//	//�ṹ���Ա��ʼ��
//	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;//�ж�ͨ��ѡ��Ϊ ��ʱ��4
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//ʹ��
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//��Ӧ���ȼ�
//	
//	NVIC_Init(&NVIC_InitStruct);//���� NVIC ��ʼ��
//	
//	
//	//5.�����ж�Դ
//	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
//	
//	//6.����жϺ���
//	
//}

////�жϺ���
///*
//**@��������TIM4_IRQHandler
//**@�������ܣ���ʱ���������һ���ж� �����ͬ�����ź�
//**@������������
//**@��������ֵ����
//*/

//void TIM4_IRQHandler(void)
//{

//		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
//		{
//			count++;
//			if(count == 4){
//				count = 0;
//		}
//			if(count == 0){
//				GPIO_WriteBit(GPIOA,GPIO_Pin_9,Bit_SET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_10,Bit_SET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_RESET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_12,Bit_RESET);
//			}
//			
//			if(count == 1){
//				GPIO_WriteBit(GPIOA,GPIO_Pin_9,Bit_RESET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_10,Bit_SET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_SET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_12,Bit_RESET);
//		}
//			
//			if(count == 2){
//				GPIO_WriteBit(GPIOA,GPIO_Pin_9,Bit_RESET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_10,Bit_RESET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_SET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_12,Bit_SET);
//		}
//			
//			if(count == 3){
//				GPIO_WriteBit(GPIOA,GPIO_Pin_9,Bit_SET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_10,Bit_RESET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_RESET);
//				GPIO_WriteBit(GPIOA,GPIO_Pin_12,Bit_SET);
//		}
//			
//		TIM_ClearITPendingBit(TIM4, TIM_IT_Update );
//		}

//}



/*
**@��������PWM_OutPut_InitConfig
**@�������ܣ�PWMģʽ��� ��ʼ������ TIM3 ͨ��1
**@������������
**@��������ֵ����
*/

void PWM_OutPut_InitConfig(void)
{
	//1.ʹ�ܶ�ʱ��3�����IO��ʱ�� ���Ÿ���ΪTIM��ͨ��һ ����Ҫ�򿪸���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//�򿪶�ʱ�� TIM3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);//��GPIOCʱ�� �Լ�����ʱ��
	
	//2.��ʼ��IO��Ϊ���ù������
	GPIO_InitTypeDef GPIO_InitStruct;//����һ����GPIO��صĽṹ��
	
	//��ʼ���ṹ��
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//�������ģʽ����Ϊ �����������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;//|GPIO_Pin_7|GPIO_Pin_8;//����ѡ��PC6 PC7 PC8
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//��������
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);//���г�ʼ��
	
	//3.�� PC6 ������ʱ��PWM������� ����Ҫ������ӳ������
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);//������ӳ�� ����ѡ��Ϊȫ����ӳ��
	
	//4.��ʼ����ʱ��TIM3 ����ARR PSC��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructl;//����һ����TIM��ʼ���йصĽṹ��
	
	//��ʼ���ṹ��
	TIM_TimeBaseInitStructl.TIM_Prescaler = TIM_PSC;//��Ƶϵ��
	TIM_TimeBaseInitStructl.TIM_Period = TIM_ARR;//��װ��ֵ
	TIM_TimeBaseInitStructl.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInitStructl.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructl);//��ʼ��TIM3
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);//��ʼ����ɺ������¼����±�־λ ����¼����±�־λ
	
	
	//5.��ʼ������Ƚϲ���
	TIM_OCInitTypeDef TIM_OCInitStruct;//����һ����PWMͨ�� ��صĽṹ��
	
	//��ʼ���ṹ��
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//����PWMģʽ
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//���ʹ��
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;//�͵�ƽ��Ч 
	TIM_OCInitStruct.TIM_Pulse = 0;

	TIM_OC1Init(TIM3,&TIM_OCInitStruct);//��ʼ��ͨ��1
  //TIM_OC2Init(TIM3,&TIM_OCInitStruct);//��ʼ��ͨ��2
  //TIM_OC3Init(TIM3,&TIM_OCInitStruct);//��ʼ��ͨ��3
	
	//6.ʹ��Ԥװ�ؼĴ��� ARR��CCR
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��CCR1
  //TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��CCR2
  //TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);//ʹ��CCR3
	TIM_ARRPreloadConfig(TIM3,ENABLE);//ʹ��ARR
	
	//7.ʹ�ܶ�ʱ��
	TIM_Cmd(TIM3,ENABLE);
	
	//8.���ϸı�Ƚ�ֵ CCRx �ı�ռ�ձ� �����ͬ�ĵ�ƽ��С
}










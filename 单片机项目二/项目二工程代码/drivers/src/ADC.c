#include "ADC.h"
#include "SysTick.h"

/*
**@��������ADC1_InitConfig
**@�������ܣ�ADC1��ʼ������ ͨ��10->PC0 ͨ��11->PC1
**@������������
**@��������ֵ����
*/

void ADC1_InitConfig(void)
{
	//1.���� PC��ʱ�� �� ADC1ʱ�� ��λʱ�� ����PC0Ϊģ������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO,ENABLE);
	
	//��ʼ��PC0
	GPIO_InitTypeDef GPIO_InitStruct;//����һ��GPIO�ṹ��
	
	//��ʼ���ṹ��
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN; //����ģʽ����Ϊģ������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;//ѡ������ PC0 PC1
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//������������Ϊ50MHZ
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);//��ʼ��GPIO
	
	//2.��λADC1 ͬʱ����ADC1��Ƶ����
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//����ADC����Ƶ��Ϊ APB2��6��Ƶ
	
	//3.��ʼ��ADC1���� ����ADC1�Ĺ���ģʽ�Լ��������е������Ϣ
	ADC_InitTypeDef ADC_InitStruct;//����һ����ADC��ʼ���йصĽṹ��
	
	//��ʼ���ṹ��
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//����ģʽ����Ϊ��������ģʽ
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//����ɨ��ģʽ ��һ��ͨ���ɼ�һ����ɺ󰴹�����˳��ɼ���һ��ͨ��
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//ѡ������ת��
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//���ⲿ���� ѡ���������
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//���ݶ��뷽ʽΪ �Ҷ���
	ADC_InitStruct.ADC_NbrOfChannel = 2;//���ù������ͨ������
	
	ADC_Init(ADC1,&ADC_InitStruct);//��ʼ��ADC1
	
	//4.���ù���ͨ������
	//����ADC��1 ͨ��ת��˳���Լ�ת��ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_239Cycles5);//ͨ��10ת��˳��Ϊ1
	ADC_RegularChannelConfig(ADC1,ADC_Channel_11,2,ADC_SampleTime_239Cycles5);//ͨ��11ת��˳��Ϊ2

	//5.ʹ��ADC1��У׼  <*������ʹ����У׼*>
	ADC_Cmd(ADC1,ENABLE);//ʹ��ADC
	
	ADC_ResetCalibration(ADC1);//��λУ׼ADC1
	while(SET == ADC_GetResetCalibrationStatus(ADC1));//�ȴ���λУ׼���
	ADC_StartCalibration(ADC1);//У׼ADC1
	while(SET == ADC_GetCalibrationStatus(ADC1));//�ȴ�У׼���
	
	
	//6.�����������ת��
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	
	//7.�ȴ�ת����� ��ȡADCֵ
	

}

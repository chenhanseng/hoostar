#include "DMA.h"
#include "UART.h"
#include "SysTick.h" 
uint16_t buf[30][2];//����һ����ά���鱣��DMA������������ ÿһ�ж�Ӧһ��ADCͨ���ɼ������� ÿһ�д洢16λ������ ��30��
uint8_t SRC[10] = {'A','B','C','D','E','F','G','H','I','\n'};//�ڴ�Դ���� ĩλ���\n��֤���ݲ�������
uint8_t DST[10];//�ڴ�Ŀ������

/*
**@��������DMA_InitConfig_M2M
**@�������ܣ�DMA��ʼ������ ʹ��DMA�������� �ڴ浽�ڴ�
**@������������
**@��������ֵ����
*/

void DMA_InitConfig_M2M(void)
{

	//1.�� DMA ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//DAM������AHBʱ������
	
	//2.��ʼ������
	DMA_InitTypeDef DMA_InitStruct;//����һ����DMA��ʼ���йصĽṹ��
	
	//��ʼ���ṹ��
	DMA_InitStruct.DMA_BufferSize = 10;//buf����Ĵ�С ��10��Ԫ��
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//���� ���� ��ΪDMA�����е� Ŀ���ַ
	DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;//���˷���Ϊ�ڴ浽�ڴ�
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)SRC;//�����ڴ�Ļ���ַΪ SRC �����׵�ַ
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�����ڴ�����ݿ��Ϊһ�ֽ�
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ�ÿ������Ԫ�ض������ַ ���Ե�ַ��Ҫƫ��
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//DMAģʽ����Ϊ���δ���
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DST;//�����������ַΪ DST���� �ĵ�ַ
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//������������ݿ��Ϊһ�ֽ�
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;//����Ϊ���� ÿ��Ԫ�ض��е�ַ ��Ҫ��ַƫ��
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;//����DMA1 ͨ��4 �����ȼ�Ϊ��
 	
	DMA_Init(DMA1_Channel4,&DMA_InitStruct);//��ʼ��DMA���� DMA1 ͨ��4
	
	//3.ʹ��
	DMA_Cmd(DMA1_Channel4,ENABLE);//�ڴ浽�ڴ� ��ָ������ ����ѡȡ����ͨ��
	
	//4.������Ӧ�����DAM����
	//USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//��USART1 ��DMA����
	
}


/*
**@��������Get_DST_value
**@�������ܣ���ȡĿ�����������
**@������������
**@��������ֵ����
*/

void Get_DST_value(void)
{
 uint8_t i;
 for(i = 0; i < 10; i++){
  printf("%c\n",DST[i]);
  delay_ms(1000);
 }
}


/*
**@��������DMA_InitConfig_M2P
**@�������ܣ�DMA��ʼ������ ʹ��DMA�������� �ڴ浽����
**@������������
**@��������ֵ����
*/

void DMA_InitConfig_M2P(void)
{

	//1.�� DMA ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//DAM������AHBʱ������
	
	//2.��ʼ������
	DMA_InitTypeDef DMA_InitStruct;//����һ����DMA��ʼ���йصĽṹ��
	
	//��ʼ���ṹ��
	DMA_InitStruct.DMA_BufferSize = 10;//buf����Ĵ�С ��10��Ԫ��
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//����������ΪDMA�����е�Ŀ���ַ
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//���˷���Ϊ�ڴ浽����
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)SRC;//�����ڴ�Ļ���ַΪ SRC �����׵�ַ
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�����ڴ�����ݿ��Ϊ����
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ�ÿ������Ԫ�ض������ַ ���Ե�ַ��Ҫƫ��
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;//DMAģʽ����Ϊѭ������
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//�����������ַΪ USART1 �����ݼĴ�����ַ
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//������������ݿ��Ϊ����
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//����Ĵ�����ַֻ��һ�� ����Ҫ��ַƫ��
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;//����DMA1 ͨ��1 �����ȼ�Ϊ��
 	
	DMA_Init(DMA1_Channel4,&DMA_InitStruct);//��ʼ��DMA���� DMA1 ͨ��4
	
	//3.ʹ��
	DMA_Cmd(DMA1_Channel4,ENABLE);
	
	//4.������Ӧ�����DAM����
	//ѡ�񴮿ڵķ������ݼĴ��� �����ݰ��˵�TX�ڼ��Զ�ͨ�����ڷ��͵����ڵ������� ������֤
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//��USART1��DMA����
	
}


/*
**@��������DMA_InitConfig_P2M
**@�������ܣ�DMA��ʼ������ ʹ��DMA�������� ���赽�ڴ�
**@������������
**@��������ֵ����
*/

void DMA_InitConfig_P2M(void)
{

	//1.�� DMA ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//DAM������AHBʱ������
	
	//2.��ʼ������
	DMA_InitTypeDef DMA_InitStruct;//����һ����DMA��ʼ���йصĽṹ��
	
	//��ʼ���ṹ��
	DMA_InitStruct.DMA_BufferSize = 30*2;//buf����Ĵ�С ��30*2��Ԫ��
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;//����������ΪDMA�����е�Դ��ַ
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//���˷���Ϊ���赽�ڴ� ����Ҫ�����ڴ浽�ڴ�İ��˷���
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)buf;//�����ڴ�Ļ���ַΪ�����������buf�����׵�ַ
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�����ڴ�����ݿ��Ϊ����
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ�ÿ������Ԫ�ض������ַ ���Ե�ַ��Ҫƫ��
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;//DMAģʽ����Ϊѭ������
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//�����������ַΪ ADC1 �����ݼĴ�����ַ
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//������������ݿ��Ϊ����
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//����Ĵ�����ַֻ��һ�� ����Ҫ��ַƫ��
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;//����DMA1 ͨ��1 �����ȼ�Ϊ���
 	
	DMA_Init(DMA1_Channel1,&DMA_InitStruct);//��ʼ��DMA���� DMA1 ͨ��1
	
	//3.ʹ��
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	//4.������Ӧ�����DAM����
	ADC_DMACmd(ADC1,ENABLE);//��ADC1 ��DMA����
	
}


/*
**@��������GetValue_ADC_Average
**@�������ܣ���ȡADC�ɼ�����30�ε�ƽ��ֵ
**@����������channel��ADC��ͨ��˳�� ��buf�����еĵ�channel��
**@��������ֵ��ADC�ɼ�����30�����ݵ�ƽ��ֵ
*/

uint16_t GetValue_ADC_Average(uint32_t channel)
{
	uint32_t i = 0,sum = 0;
	for(i = 0;i < 30;i++){
	sum += buf[i][channel];
	}
	return (uint16_t)(sum/30);//����ADC�ɼ�����30�����ݵ�ƽ��ֵ
	
	
}



#include <stdio.h>
#include <stdbool.h>
#include "UART.h"
#include "SysTick.h"
#include "NVIC.h"
#include "EXTI.h"
#include "TIM.h"
#include "ADC.h"
#include "DMA.h"
#include "IIC.h"
#include "DHT11.h"
#include "oled.h"
#include "gizwits_product.h"

//ע��ģʽ���л�����ʹ�ð����ж� ����ͨ����ʱ������

/******�궨��******/
#define RUN 1
#define STOP 0

int ret;
int temp1;
int temp2;
int work_mode = RUN;//���岢��ʼ��ϵͳ����ģʽ RUN��������ģʽ STOP�������ģʽ 

	uint8_t wendu_value = 0;//�����¶ȱ���
	uint8_t shidu_value = 0;//����ʪ�ȱ���
	uint32_t smoke_value = 0;//��������Ũ��ֵ
	uint16_t CO2_value = 0;//���������̼Ũ�ȱ���
	uint16_t TVOC_value = 0;//����TVOCŨ�ȱ���
	float V_value = 0;//�����ѹ�ɼ�����
	
	dataPoint_t currentDataPoint;//����һ���������нṹ�� �����������ݵ������


/*
**@��������Gizwits_Init
**@�������ܣ��������豸��ʼ��
**@������������
**@��������ֵ����
*/
void Gizwits_Init(void)
{
	TIM_InitConfig();//TIM6 ʵ��1MSϵͳ��ʱ
	UART3_InitConfig(9600);//WIFIͨ�Ŵ��ڳ�ʼ��
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//�豸״̬�ṹ���ʼ�� ��սṹ��
	gizwitsInit();
}


int main(void)
{
	
	
/*****************************************��ʼ������*****************************************************/
	//1.�������ж����ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ����麯��
	NVIC_InitConfig_USART();//���ô����ж����ú��� 
	
	UART_InitConfig();//���ô��ڳ�ʼ������
	SysTick_InitConfig(72);//������શ�ʱ����ʼ������
	PWM_OutPut_InitConfig();//����PWM�����ʼ������
	ADC1_InitConfig();//����ADC1��ʼ������
	DMA_InitConfig_P2M();//��ADC1 ��DMA���� ��������

	DHT11_GPIO_Pin_InitCnofig();//DHT11���ų�ʼ�� ����PA8Ϊ����
	
	OLED_Init();//��ʼ��OLED  
	OLED_Clear();//����
	
	SGP30_InitConfig();//����SGP30 ��ʼ������
	

	Gizwits_Init();//���û����Ƴ�ʼ������
	gizwitsSetMode(WIFI_AIRLINK_MODE);
	printf("�����Ƴ�ʼ��\n");

	u8 chr[] = "CO2:";
	u8 str[] = "TVOC:";

/******************************************************************************************************/

	while(1){
		
		//�ṹ���Ա��ֵ
		currentDataPoint.valuewendu_value = wendu_value;//Add Sensor Data Collection
    currentDataPoint.valueshidu_value = shidu_value;//Add Sensor Data Collection
    currentDataPoint.valueV_value = V_value;//Add Sensor Data Collection
    currentDataPoint.valuesmoke_value = smoke_value;//Add Sensor Data Collection
    currentDataPoint.valueCO2_value = CO2_value;//Add Sensor Data Collection
    currentDataPoint.valueTVOC_value = TVOC_value;//Add Sensor Data Collection
		
		userHandle();//���ô˺������ṹ���Ա��ֵ �����ɼ������ź�������ṹ����
		gizwitsHandle((dataPoint_t *)&currentDataPoint);//Э�鴦�� ���ô˺����ϱ����ݵ�������
		
/**�жϵ�ǰϵͳ����ģʽ �����STOPģʽ ��⻽�Ѱ����Ƿ��� �������л�ģʽΪRUN**/
	if(work_mode == STOP){
			if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)){
				delay_ms(10);
			if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)){
			work_mode = RUN;
			printf("����ģʽ��");
			}
		}
		
	}

	
/**�жϵ�ǰϵͳ����ģʽ �����RUNģʽ��ִ�й��ܴ���**/
	if(work_mode == RUN){	

/***********************************�ź����ɼ�����**************************************/
		smoke_value = GetValue_ADC_Average(0);//DMA�����˵����ݴ��������� ���ô˺������ɼ�������Ũ��������ȡ����
		V_value = 2*GetValue_ADC_Average(1)*3.3/4095;//DMA�����˵����ݴ��������� ���ô˺������ɼ��ĵ�ѹ������ȡ����
		
		SGP30_Read(&CO2_value,&TVOC_value);//��ȡCO2��TVOC����
		DHT11_ReadData(&wendu_value,&shidu_value);//����DHT11���ݶ�ȡ���� ��ȡʪ�Ⱥ��¶�
		printf("�¶ȣ�%d",wendu_value);
		printf("ʪ�ȣ�%d",shidu_value);
		printf("CO2Ũ�ȣ�%d",CO2_value);
		printf("TVOCŨ�ȣ�%d",TVOC_value);
		printf("����%d",smoke_value);
		printf("��ѹ��%.2f",V_value);
		
		temp1 = V_value*10;
		temp2 = V_value*100;
		temp1 = temp1%10;//V_value�ĵ�һλС��
		temp2 = temp2%100;//V_value�ĵڶ�λС��

		
/**************************************OLED��ʾ����***********************************/
		char dian = '.';
		OLED_Clear();//����
		OLED_ShowCHinese(0,0,7);//��
		OLED_ShowCHinese(18,0,8);//��
		OLED_ShowCHinese(36,0,9);//��
		OLED_ShowNum(40,0,wendu_value,2,16);//��ʾ�¶ȵ�ֵ
		
		OLED_ShowCHinese(72,0,10);//ʪ
		OLED_ShowCHinese(90,0,11);//��
		OLED_ShowCHinese(108,0,12);//��
		OLED_ShowNum(112,0,shidu_value,2,16);//��ʾ�¶ȵ�ֵ 
		
		OLED_ShowCHinese(0,3,18);//��
		OLED_ShowCHinese(18,3,19);//ѹ
		OLED_ShowCHinese(36,3,20);//��
		OLED_ShowNum(40,3,V_value,1,16);//��ʾ��ѹ������
		OLED_ShowChar(52,3,dian);//��ʾ'.'
		OLED_ShowNum(64,3,temp1,1,16);//��ʾ��ѹ�ĵ�һλС��
		OLED_ShowNum(76,3,temp2,1,16);//��ʾ��ѹ�ĵڶ�λС��

		OLED_ShowCHinese(0,6,13);//��
		OLED_ShowCHinese(18,6,14);//��
		OLED_ShowCHinese(36,6,15);//Ũ
		OLED_ShowCHinese(54,6,16);//��
		OLED_ShowCHinese(72,6,17);//��
		OLED_ShowNum(80,6,smoke_value,4,16);//��ʾ����Ũ�ȵ�����
		
		delay_ms(1000);
		OLED_Clear();//����
		OLED_ShowString(0,0,chr);//��ʾ�ַ���CO2:
		OLED_ShowNum(30,0,CO2_value,4,16);//��ʾCO2Ũ�ȵ�ֵ
		
		OLED_ShowString(0,3,str);//��ʾ�ַ���TVOC:
		OLED_ShowNum(40,3,TVOC_value,4,16);//��ʾTVOCŨ�ȵ�ֵ
		
		
/*******************�����������Ƿ��� �������л�ģʽΪSTOP �������״̬*****************/
		if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)){ 
			delay_ms(10);
		if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)){ 
			work_mode = STOP;
			printf("����ģʽ��");
			OLED_Clear();//����	
			}
		}
			
	}
}

}




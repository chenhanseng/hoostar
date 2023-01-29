#include "DHT11.h" 
#include "delay.h"

#define DHT11_High() GPIO_SetBits(GPIOA,GPIO_Pin_8)//����������
#define DHT11_Low() GPIO_ResetBits(GPIOA,GPIO_Pin_8)//����������
#define DHT11_Read() GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)//��ȡ���ߵ�ƽ

/*****����ʹ�õ���ʱ�������Ǿ�׼��ʱ ������ʱ�����Ĳ�����ʵ��ֵ��һ������******/

/*
**@������:DHT11_GPIO_Pin_InitCnofig
**@��������:DHT11�������ų�ʼ������
**@��������:��
**@��������ֵ:��
*/
void DHT11_GPIO_Pin_InitCnofig(void)
{
	//1.��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;//����һ���ṹ�� ����GPIO������Ϣ
	
	//�ṹ���Ա��ʼ��
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO��ʼ�� ���� PA8 �������ģʽ
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_8);//ʹ���߿��е�ƽΪ�ߵ�ƽ
	

}


/*
**@������:DHT11_Output
**@��������:��������Ϊ���ģʽ ���������������MCU��˵��
**@��������:��
**@��������ֵ:��
*/

void DHT11_Output(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;//����һ���ṹ�� ����GPIO������Ϣ

	//�ṹ���Ա��ʼ��  PA8����Ϊ�������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);
}

/*
**@������:DHT11_Input
**@��������:��������Ϊ����ģʽ ����������������MCU��˵��
**@��������:��
**@��������ֵ:��
*/

void DHT11_Input(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;//����һ���ṹ�� ����GPIO������Ϣ

	//�ṹ���Ա��ʼ��  PB7����Ϊ��©���
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);
}


/*
**@������:DHT11_SendReset
**@��������:MCU������ʼ�ź�
**@��������:��
**@��������ֵ:��
*/
void DHT11_SendReset(void)	   
{                 
	DHT11_Output(); 	//����IOΪ���
	DHT11_Low(); 			//��������
	delay_ms(40);    	//�͵�ƽά������18ms
	DHT11_High(); 			//��������
	delay_us(40);     //�ߵ�ƽά��20~40us
}


/*
**@������:DHT11_CheckResponse
**@��������:MCU�����Ӧ�ź�
**@��������:��
**@��������ֵ:����0��ʾ���� ����1��ʾ���յ���Ӧ�ź�
*/
uint8_t DHT11_CheckResponse(void) 	   
{   
	uint8_t timecount=0;//����һ������ʱ��
	DHT11_Input();		//����IOΪ����	 
    
	//������Ӧ�����DHT11����������40~50us

	//����ȡ���ߵ�ƽ ��ʾ��û�м�⵽��Ӧ ��ʱ1us ����ʱ��100usʱ���ʾ����
	while (DHT11_Read() && timecount<100)
	{
		timecount++;
		delay_us(1);
	}
	//����while	��ʱδ��⵽DHT11Ӧ��	�򷵻�0
	//δ��ʱ�������յ��͵�ƽ�ź�	�����ж�
	if(timecount>=100)
		return 0;
	else 
		timecount=0;
    //DHT11�������ߺ���ٴ���������40~50us
	while (!DHT11_Read() && timecount<100)
	{
		timecount++;
		delay_us(1);
	}
	if(timecount>=100)
		return 0;	    
	//��ȷ��⵽�˸ߵ�ƽ��δ��ʱ	����1
	return 1;
}


/*
**@������:DHT11_ReadBit
**@��������:��ȡһ��bit������
**@��������:��
**@��������ֵ:����0��ʾ�͵�ƽ ����1��ʾ�ߵ�ƽ
*/
uint8_t DHT11_ReadBit(void) 			 
{
 	uint8_t timecount=0;
	//0��1���źŶ����ȵ͵�ƽ�ٸߵ�ƽ
	//�������ڸߵ�ƽ��ά��ʱ�䲻ͬ
	while(DHT11_Read() && timecount<100)
	{
		timecount++;
		delay_us(1);
	}
	timecount=0;
	while(!DHT11_Read() && timecount<100)//��ȡ���ߵ�ƽ���˳�whileѭ�� ��ʾ���ݵ��� ���ݸߵ�ƽ����ʱ���ж���0����1
	{
		timecount++;
		delay_us(1);
	}
	//����DATASHEET������0��1��ʽ	�ȴ�40us���ж�����
	delay_us(60);//��ʵ�˴���40us ��������ʱ��������׼ ������ֵ���в���
	//��Ϊ�ߵ�ƽ	�򷵻���1
	if(DHT11_Read())
		return 1;
	else 
		return 0;		   
}


/*
**@������:DHT11_ReadByte
**@��������:��ȡһ��Byte������
**@��������:��
**@��������ֵ:���ض�ȡ����һ���ֽ�����
*/
uint8_t DHT11_ReadByte(void)    
{        
	uint8_t i,dat = 0;
	for(i=0;i<8;i++) 
	{
		dat<<=1; 
		dat|=DHT11_ReadBit();
	}						    
	return dat;
}


/*
**@������:DHT11_ReadData
**@��������:��ȡDHT11���ص���������
**@��������:*wendu:�����¶����ݵı��� �ĵ�ַ   *shidu:����ʪ�����ݵı��� �ĵ�ַ
**@��������ֵ:0��ʾ��ȡ���� 1��ʾ��ȡ��ȷ
*/
uint8_t DHT11_ReadData(uint8_t *wendu,uint8_t *shidu)    
{   
 	uint8_t buf[5];//����һ���������DHT11���ص����� 40bit ������ ÿһ��Ԫ��һ�ֽ� 
	uint8_t i;
	DHT11_SendReset();//MCU������ʼ�ź�
	if(DHT11_CheckResponse() == 1)//MCU��⵽��Ӧ�ź� ��ʼ������
	{
		for(i=0;i<5;i++)	//������ȡDHT11���ص�40λ����
		{
			buf[i]=DHT11_ReadByte();
		}
		//����DATASHEET�ṩ��У���㷨
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			//С��������ʵ��Ϊ0	��˲�ȡ
			*shidu=buf[0];//����ʪ�����ݵ���������
			*wendu=buf[2];//�����¶����ݵ���������
		}
	}
	else 
		return 0;
	
	return 1;	    
}



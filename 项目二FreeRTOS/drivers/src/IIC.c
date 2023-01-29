#include "IIC.h"
#include "delay.h"
#include "UART.h"
/*****����ʹ�õ���ʱ�������Ǿ�׼��ʱ ������ʱ�����Ĳ�����ʵ��ֵ��һ������******/

/*ͨ�����ģ��ķ�ʽʵ��EEPROMͨ�� ����Ϊ��Ƭ�� �ӻ�ΪEEPROM*/

#define SCL_High() GPIO_SetBits(GPIOB,GPIO_Pin_6)//��SCL������
#define SCL_Low() GPIO_ResetBits(GPIOB,GPIO_Pin_6)//��SCL������
#define SDA_High() GPIO_SetBits(GPIOB,GPIO_Pin_7)//��SDA������
#define SDA_Low() GPIO_ResetBits(GPIOB,GPIO_Pin_7)//��SDA������
#define SDA_Read() GPIO_ReadInputDataBit(GPIOB,  GPIO_Pin_7)//��ȡ�����ߵ�ƽ

/*
**@������:IIC_GPIO_Pin_InitCnofig
**@��������:EEPROM ���ų�ʼ������
**@��������:��
**@��������ֵ:��
*/

void IIC_GPIO_Pin_InitCnofig(void)
{
	//1.��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;//����һ���ṹ�� ����GPIO������Ϣ
	
	//�ṹ���Ա��ʼ��  PB6 PB7����Ϊ��©���(��ֹ������ʱ ����֮����ֶ�·)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO��ʼ�� �����������ģʽ
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);//ʹ���߿��е�ƽΪ�ߵ�ƽ
	

}


/*
**@������:IIC_SDA_Output
**@��������:�������������ʱ ��������Ϊ��©���ģʽ �������������������������˵��
**@��������:��
**@��������ֵ:��
*/

void IIC_SDA_Output(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;//����һ���ṹ�� ����GPIO������Ϣ

	//�ṹ���Ա��ʼ��  PB7����Ϊ��©���
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB,&GPIO_InitStruct);
}



/*
**@������:IIC_SDA_Input
**@��������:��������������ʱ ��������Ϊ��������ģʽ �������������������������˵��
**@��������:��
**@��������ֵ:��
*/

void IIC_SDA_Input(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;//����һ���ṹ�� ����GPIO������Ϣ

	//�ṹ���Ա��ʼ��  PB7����Ϊ��������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB,&GPIO_InitStruct);
}



/*
**@������:IIC_Start
**@��������:ģ��IIC����ʼ�ź� ��ʱ����Ϊ�ߵ�ƽʱ �����߲���һ���½���
**@��������:��
**@��������ֵ:��
*/

void IIC_Start(void)
{

	IIC_SDA_Output();//������������Ϊ���ģʽ
	
	SDA_High();//������������
	SCL_High();//��ʱ��������
	delay_us(8);//��ʱ5us

	SDA_Low();//������������ ��SCLΪ�ߵ�ƽʱ�����߲���һ���½���
	delay_us(8);//��ʱ5us
	SCL_Low();//��ʱ�������� ��ֹ�󴥲���ֹͣ�ź�
}



/*
**@������:IIC_Stop
**@��������:ģ��IIC��ֹͣ�ź� ��ʱ����Ϊ�ߵ�ƽʱ �����߲���һ��������
**@��������:��
**@��������ֵ:��
*/

void IIC_Stop(void)
{
//	SCL_Low();//���ͺ�����������ģʽ ��ֹ����
	IIC_SDA_Output();//������������Ϊ���ģʽ
	SCL_Low();
	SDA_Low();//������������
	delay_us(8);
	SCL_High();//��ʱ��������
	SDA_High();
	delay_us(8);//��ʱ5us
	//ֹͣ�źŲ����� ���߿��� ����ʱ����ӦΪ�ߵ�ƽ ���Բ���Ҫ��ʱ��������
}



/*
**@������:IIC_Send_ACK
**@��������:ģ��IIC ���Ϳ϶�Ӧ���ź� ��ʱ����Ϊ�ߵ�ƽʱ ������Ϊ�͵�ƽ
**@��������:��
**@��������ֵ:��
*/

void IIC_Send_ACK(void)
{
	SCL_Low();//���ͺ�����������ģʽ ��ֹ����
	IIC_SDA_Output();//������������Ϊ���ģʽ
	SDA_Low();//������������
	delay_us(8);//��ʱ5us
	SCL_High();//��ʱ�������� ʹʱ����Ϊ��ʱ ������Ϊ��
	delay_us(8);//���ָߵ�ƽ״̬һ��ʱ��
	SCL_Low();//��ʱ�������� Ӧ���źŽ���
}



/*
**@������:IIC_Send_NACK
**@��������:ģ��IIC ���ͷ�Ӧ���ź� ��ʱ����Ϊ�ߵ�ƽʱ ������Ϊ�ߵ�ƽ
**@��������:��
**@��������ֵ:��
*/

void IIC_Send_NACK(void)
{
	SCL_Low();//���ͺ�����������ģʽ ��ֹ����
	IIC_SDA_Output();//������������Ϊ���ģʽ
	
	SDA_High();//������������
	delay_us(8);//��ʱ5us 
	SCL_High();//��ʱ�������� ʹʱ����Ϊ��ʱ ������Ϊ��
	delay_us(8);//���ָߵ�ƽ״̬һ��ʱ��
	SCL_Low();//��ʱ�������� Ӧ���źŽ���
//	SDA_High();//�ͷ�������
}



/*
**@������:IIC_Wait_ACK
**@��������:ģ��IIC �ȴ��϶�Ӧ���ź�
**@��������:��
**@��������ֵ:����Ӧ���ź� ����ֵ����Ϊ0���϶�Ӧ�𣩻�1����Ӧ��)
*/

uint8_t IIC_Wait_ACK(void)
{
	IIC_SDA_Input();//������������Ϊ����ģʽ ��Ϊ������Ҫ��ȡ�ӻ����͹�����Ӧ���ź�
					//Ӧ���ź�ֻ��һλ ��Ϊ0
	SDA_High();
	delay_us(10);
	SCL_High();//SCLΪ�ߵ�ƽ�ڼ� �����ȶ� ���Զ�ȡ����
	delay_us(10);
	uint8_t errtime = 0;//����һ������ʱ�� ������ʱ�����255 ���ʾ��������
	while(SDA_Read())
		{
			errtime++;
			if(errtime > 255)
				{
					IIC_Stop();
					return 1;
				}
			
		}
	SCL_Low();//ǯ������ ��ֹ����ֹͣ�ź�
	return 0;
}



/*
**@������:IIC_Send_Byte
**@��������:ģ��IIC ����һ�ֽ�����
**@��������:��
**@��������ֵ:��
*/

void IIC_Send_Byte(uint8_t data)
{
	
	IIC_SDA_Output();//����������Ϊ���ģʽ ��ΪSDA��ʱ��Ҫ��������
	uint8_t i = 0;
	SCL_Low();//ʱ�������� ���ݿ��Ըı�
	for(i = 0;i < 8;i++)//ѭ������8λ����
		{
			if(data & 0x80)//�ж�data���λ�� 0����1 ��1��SDA���� ��0��SDA����
				{
					SDA_High();
				}
			else
				{
					SDA_Low();
				}
			data = data<<1;//ÿ������һλ���� ��data����һλ ȷ��ÿ����Ҫ���͵�λ�������λ

		//ÿ����һλ���� ����һ��ʱ������ ���´�������ģ���1��ʱ������
		delay_us(8);
		SCL_High();
		delay_us(8);
		SCL_Low();//ÿ������һλ���� ��ʱ�������� ��֤������һλ����ʱʱ����Ϊ�� ���ݿ��Ըı�
		}
}




/*
**@������:IIC_Receive_Byte
**@��������:ģ��IIC ����һ�ֽ�����
**@��������:��
**@��������ֵ:��
*/

uint8_t IIC_Receive_Byte(uint8_t ack)
{	

	IIC_SDA_Input();//����������Ϊ����ģʽ ��ʱ��Ҫ���մӻ�������������
	uint8_t i = 0;
	uint8_t ReceiveData = 0;
	for(i = 0;i < 8;i++)//ѭ������8λ����
		{
			SCL_Low();
			delay_us(8);
			SCL_High();
			ReceiveData <<= 1;
			if(SDA_Read())//�ж�data���λ�� 0����1 ��1��SDA���� ��0��SDA����
				{
					ReceiveData |=0x01; 
				}
		//ÿ����һλ���� ����һ��ʱ������ ���´�������ģ���1��ʱ������
		delay_us(8);
		}
	if(ack)
		{
			IIC_Send_NACK();//�������ackΪ1 ���ͷ�Ӧ�� ���ٽ�������
		}
	else
		IIC_Send_ACK();//�������ackΪ0 ���Ϳ϶�Ӧ�� ������������
	return ReceiveData;
}


/*
**@������:EEPROM_Write_Byte
**@��������:ʹ�� EEPROM ��IICͨ��Э�鷢������
**@��������:addr
************@��������������ݴ����д洢�ռ�ĵ�ַ �����ݵĴ�ŵ�ַ
************@data
************@�������������Ҫ���͵�����
**@��������ֵ:���ش����� 0xee������� 0xff��������
*/

uint8_t EEPROM_Write_Byte(uint8_t addr,uint8_t data)
{
	IIC_Start();
	IIC_Send_Byte(0xA0);//�����豸��ַ��һλ��λ

	//����Ӧ���ź�
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//ֹͣͨ��
			printf("device addr err!");
			return 0xee;//���ش�����
		}
	
	IIC_Send_Byte(addr);//���ʹ洢�ռ��ַ
	//����Ӧ���ź�
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//ֹͣͨ��
			printf("word addr err!");
			return 0xee;//���ش�����
		}
	
	IIC_Send_Byte(data);//��������
	//����Ӧ���ź�
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//ֹͣͨ��
			printf("Send date stop");
			return 0xee;//���ش�����
		}
	IIC_Stop();
	return 0xff;//������ȷ��
}



/*
**@������:EEPROM_Read_Byte
**@��������:ʹ�� EEPROM ��IICͨ��Э���ȡ����
**@��������:addr
************@��������������ݴ����д洢�ռ�ĵ�ַ �����ݵĴ�ŵ�ַ
**@��������ֵ:���ؽ��յ�������
*/

uint8_t EEPROM_Read_Byte(uint8_t addr)
{
	uint8_t receivedata = 0;
	IIC_Start();
	IIC_Send_Byte(0xA0);//�����豸��ַ��һλдλ

	//����Ӧ���ź�
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//ֹͣͨ��
			printf("device addr err!");
			return 0xee;//���ش�����
		}
	
	IIC_Send_Byte(addr);//���ʹ洢�ռ��ַ
	//����Ӧ���ź�
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//ֹͣͨ��
			printf("word addr err!");
			return 0xee;//���ش�����
		}
	
	IIC_Start();
	
	IIC_Send_Byte(0xA1);//�����豸��ַ��һλ��λ
	//����Ӧ���ź�
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//ֹͣͨ��
			printf("Send date stop");
			return 0xee;//���ش�����
		}
	
	receivedata = IIC_Receive_Byte(1);//��ȡ���� ��ȡ��ɲ��ٽ�������
	IIC_Stop();//ֹͣͨ��
	return receivedata;
}



/*
**@������:SGP30_InitConfig
**@��������:SGP30��ʼ��
**@��������:��
**@��������ֵ:��
*/
void SGP30_InitConfig(void)
{
	IIC_GPIO_Pin_InitCnofig();//GPIO��ʼ��
	IIC_Start();//������ʼ�ź�
	IIC_Send_Byte(0x58<<1);//������λ�豸��ַ�� ��һλдλ��0��
	IIC_Wait_ACK();//�ȴ�Ӧ���ź�
	
	IIC_Send_Byte(0x20);//�������� ��ʼ���豸SGP30��0x2003�� �ĸ߰�λ
	IIC_Wait_ACK();//�ȴ�Ӧ���ź�
	
	IIC_Send_Byte(0x03);//�������� ��ʼ���豸SGP30��0x2003�� �ĵͰ�λ
	IIC_Wait_ACK();//�ȴ�Ӧ���ź�
	
	IIC_Stop();//����ֹͣ�ź�
	delay_ms(1100);//�ȴ��豸��ʼ�����
	
}


/*
**@������:SGP30_Write
**@��������:SGP30д���� ����0x2008 ��ʾSGP30��ʼ���� ����������
**@��������:
					a�������λ/0x20
					b�������λ/0x08
**@��������ֵ:��
*/
void SGP30_Write(uint8_t a,uint8_t b)
{
	IIC_Start();//������ʼ�ź�
	IIC_Send_Byte(0xb0);//�����豸��ַ��дλ 0xb0
	IIC_Wait_ACK();//�ȴ�Ӧ���ź�
	IIC_Send_Byte(a);//�������ݸ߰�λ
	IIC_Wait_ACK();//�ȴ�Ӧ���ź�
	
	IIC_Send_Byte(b);//�������ݵͰ�λ
	IIC_Wait_ACK();//�ȴ�Ӧ���ź�
	
	IIC_Stop();
	delay_ms(1100);//�ȴ�SGP30��ʼ����
	
}


/*
**@������:SGP30_Read
**@��������:SGP30������ 
**@��������:��
**@��������ֵ:����CO2Ũ�Ⱥ�TVOCŨ��
*/
void SGP30_Read(uint16_t * CO2,uint16_t* TVOC)
{
	uint8_t CRC_Check = 0;
	SGP30_Write(0x20,0x08);//��������0x2008 ʹSGP30��ʼ����
	
	IIC_Start();//������ʼ�ź�
	IIC_Send_Byte(0xb1);//�����豸��ַ�Ӷ�λ
	IIC_Wait_ACK();//�ȴ�Ӧ��
	
	*CO2	|= (u16)(IIC_Receive_Byte(0)<<8);//8~15λ �ҽ��������ݷ���һ���϶�Ӧ��
 	*CO2 |= (u16)(IIC_Receive_Byte(0));//0~7λ
	CRC_Check = IIC_Receive_Byte(0);//����16λ���� ����һ��У����
	*TVOC |= (u16)IIC_Receive_Byte(0)<<8;//8~15λ �ҽ��������ݷ���һ���϶�Ӧ��
 	*TVOC |= (u16)(IIC_Receive_Byte(0));//0~7λ �ҽ��������ݷ���һ���϶�Ӧ��
	CRC_Check = IIC_Receive_Byte(1);//����16λ���� ����һ��У���� ����ȫ��������� ����һ����Ӧ��
	IIC_Stop();//����ֹͣ�ź� ͨ�Ž���
	return;
	
}













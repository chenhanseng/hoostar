#include "iic_drv.h"
#include "Systick.h"


 
/*
**引脚初始化配置
*/
void IIC_Config(void)
{
	/*PB6 --> SCL*/
	/*PB7 --> SDA*/
	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_IIC_PORT, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//配置开漏输出
	GPIO_InitStruct.GPIO_Pin = IIC_SCK_PIN | IIC_SDA_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_PORT, &GPIO_InitStruct);
	GPIO_SetBits(IIC_PORT, IIC_SCK_PIN | IIC_SDA_PIN);
}

/*
**配置sda工作状态为输入模式
*/
void sdaIn(void)
{	
	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_IIC_PORT, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//配置开漏输出
	GPIO_InitStruct.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Init(IIC_PORT, &GPIO_InitStruct);
}

/*
**配置sda工作状态为输出模式
*/
void sdaOut(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_IIC_PORT, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//配置开漏输出
	GPIO_InitStruct.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_PORT, &GPIO_InitStruct);
}

/*
**开始信号
*/
void IIC_Start(void)
{
	SCL_LOW();
	sdaOut();	
	
	SDA_HIGH();
	SCL_HIGH();
	Delay_Us(2);
	SDA_LOW();
	SCL_LOW();	//钳住总线
}

/*
**停止信号
*/
void IIC_Stop(void)
{
	SCL_LOW();
	sdaOut();

	SDA_LOW();
	SCL_HIGH();
	Delay_Us(2);
	SDA_HIGH();
}

/*
**发送应答信号(接收方)
*/
void IIC_SendAck(void)
{
	SCL_LOW();
	sdaOut();

	SDA_LOW();
	Delay_Us(2);
	SCL_HIGH();
	Delay_Us(2);
	SCL_LOW();
}

/*
**发送非应答信号（接收方）
*/
void IIC_SendNoack(void)
{
	SCL_LOW();
	sdaOut();

	SDA_HIGH();
	SCL_HIGH();
	Delay_Us(2);
	SCL_LOW();
}


/*
**等待应答/非应答信号（发送方）
*/
en_ack_flag IIC_WaitforACK(void)
{
	uint8_t cnt = 0;	//读取次数

	SCL_LOW();
	sdaIn();

	SCL_HIGH();
	Delay_Us(2);
	while(SDA_READ())
	{
		cnt++;
		if(cnt > 50)
		{
			IIC_Stop();
			return IIC_NOACK;
		}
	}

	SCL_LOW();
	return IIC_ACK;
}

/*
**发送一个字节数据
*/
void IIC_SendByte(uint8_t data)
{
	uint8_t i;

	SCL_LOW();
	sdaOut();

	for(i = 0;i < 8;i++)
	{
		if(data & 0x80)		//高位先行
		{
			SDA_HIGH();
		}
		else
		{
			SDA_LOW();
		}
		data = data<<1;
		SCL_HIGH();
		Delay_Us(2);
		SCL_LOW();		//拉低的目的是为了使得发送方有机会可以切换电平状态
		Delay_Us(2);
	}
}

/*
**接收一个字节数据
*/
uint8_t IIC_RcvByte(en_ack_flag ack)
{
	uint8_t i;
	uint8_t rcvdata = 0;		//这里一定要赋初值为0
	
	SCL_LOW();
	sdaIn();		//设置输入模式接收数据
	
	for(i = 0; i < 8; i++)
	{
		SCL_HIGH();
		Delay_Us(2);
		rcvdata = rcvdata << 1;
		if(SDA_READ())
		{
			rcvdata |= 0x01;
		}
		SCL_LOW();
		Delay_Us(2);
	}

	if(ack == IIC_ACK)
		IIC_SendAck();
	else
		IIC_SendNoack();

	return rcvdata;
}
 


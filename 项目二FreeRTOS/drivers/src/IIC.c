#include "IIC.h"
#include "delay.h"
#include "UART.h"
/*****由于使用的延时函数不是精准延时 所以延时函数的参数与实际值有一定差异******/

/*通过软件模拟的方式实现EEPROM通信 主机为单片机 从机为EEPROM*/

#define SCL_High() GPIO_SetBits(GPIOB,GPIO_Pin_6)//将SCL线拉高
#define SCL_Low() GPIO_ResetBits(GPIOB,GPIO_Pin_6)//将SCL线拉低
#define SDA_High() GPIO_SetBits(GPIOB,GPIO_Pin_7)//将SDA线拉高
#define SDA_Low() GPIO_ResetBits(GPIOB,GPIO_Pin_7)//将SDA线拉低
#define SDA_Read() GPIO_ReadInputDataBit(GPIOB,  GPIO_Pin_7)//读取数据线电平

/*
**@函数名:IIC_GPIO_Pin_InitCnofig
**@函数功能:EEPROM 引脚初始化配置
**@函数参数:无
**@函数返回值:无
*/

void IIC_GPIO_Pin_InitCnofig(void)
{
	//1.打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;//定义一个结构体 保存GPIO配置信息
	
	//结构体成员初始化  PB6 PB7配置为开漏输出(防止多主机时 主机之间出现短路)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO初始化 配置输入输出模式
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);//使总线空闲电平为高电平
	

}


/*
**@函数名:IIC_SDA_Output
**@函数功能:数据线输出数据时 将其配置为开漏输出模式 这里的输入输出是相对于主机来说的
**@函数参数:无
**@函数返回值:无
*/

void IIC_SDA_Output(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;//定义一个结构体 保存GPIO配置信息

	//结构体成员初始化  PB7配置为开漏输出
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB,&GPIO_InitStruct);
}



/*
**@函数名:IIC_SDA_Input
**@函数功能:数据线输入数据时 将其配置为浮空输入模式 这里的输入输出是相对于主机来说的
**@函数参数:无
**@函数返回值:无
*/

void IIC_SDA_Input(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;//定义一个结构体 保存GPIO配置信息

	//结构体成员初始化  PB7配置为浮空输入
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB,&GPIO_InitStruct);
}



/*
**@函数名:IIC_Start
**@函数功能:模拟IIC的起始信号 即时钟线为高电平时 数据线产生一个下降沿
**@函数参数:无
**@函数返回值:无
*/

void IIC_Start(void)
{

	IIC_SDA_Output();//将数据线设置为输出模式
	
	SDA_High();//将数据线拉高
	SCL_High();//将时钟线拉高
	delay_us(8);//延时5us

	SDA_Low();//将数据线拉低 在SCL为高电平时数据线产生一个下降沿
	delay_us(8);//延时5us
	SCL_Low();//将时钟线拉低 防止误触产生停止信号
}



/*
**@函数名:IIC_Stop
**@函数功能:模拟IIC的停止信号 即时钟线为高电平时 数据线产生一个上升沿
**@函数参数:无
**@函数返回值:无
*/

void IIC_Stop(void)
{
//	SCL_Low();//拉低后配置数据线模式 防止干扰
	IIC_SDA_Output();//将数据线设置为输出模式
	SCL_Low();
	SDA_Low();//将数据线拉低
	delay_us(8);
	SCL_High();//将时钟线拉高
	SDA_High();
	delay_us(8);//延时5us
	//停止信号产生后 总线空闲 空闲时总线应为高电平 所以不需要将时钟线拉低
}



/*
**@函数名:IIC_Send_ACK
**@函数功能:模拟IIC 发送肯定应答信号 即时钟线为高电平时 数据线为低电平
**@函数参数:无
**@函数返回值:无
*/

void IIC_Send_ACK(void)
{
	SCL_Low();//拉低后配置数据线模式 防止干扰
	IIC_SDA_Output();//将数据线设置为输出模式
	SDA_Low();//将数据线拉低
	delay_us(8);//延时5us
	SCL_High();//将时钟线拉高 使时钟线为高时 数据线为低
	delay_us(8);//保持高电平状态一段时间
	SCL_Low();//将时钟线拉低 应答信号结束
}



/*
**@函数名:IIC_Send_NACK
**@函数功能:模拟IIC 发送否定应答信号 即时钟线为高电平时 数据线为高电平
**@函数参数:无
**@函数返回值:无
*/

void IIC_Send_NACK(void)
{
	SCL_Low();//拉低后配置数据线模式 防止干扰
	IIC_SDA_Output();//将数据线设置为输出模式
	
	SDA_High();//将数据线拉高
	delay_us(8);//延时5us 
	SCL_High();//将时钟线拉高 使时钟线为高时 数据线为低
	delay_us(8);//保持高电平状态一段时间
	SCL_Low();//将时钟线拉低 应答信号结束
//	SDA_High();//释放数据线
}



/*
**@函数名:IIC_Wait_ACK
**@函数功能:模拟IIC 等待肯定应答信号
**@函数参数:无
**@函数返回值:返回应答信号 它的值可以为0（肯定应答）或1（否定应答)
*/

uint8_t IIC_Wait_ACK(void)
{
	IIC_SDA_Input();//将数据线配置为输入模式 因为主机需要读取从机发送过来的应答信号
					//应答信号只有一位 且为0
	SDA_High();
	delay_us(10);
	SCL_High();//SCL为高电平期间 数据稳定 可以读取数据
	delay_us(10);
	uint8_t errtime = 0;//定义一个错误时间 如果这个时间大于255 则表示发生错误
	while(SDA_Read())
		{
			errtime++;
			if(errtime > 255)
				{
					IIC_Stop();
					return 1;
				}
			
		}
	SCL_Low();//钳制总线 防止产生停止信号
	return 0;
}



/*
**@函数名:IIC_Send_Byte
**@函数功能:模拟IIC 发送一字节数据
**@函数参数:无
**@函数返回值:无
*/

void IIC_Send_Byte(uint8_t data)
{
	
	IIC_SDA_Output();//数据线设置为输出模式 因为SDA此时需要发送数据
	uint8_t i = 0;
	SCL_Low();//时钟线拉低 数据可以改变
	for(i = 0;i < 8;i++)//循环发送8位数据
		{
			if(data & 0x80)//判断data最高位是 0还是1 是1则SDA拉高 是0则SDA拉低
				{
					SDA_High();
				}
			else
				{
					SDA_Low();
				}
			data = data<<1;//每发送完一位数据 则data左移一位 确保每次需要发送的位都是最高位

		//每发送一位数据 经过一个时钟周期 以下代码用于模拟出1个时钟周期
		delay_us(8);
		SCL_High();
		delay_us(8);
		SCL_Low();//每发送完一位数据 将时钟线拉低 保证发送下一位数据时时钟线为低 数据可以改变
		}
}




/*
**@函数名:IIC_Receive_Byte
**@函数功能:模拟IIC 接收一字节数据
**@函数参数:无
**@函数返回值:无
*/

uint8_t IIC_Receive_Byte(uint8_t ack)
{	

	IIC_SDA_Input();//数据线设置为输入模式 此时需要接收从机发送来的数据
	uint8_t i = 0;
	uint8_t ReceiveData = 0;
	for(i = 0;i < 8;i++)//循环发送8位数据
		{
			SCL_Low();
			delay_us(8);
			SCL_High();
			ReceiveData <<= 1;
			if(SDA_Read())//判断data最高位是 0还是1 是1则SDA拉高 是0则SDA拉低
				{
					ReceiveData |=0x01; 
				}
		//每发送一位数据 经过一个时钟周期 以下代码用于模拟出1个时钟周期
		delay_us(8);
		}
	if(ack)
		{
			IIC_Send_NACK();//如果参数ack为1 发送否定应答 不再接收数据
		}
	else
		IIC_Send_ACK();//如果参数ack为0 发送肯定应答 继续接收数据
	return ReceiveData;
}


/*
**@函数名:EEPROM_Write_Byte
**@函数功能:使用 EEPROM 以IIC通信协议发送数据
**@函数参数:addr
************@这个参数代表数据传输中存储空间的地址 即数据的存放地址
************@data
************@这个参数代表需要发送的数据
**@函数返回值:返回错误码 0xee代表出错 0xff代表正常
*/

uint8_t EEPROM_Write_Byte(uint8_t addr,uint8_t data)
{
	IIC_Start();
	IIC_Send_Byte(0xA0);//发送设备地址加一位读位

	//接收应答信号
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//停止通信
			printf("device addr err!");
			return 0xee;//返回错误码
		}
	
	IIC_Send_Byte(addr);//发送存储空间地址
	//接收应答信号
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//停止通信
			printf("word addr err!");
			return 0xee;//返回错误码
		}
	
	IIC_Send_Byte(data);//发送数据
	//接收应答信号
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//停止通信
			printf("Send date stop");
			return 0xee;//返回错误码
		}
	IIC_Stop();
	return 0xff;//返回正确码
}



/*
**@函数名:EEPROM_Read_Byte
**@函数功能:使用 EEPROM 以IIC通信协议读取数据
**@函数参数:addr
************@这个参数代表数据传输中存储空间的地址 即数据的存放地址
**@函数返回值:返回接收到的数据
*/

uint8_t EEPROM_Read_Byte(uint8_t addr)
{
	uint8_t receivedata = 0;
	IIC_Start();
	IIC_Send_Byte(0xA0);//发送设备地址加一位写位

	//接收应答信号
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//停止通信
			printf("device addr err!");
			return 0xee;//返回错误码
		}
	
	IIC_Send_Byte(addr);//发送存储空间地址
	//接收应答信号
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//停止通信
			printf("word addr err!");
			return 0xee;//返回错误码
		}
	
	IIC_Start();
	
	IIC_Send_Byte(0xA1);//发送设备地址加一位读位
	//接收应答信号
	if(1 == IIC_Wait_ACK())
		{
			IIC_Stop();//停止通信
			printf("Send date stop");
			return 0xee;//返回错误码
		}
	
	receivedata = IIC_Receive_Byte(1);//读取数据 读取完成不再接收数据
	IIC_Stop();//停止通信
	return receivedata;
}



/*
**@函数名:SGP30_InitConfig
**@函数功能:SGP30初始化
**@函数参数:无
**@函数返回值:无
*/
void SGP30_InitConfig(void)
{
	IIC_GPIO_Pin_InitCnofig();//GPIO初始化
	IIC_Start();//发送起始信号
	IIC_Send_Byte(0x58<<1);//发送七位设备地址号 加一位写位（0）
	IIC_Wait_ACK();//等待应答信号
	
	IIC_Send_Byte(0x20);//发送命令 初始化设备SGP30（0x2003） 的高八位
	IIC_Wait_ACK();//等待应答信号
	
	IIC_Send_Byte(0x03);//发送命令 初始化设备SGP30（0x2003） 的低八位
	IIC_Wait_ACK();//等待应答信号
	
	IIC_Stop();//发送停止信号
	delay_ms(1100);//等待设备初始化完成
	
}


/*
**@函数名:SGP30_Write
**@函数功能:SGP30写命令 例如0x2008 表示SGP30开始工作 检测空气质量
**@函数参数:
					a：命令高位/0x20
					b：命令低位/0x08
**@函数返回值:无
*/
void SGP30_Write(uint8_t a,uint8_t b)
{
	IIC_Start();//发送起始信号
	IIC_Send_Byte(0xb0);//发送设备地址加写位 0xb0
	IIC_Wait_ACK();//等待应答信号
	IIC_Send_Byte(a);//发送数据高八位
	IIC_Wait_ACK();//等待应答信号
	
	IIC_Send_Byte(b);//发送数据低八位
	IIC_Wait_ACK();//等待应答信号
	
	IIC_Stop();
	delay_ms(1100);//等待SGP30开始工作
	
}


/*
**@函数名:SGP30_Read
**@函数功能:SGP30读数据 
**@函数参数:无
**@函数返回值:返回CO2浓度和TVOC浓度
*/
void SGP30_Read(uint16_t * CO2,uint16_t* TVOC)
{
	uint8_t CRC_Check = 0;
	SGP30_Write(0x20,0x08);//发送命令0x2008 使SGP30开始工作
	
	IIC_Start();//发送起始信号
	IIC_Send_Byte(0xb1);//发送设备地址加读位
	IIC_Wait_ACK();//等待应答
	
	*CO2	|= (u16)(IIC_Receive_Byte(0)<<8);//8~15位 且接收完数据发送一个肯定应答
 	*CO2 |= (u16)(IIC_Receive_Byte(0));//0~7位
	CRC_Check = IIC_Receive_Byte(0);//接收16位数据 则有一个校验码
	*TVOC |= (u16)IIC_Receive_Byte(0)<<8;//8~15位 且接收完数据发送一个肯定应答
 	*TVOC |= (u16)(IIC_Receive_Byte(0));//0~7位 且接收完数据发送一个肯定应答
	CRC_Check = IIC_Receive_Byte(1);//接收16位数据 则有一个校验码 数据全部接收完毕 发送一个否定应答
	IIC_Stop();//发送停止信号 通信结束
	return;
	
}













#include "DHT11.h" 
#include "delay.h"

#define DHT11_High() GPIO_SetBits(GPIOA,GPIO_Pin_8)//将总线拉高
#define DHT11_Low() GPIO_ResetBits(GPIOA,GPIO_Pin_8)//将总线拉低
#define DHT11_Read() GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)//读取总线电平

/*****由于使用的延时函数不是精准延时 所以延时函数的参数与实际值有一定差异******/

/*
**@函数名:DHT11_GPIO_Pin_InitCnofig
**@函数功能:DHT11总线引脚初始化配置
**@函数参数:无
**@函数返回值:无
*/
void DHT11_GPIO_Pin_InitCnofig(void)
{
	//1.打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;//定义一个结构体 保存GPIO配置信息
	
	//结构体成员初始化
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//2.GPIO初始化 配置 PA8 输入输出模式
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_8);//使总线空闲电平为高电平
	

}


/*
**@函数名:DHT11_Output
**@函数功能:总线配置为输出模式 这里的输出是相对于MCU来说的
**@函数参数:无
**@函数返回值:无
*/

void DHT11_Output(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;//定义一个结构体 保存GPIO配置信息

	//结构体成员初始化  PA8配置为推挽输出
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);
}

/*
**@函数名:DHT11_Input
**@函数功能:总线配置为输入模式 这里的输入是相对于MCU来说的
**@函数参数:无
**@函数返回值:无
*/

void DHT11_Input(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;//定义一个结构体 保存GPIO配置信息

	//结构体成员初始化  PB7配置为开漏输出
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);
}


/*
**@函数名:DHT11_SendReset
**@函数功能:MCU发送起始信号
**@函数参数:无
**@函数返回值:无
*/
void DHT11_SendReset(void)	   
{                 
	DHT11_Output(); 	//设置IO为输出
	DHT11_Low(); 			//拉低总线
	delay_ms(40);    	//低电平维持至少18ms
	DHT11_High(); 			//拉高总线
	delay_us(40);     //高电平维持20~40us
}


/*
**@函数名:DHT11_CheckResponse
**@函数功能:MCU检测响应信号
**@函数参数:无
**@函数返回值:返回0表示出错 返回1表示接收到响应信号
*/
uint8_t DHT11_CheckResponse(void) 	   
{   
	uint8_t timecount=0;//设置一个错误时间
	DHT11_Input();		//设置IO为输入	 
    
	//正常响应情况下DHT11会拉低总线40~50us

	//若读取到高电平 表示还没有检测到响应 延时1us 当延时到100us时则表示出错
	while (DHT11_Read() && timecount<100)
	{
		timecount++;
		delay_us(1);
	}
	//跳出while	超时未检测到DHT11应答	则返回0
	//未超时即正常收到低电平信号	继续判断
	if(timecount>=100)
		return 0;
	else 
		timecount=0;
    //DHT11拉低总线后会再次拉高总线40~50us
	while (!DHT11_Read() && timecount<100)
	{
		timecount++;
		delay_us(1);
	}
	if(timecount>=100)
		return 0;	    
	//正确检测到了高电平且未超时	返回1
	return 1;
}


/*
**@函数名:DHT11_ReadBit
**@函数功能:读取一个bit的数据
**@函数参数:无
**@函数返回值:返回0表示低电平 返回1表示高电平
*/
uint8_t DHT11_ReadBit(void) 			 
{
 	uint8_t timecount=0;
	//0、1的信号都是先低电平再高电平
	//区别在于高电平的维持时间不同
	while(DHT11_Read() && timecount<100)
	{
		timecount++;
		delay_us(1);
	}
	timecount=0;
	while(!DHT11_Read() && timecount<100)//读取到高电平则退出while循环 表示数据到来 根据高电平持续时间判断是0还是1
	{
		timecount++;
		delay_us(1);
	}
	//根据DATASHEET描述的0与1格式	等待40us再判断总线
	delay_us(60);//其实此处是40us 但由于延时函数不精准 所以数值上有差异
	//若为高电平	则返回了1
	if(DHT11_Read())
		return 1;
	else 
		return 0;		   
}


/*
**@函数名:DHT11_ReadByte
**@函数功能:读取一个Byte的数据
**@函数参数:无
**@函数返回值:返回读取到的一个字节数据
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
**@函数名:DHT11_ReadData
**@函数功能:读取DHT11返回的所有数据
**@函数参数:*wendu:接收温度数据的变量 的地址   *shidu:接收湿度数据的变量 的地址
**@函数返回值:0表示读取错误 1表示读取正确
*/
uint8_t DHT11_ReadData(uint8_t *wendu,uint8_t *shidu)    
{   
 	uint8_t buf[5];//定义一个数组接收DHT11返回的所有 40bit 的数据 每一个元素一字节 
	uint8_t i;
	DHT11_SendReset();//MCU发送起始信号
	if(DHT11_CheckResponse() == 1)//MCU检测到响应信号 则开始读数据
	{
		for(i=0;i<5;i++)	//完整读取DHT11返回的40位数据
		{
			buf[i]=DHT11_ReadByte();
		}
		//根据DATASHEET提供的校验算法
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			//小数部分其实都为0	因此不取
			*shidu=buf[0];//返回湿度数据的整数部分
			*wendu=buf[2];//反回温度数据的整数部分
		}
	}
	else 
		return 0;
	
	return 1;	    
}



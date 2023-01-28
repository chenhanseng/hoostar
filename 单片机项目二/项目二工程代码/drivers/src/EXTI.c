#include "EXTI.h"
#include "SysTick.h"
#include "UART.h"

#define RUN 1
#define STOP 0
extern int work_mode;


/*
**@函数名：NVIC_InitConfig_EXTI
**@函数功能：外部中断9 配置函数
**@函数参数：无
**@函数返回值：无
*/
void NVIC_InitConfig_EXTI(void)
{
	//1.打开 GPIOC 外部时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	//2.初始化 PC9 引脚
	GPIO_InitTypeDef GPIO_InitStruct;//定义一个GPIO结构体 保存初始化状态
	
	//GPIO 引脚初始化配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; //工作模式设置为上拉输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//引脚选择PC9
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//工作速率设置为50MHZ
	
	//进行初始化
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//3.选择外部中断源 选择端口C 和 引脚PC9
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource9);
	
	
	//4.配置NVIC（为EXTI设置中断优先级）
	
	//针对每个中断 设置对应的抢占优先级和响应优先级
	NVIC_InitTypeDef NVIC_InitStruct;//定义一个与 NVIC 相关的结构体
	
	//结构体成员初始化
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;//中断通道选择为 外部中断线5-9
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//响应优先级
	
	NVIC_Init(&NVIC_InitStruct);//进行 NVIC 初始化
	
	
	//5.EXTI配置
	EXTI_InitTypeDef EXTI_InitStruct;//定义一个与外部中断相关的结构体
	
	//初始化结构体
	EXTI_InitStruct.EXTI_Line = EXTI_Line9;//选择外部中断线9
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//使能
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//触发类型设置为中断
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//设置为下降沿触发
	
	//进行初始化
	EXTI_Init(&EXTI_InitStruct);
	

	//6.完善中断处理函数
}

/*
**@函数名：NVIC_InitConfig_EXTI
**@函数功能：外部中断8 配置函数
**@函数参数：无
**@函数返回值：无
*/
void NVIC_InitConfig_EXTI8(void)
{
	//1.打开 GPIOC 外部时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	//2.初始化 PC9 引脚
	GPIO_InitTypeDef GPIO_InitStruct;//定义一个GPIO结构体 保存初始化状态
	
	//GPIO 引脚初始化配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; //工作模式设置为上拉输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;//引脚选择PC9
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//工作速率设置为50MHZ
	
	//进行初始化
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//3.选择外部中断源 选择端口C 和 引脚PC8
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource8);
	
	
	//4.配置NVIC（为EXTI设置中断优先级）
	
	//针对每个中断 设置对应的抢占优先级和响应优先级
	NVIC_InitTypeDef NVIC_InitStruct;//定义一个与 NVIC 相关的结构体
	
	//结构体成员初始化
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;//中断通道选择为 外部中断线5-9
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//响应优先级
	
	NVIC_Init(&NVIC_InitStruct);//进行 NVIC 初始化
	
	
	//5.EXTI配置
	EXTI_InitTypeDef EXTI_InitStruct;//定义一个与外部中断相关的结构体
	
	//初始化结构体
	EXTI_InitStruct.EXTI_Line = EXTI_Line8;//选择外部中断线8
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//使能
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//触发类型设置为中断
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//设置为下降沿触发
	
	//进行初始化
	EXTI_Init(&EXTI_InitStruct);
	

	//6.完善中断处理函数
}


//中断处理函数
/*
**@函数名：EXTI9_5_IRQHandler
**@函数功能：外部中断 8 9改变标志位
**@函数参数：无
**@函数返回值：无
*/
void EXTI9_5_IRQHandler(void)
{
	if(SET == EXTI_GetITStatus(EXTI_Line9))//外部中断标志位 置位 表示外部中断触发
	{
		EXTI_ClearITPendingBit(EXTI_Line9);//清除外部中断标志位
		work_mode = STOP;
	}
																																																			
		if(SET == EXTI_GetITStatus(EXTI_Line8))//外部中断标志位 置位 表示外部中断触发
	{
		EXTI_ClearITPendingBit(EXTI_Line8);//清除外部中断标志位
		work_mode = RUN;	
	}
}


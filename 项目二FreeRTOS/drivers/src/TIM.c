#include "TIM.h"
#include "UART.h"
#include <stdio.h>
#include "gizwits_product.h"
#include "gizwits_protocol.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#define TIM_ARR 255//TIM3重装载值
#define TIM_PSC 720//TIM3分频系数
/* 40ms的定时 */
#define TIM7_ARR 400//TIM3重装载值
#define TIM7_PSC 7200//TIM3分频系数	

#define RUN 1
#define STOP 0

/***声明外部变量***/
extern int work_mode;
extern int Flag_Task_close;
extern TaskHandle_t GETVALUE_Task_Handle;
extern TaskHandle_t OLED_Task_Handle;
extern TaskHandle_t GIZWITS_Task_Handle;

int TIM4_PSC = 7200;
int count = -1;

/*
**@函数名：TIM_InitConfig
**@函数功能：定时器 TIMx 中断的配置 每隔一段时间产生一次中断
**@函数参数：无
**@函数返回值：无
*/

void TIM_InitConfig(void)
{
	//1.打开 TIM6 的外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	
	//2.初始化定时器
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//定义与TIM相关的结构体
	
	//结构体初始化
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200-1;//配置PSC 预分频系数 确定计数频率
	TIM_TimeBaseInitStruct.TIM_Period = 10-1;//配置ARR 自动重装初值 确定计数的次数
	//确定计数方式 基本定时器可以不设置 默认向上计数
	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);//TIM初始化 初始化完成后就会产生一个更新事件标志位
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);//初始化完成后 清空事件更新标志位 确保计时完成才进入中断
	
	//3.使能定时器
	TIM_Cmd(TIM6,ENABLE);
	
	//4.初始化NVIC
	NVIC_InitTypeDef NVIC_InitStruct;//定义一个与 NVIC 相关的结构体
	
	//结构体成员初始化
	NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;//中断通道选择为 定时器6
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//响应优先级
	
	NVIC_Init(&NVIC_InitStruct);//进行 NVIC 初始化
	
	
	//5.配置中断源
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	
	//6.完成中断函数
	
}

//中断函数
/*
**@函数名：TIM6_IRQHandler
**@函数功能：每1ms秒进入一次中断 实现协议层系统时间的维护
**@函数参数：无
**@函数返回值：无
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
**@函数名：TIM7_InitConfig
**@函数功能：定时器 TIMx 中断的配置
**@函数参数：无
**@函数返回值：无
*/

void TIM7_InitConfig(void)
{
	//1.打开 TIM7 的外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	
	//2.初始化定时器
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//定义与TIM相关的结构体
	
	//结构体初始化
	TIM_TimeBaseInitStruct.TIM_Prescaler = TIM7_ARR-1;//配置PSC 预分频系数 确定计数频率
	TIM_TimeBaseInitStruct.TIM_Period = TIM7_PSC-1;//配置ARR 自动重装初值 确定计数的次数
	//确定计数方式 基本定时器可以不设置 默认向上计数
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStruct);//TIM初始化 初始化完成后就会产生一个更新事件标志位
	TIM_ClearFlag(TIM7,TIM_FLAG_Update);//初始化完成后 清空事件更新标志位 确保计时完成才进入中断
	
	//3.使能定时器
	//TIM_Cmd(TIM7,ENABLE); //放入按键中断配置 实现按键中断控制定时
	
	//4.初始化NVIC
	NVIC_InitTypeDef NVIC_InitStruct;//定义一个与 NVIC 相关的结构体
	
	//结构体成员初始化
	NVIC_InitStruct.NVIC_IRQChannel = TIM7_IRQn;//中断通道选择为 定时器6
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;//响应优先级
	
	NVIC_Init(&NVIC_InitStruct);//进行 NVIC 初始化
	
	
	//5.配置中断源
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
	
	//6.完成中断函数
	
}

//中断函数
/*
**@函数名：TIM7_IRQHandler
**@函数功能：按键中断下半部分 定时器计时结束 进入定时器中断
**@函数参数：无
**@函数返回值：无
*/

void TIM7_IRQHandler(void)
{

		if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM7, TIM_IT_Update );
			if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9))//消抖后若按键还是按下状态 执行按键中断真正需要执行的任务
			{
				TIM_Cmd(TIM7,DISABLE);//失能定时器 等待下一次按键中断到来 不关闭的话会每隔一段消抖时间进入一次TIM7_IRQHandler
				printf("运行模式！\n");
				Flag_Task_close = -1;//将任务挂起标志位恢复默认状态
				work_mode = RUN;
				if(pdFALSE == xTaskResumeFromISR( GETVALUE_Task_Handle)) // 唤醒参数获取任务
				{
					printf("GETVALUE_Task Resume failed!\n");
				}
				
				if(pdFALSE == xTaskResumeFromISR( OLED_Task_Handle)) // 唤醒OLED任务
				{
					printf("OLED_Task Resume failed!\n");
				}
				
				if(pdFALSE == xTaskResumeFromISR( GIZWITS_Task_Handle)) // 唤醒GIZWIT数据上行任务
				{
					printf("GIZWITS_Task Resume failed!\n");
				}
			}
			
			if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8))//消抖后若按键还是按下状态 执行按键中断真正需要执行的任务
			{
				TIM_Cmd(TIM7,DISABLE);//失能定时器 等待下一次按键中断到来 不关闭的话会每隔一段消抖时间进入一次TIM7_IRQHandler
				printf("停止模式！\n");
				work_mode = STOP;//当前系统工作模式设为停止
				Flag_Task_close = 1;//将任务挂起标志位置位 在各自的任务处理函数会将自身任务挂起
			//	__WFI();//STM32进入睡眠模式 关闭内核时钟
			}
			TIM_Cmd(TIM7,DISABLE);
		}
		
}



/*
**@函数名：TIM4_InitConfig
**@函数功能：使用定时器四对步进电机进行控制 调节每秒脉冲个数控制电机转速
**@函数参数：无
**@函数返回值：无
*/
//void TIM4_InitConfig(void)
//{
//	//GPIO初始化
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	GPIO_InitTypeDef GPIO_InitStruct;
//	
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
//	
//	
//	//1.打开 TIM3 的外设时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
//	
//	//2.初始化定时器
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//定义与TIM相关的结构体
//	
//	//结构体初始化
//	TIM_TimeBaseInitStruct.TIM_Prescaler = TIM4_PSC-1;//配置PSC 预分频系数 确定计数频率
//	TIM_TimeBaseInitStruct.TIM_Period = 30-1;//配置ARR 自动重装初值 确定计数的次数
//	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//确定计数方式 向上计数
//	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//	
//	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);//TIM初始化 初始化完成后就会产生一个更新事件标志位
//	TIM_ClearFlag(TIM4,TIM_FLAG_Update);//初始化完成后 清空事件更新标志位 确保计时完成才进入中断
//	
//	//3.使能定时器
//	TIM_Cmd(TIM4,ENABLE);
//	
//	//4.初始化NVIC
//	NVIC_InitTypeDef NVIC_InitStruct;//定义一个与 NVIC 相关的结构体
//	
//	//结构体成员初始化
//	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;//中断通道选择为 定时器4
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//响应优先级
//	
//	NVIC_Init(&NVIC_InitStruct);//进行 NVIC 初始化
//	
//	
//	//5.配置中断源
//	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
//	
//	//6.完成中断函数
//	
//}

////中断函数
///*
//**@函数名：TIM4_IRQHandler
//**@函数功能：计时器溢出进入一次中断 输出不同脉冲信号
//**@函数参数：无
//**@函数返回值：无
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
**@函数名：PWM_OutPut_InitConfig
**@函数功能：PWM模式输出 初始化配置 TIM3 通道1
**@函数参数：无
**@函数返回值：无
*/

void PWM_OutPut_InitConfig(void)
{
	//1.使能定时器3和相关IO口时钟 引脚复用为TIM的通道一 所以要打开复用时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//打开定时器 TIM3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);//打开GPIOC时钟 以及复用时钟
	
	//2.初始化IO口为复用功能输出
	GPIO_InitTypeDef GPIO_InitStruct;//定义一个与GPIO相关的结构体
	
	//初始化结构体
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//输入输出模式设置为 复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;//|GPIO_Pin_7|GPIO_Pin_8;//引脚选择PC6 PC7 PC8
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//设置速率
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);//进行初始化
	
	//3.把 PC6 用作定时器PWM输出引脚 所以要进行重映射配置
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);//设置重映射 这里选择为全部重映射
	
	//4.初始化定时器TIM3 配置ARR PSC等
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructl;//定义一个与TIM初始化有关的结构体
	
	//初始化结构体
	TIM_TimeBaseInitStructl.TIM_Prescaler = TIM_PSC;//分频系数
	TIM_TimeBaseInitStructl.TIM_Period = TIM_ARR;//重装载值
	TIM_TimeBaseInitStructl.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructl.TIM_ClockDivision = TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructl);//初始化TIM3
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);//初始化完成后会产生事件更新标志位 清空事件更新标志位
	
	
	//5.初始化输出比较参数
	TIM_OCInitTypeDef TIM_OCInitStruct;//定义一个与PWM通道 相关的结构体
	
	//初始化结构体
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//配置PWM模式
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//输出使能
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;//低电平有效 
	TIM_OCInitStruct.TIM_Pulse = 0;

	TIM_OC1Init(TIM3,&TIM_OCInitStruct);//初始化通道1
  //TIM_OC2Init(TIM3,&TIM_OCInitStruct);//初始化通道2
  //TIM_OC3Init(TIM3,&TIM_OCInitStruct);//初始化通道3
	
	//6.使能预装载寄存器 ARR和CCR
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能CCR1
  //TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能CCR2
  //TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能CCR3
	TIM_ARRPreloadConfig(TIM3,ENABLE);//使能ARR
	
	//7.使能定时器
	TIM_Cmd(TIM3,ENABLE);
	
	//8.不断改变比较值 CCRx 改变占空比 输出不同的电平大小
}










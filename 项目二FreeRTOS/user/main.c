#include <stdio.h>
#include <stdbool.h>
#include "UART.h"
#include "EXTI.h"
#include "TIM.h"
#include "ADC.h"
#include "DMA.h"
#include "IIC.h"
#include "DHT11.h"
#include "oled.h"
#include "gizwits_product.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/****函数声明****/
static void AppTaskCreate(void);
static void GETVALUE_Task(void* parameter);
static void OLED_Task(void* parameter);
static void GIZWITS_Task(void* parameter);

/******宏定义******/
#define RUN 1
#define STOP 0

/****定义任务句柄****/
static TaskHandle_t AppTaskCreate_Handle = NULL;//创建任务句柄
TaskHandle_t GETVALUE_Task_Handle = NULL;//参数采集任务句柄
TaskHandle_t OLED_Task_Handle = NULL;//OLED显示任务句柄
TaskHandle_t GIZWITS_Task_Handle = NULL;//机智云数据上行任务句柄

static SemaphoreHandle_t MuxSem_Handle = NULL;//定义一个信号量句柄

/***定义全局变量***/
int ret;
int temp1;
int temp2;
int work_mode = RUN;//定义并初始化系统工作模式 RUN代表运行模式 STOP代表待机模式 
int Flag_Task_close = -1;

uint8_t wendu_value = 0;//定义温度变量
uint8_t shidu_value = 0;//定义湿度变量
uint32_t smoke_value = 0;//定义烟雾浓度值
uint16_t CO2_value = 0;//定义二氧化碳浓度变量
uint16_t TVOC_value = 0;//定义TVOC浓度变量
float V_value = 0;//定义电压采集变量

dataPoint_t currentDataPoint;//定义一个数据上行结构体 保存所有数据点的数据

/*
**@函数名：Gizwits_Init
**@函数功能：机智云设备初始化
**@函数参数：无
**@函数返回值：无
*/
void Gizwits_Init(void)
{
	TIM_InitConfig();//TIM6 实现1MS系统定时
	UART3_InitConfig(9600);//WIFI通信串口初始化
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//设备状态结构体初始化 清空结构体
	gizwitsInit();
}

//硬件初始化函数
void BSP_Init(void)
{
	/*****************************************初始化部分*****************************************************/
	//1.先设置中断优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//调用中断优先级分组函数 
	UART_InitConfig();//调用串口初始化函数
	TIM7_InitConfig();//定时器初始化 但定时器未使能 通过外部中断触发定时器
	NVIC_InitConfig_EXTI();//配置外部中断9
	NVIC_InitConfig_EXTI8();//配置外部中断8
	PWM_OutPut_InitConfig();//调用PWM输出初始化函数
	ADC1_InitConfig();//调用ADC1初始化函数
	DMA_InitConfig_P2M();//打开ADC1 的DMA请求 搬运数据
	DHT11_GPIO_Pin_InitCnofig();//DHT11引脚初始化 配置PA8为总线
	OLED_Init();//初始化OLED 
	OLED_Clear();//清屏
	
	SGP30_InitConfig();//调用SGP30 初始化函数
	Gizwits_Init();//调用机智云初始化函数
	gizwitsSetMode(WIFI_AIRLINK_MODE);
	printf("初始化完成！\n");
/******************************************************************************************************/
}

int main(void)
{
	/* 开发板硬件初始化 */
			BSP_Init();
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
     /* 创建任务AppTaskCreate*/
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate, //任务函数
                        (const char*    )"AppTaskCreate",//任务名称
                        (uint16_t       )256,						//任务堆栈大小
                        (void*          )NULL,					//传递给任务函数的参数 没有可以填NULL
                        (UBaseType_t    )1,							//任务优先级
                        (TaskHandle_t*  )&AppTaskCreate_Handle);//任务句柄
	
  if(pdPASS == xReturn)
    vTaskStartScheduler();  //开启任务调度
  else
    return -1;  
  
  while(1); 
	
}


//环境参数采集任务的函数 获取环境参数值
static void GETVALUE_Task(void* parameter)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	while(1)
	{
		if(Flag_Task_close == 1)//任务挂起标志位置位 则挂起任务
		{
			vTaskSuspend(GETVALUE_Task_Handle);
		}
		
		//获取互斥量 MuxSem,没获取到则一直等待
		xReturn = xSemaphoreTake(MuxSem_Handle,/* 互斥量句柄 */
                              portMAX_DELAY); /* 等待时间 一直阻塞直到成功获取互斥量 */
		if(work_mode == RUN)
		{
			
		printf("优先级：4\n");		
		/***********************************信号量采集部分**************************************/
		smoke_value = GetValue_ADC_Average(0);//DMA将搬运的数据存入数组中 调用此函数将采集的烟雾浓度数据提取出来
		V_value = 2*GetValue_ADC_Average(1)*3.3/4095;//DMA将搬运的数据存入数组中 调用此函数将采集的电压数据提取出来
		
		DHT11_ReadData(&wendu_value,&shidu_value);//调用DHT11数据读取函数 读取湿度和温度
		SGP30_Read(&CO2_value,&TVOC_value);//读取CO2和TVOC含量
		
		printf("温度：%d  ",wendu_value);
		printf("湿度：%d  ",shidu_value);
		printf("CO2浓度：%d  ",CO2_value);
		printf("TVOC浓度：%d  ",TVOC_value);
		printf("烟雾：%d  ",smoke_value);
		printf("电压：%.2f  \n",V_value);
		
		temp1 = V_value*10;
		temp2 = V_value*100;
		temp1 = temp1%10;//V_value的第一位小数
		temp2 = temp2%100;//V_value的第二位小数
		delay_ms(1000);
		
		}		
		xReturn = xSemaphoreGive( MuxSem_Handle );//给出互斥量
		vTaskDelay(1000);     //每1000ms扫描一次	
	}
	
}


//OLED任务的函数 显示环境参数值
static void OLED_Task(void* parameter)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	u8 chr[] = "CO2:";
	u8 str[] = "TVOC:";
	while(1)
	{
		if(Flag_Task_close == 1)//任务挂起标志位置位 则挂起任务
		{
			vTaskSuspend(OLED_Task_Handle);
		}
		
		//获取互斥量 MuxSem,没获取到则一直等待
		xReturn = xSemaphoreTake(MuxSem_Handle,/* 互斥量句柄 */
                              portMAX_DELAY); /* 等待时间 一直阻塞直到成功获取互斥量 */
		
		if(work_mode == RUN)
		{
			printf("优先级：3_OLED\n");
		/**************************************OLED显示部分***********************************/
		char dian = '.';
		OLED_Clear();//清屏
		OLED_ShowCHinese(0,0,7);//温
		OLED_ShowCHinese(18,0,8);//度
		OLED_ShowCHinese(36,0,9);//：
		OLED_ShowNum(40,0,wendu_value,2,16);//显示温度的值
		
		OLED_ShowCHinese(72,0,10);//湿
		OLED_ShowCHinese(90,0,11);//度
		OLED_ShowCHinese(108,0,12);//：
		OLED_ShowNum(112,0,shidu_value,2,16);//显示温度的值 
		
		OLED_ShowCHinese(0,3,18);//电
		OLED_ShowCHinese(18,3,19);//压
		OLED_ShowCHinese(36,3,20);//：
		OLED_ShowNum(40,3,V_value,1,16);//显示电压的整数
		OLED_ShowChar(52,3,dian);//显示'.'
		OLED_ShowNum(64,3,temp1,1,16);//显示电压的第一位小数
		OLED_ShowNum(76,3,temp2,1,16);//显示电压的第二位小数

		OLED_ShowCHinese(0,6,13);//烟
		OLED_ShowCHinese(18,6,14);//雾
		OLED_ShowCHinese(36,6,15);//浓
		OLED_ShowCHinese(54,6,16);//度
		OLED_ShowCHinese(72,6,17);//：
		OLED_ShowNum(80,6,smoke_value,4,16);//显示烟雾浓度的整数
		
		delay_ms(1000);
		OLED_Clear();//清屏
		OLED_ShowString(0,0,chr);//显示字符串CO2:
		OLED_ShowNum(30,0,CO2_value,4,16);//显示CO2浓度的值
		
		OLED_ShowString(0,3,str);//显示字符串TVOC:
		OLED_ShowNum(40,3,TVOC_value,4,16);//显示TVOC浓度的值
		}
		xReturn = xSemaphoreGive( MuxSem_Handle );//给出互斥量
		vTaskDelay(1000);     //每20ms扫描一次	
	}
}

//GIZWITS任务的函数 实现机智云数据的上行
static void GIZWITS_Task(void* parameter)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	while(1)
	{
		if(Flag_Task_close == 1)//任务挂起标志位置位 则挂起任务
		{
			vTaskSuspend(GIZWITS_Task_Handle);
		}
		
		//获取互斥量 MuxSem,没获取到则一直等待
		xReturn = xSemaphoreTake(MuxSem_Handle,/* 互斥量句柄 */
                              portMAX_DELAY); /* 等待时间 一直阻塞直到成功获取互斥量 */
	
		if(work_mode == RUN)
		{
				printf("优先级：2_GIZWITS\n");
			/******机智云数据上行部分*******/
		//结构体成员赋值
		currentDataPoint.valuewendu_value = wendu_value;//Add Sensor Data Collection
    currentDataPoint.valueshidu_value = shidu_value;//Add Sensor Data Collection
    currentDataPoint.valueV_value = V_value;//Add Sensor Data Collection
    currentDataPoint.valuesmoke_value = smoke_value;//Add Sensor Data Collection
    currentDataPoint.valueCO2_value = CO2_value;//Add Sensor Data Collection
    currentDataPoint.valueTVOC_value = TVOC_value;//Add Sensor Data Collection
		
		userHandle();//调用此函数给结构体成员赋值 即将采集到的信号量传入结构体中
		gizwitsHandle((dataPoint_t *)&currentDataPoint);//协议处理 调用此函数上报数据到服务器
		}
		xReturn = xSemaphoreGive( MuxSem_Handle );//给出互斥量
		vTaskDelay(1000);     //每20ms扫描一次	
	}
}


//创建所需任务
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;
  taskENTER_CRITICAL();           //进入临界区
	
	/* 创建MuxSem */
  MuxSem_Handle = xSemaphoreCreateMutex();	//创建互斥信号量 带有优先级继承 
  if(NULL != MuxSem_Handle)
    printf("MuxSem_Handle互斥量创建成功!\r\n");

  xReturn = xSemaphoreGive( MuxSem_Handle );//给出互斥量
	
	  /* GETVALUE_Task 任务 */
	xReturn = xTaskCreate((TaskFunction_t )GETVALUE_Task,
                        (const char*    )"GETVALUE_Task",
                        (uint16_t       )256,
                        (void*          )NULL,
                        (UBaseType_t    )4,
                        (TaskHandle_t*  )&GETVALUE_Task_Handle);
  if(pdPASS == xReturn)
    printf("GETVALUE_Task 任务创建成功\r\n");
	
	/* OLED_Task 任务 OLED显示 */
	xReturn = xTaskCreate((TaskFunction_t )OLED_Task,
                        (const char*    )"OLED_Task",
                        (uint16_t       )256,
                        (void*          )NULL,
                        (UBaseType_t    )3,
                        (TaskHandle_t*  )&OLED_Task_Handle);
  if(pdPASS == xReturn)
    printf("OLED_Task 任务创建成功\r\n");
	
	/* GIZWITS_Task 任务 机智云的数据上行 */
	xReturn = xTaskCreate((TaskFunction_t )GIZWITS_Task,
                        (const char*    )"GIZWITS_Task",
                        (uint16_t       )128,
                        (void*          )NULL,
                        (UBaseType_t    )2,
                        (TaskHandle_t*  )&GIZWITS_Task_Handle);
  if(pdPASS == xReturn)
    printf("GIZWITS_Task 任务创建成功\r\n");
    
  vTaskDelete(AppTaskCreate_Handle); //删除任务
  taskEXIT_CRITICAL();            //退出临界区
}


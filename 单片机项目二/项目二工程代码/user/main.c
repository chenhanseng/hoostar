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

//注：模式的切换可以使用按键中断 消抖通过定时器消抖

/******宏定义******/
#define RUN 1
#define STOP 0

int ret;
int temp1;
int temp2;
int work_mode = RUN;//定义并初始化系统工作模式 RUN代表运行模式 STOP代表待机模式 

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


int main(void)
{
	
	
/*****************************************初始化部分*****************************************************/
	//1.先设置中断优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//调用中断优先级分组函数
	NVIC_InitConfig_USART();//调用串口中断配置函数 
	
	UART_InitConfig();//调用串口初始化函数
	SysTick_InitConfig(72);//调用嘀嗒定时器初始化函数
	PWM_OutPut_InitConfig();//调用PWM输出初始化函数
	ADC1_InitConfig();//调用ADC1初始化函数
	DMA_InitConfig_P2M();//打开ADC1 的DMA请求 搬运数据

	DHT11_GPIO_Pin_InitCnofig();//DHT11引脚初始化 配置PA8为总线
	
	OLED_Init();//初始化OLED  
	OLED_Clear();//清屏
	
	SGP30_InitConfig();//调用SGP30 初始化函数
	

	Gizwits_Init();//调用机智云初始化函数
	gizwitsSetMode(WIFI_AIRLINK_MODE);
	printf("机智云初始化\n");

	u8 chr[] = "CO2:";
	u8 str[] = "TVOC:";

/******************************************************************************************************/

	while(1){
		
		//结构体成员赋值
		currentDataPoint.valuewendu_value = wendu_value;//Add Sensor Data Collection
    currentDataPoint.valueshidu_value = shidu_value;//Add Sensor Data Collection
    currentDataPoint.valueV_value = V_value;//Add Sensor Data Collection
    currentDataPoint.valuesmoke_value = smoke_value;//Add Sensor Data Collection
    currentDataPoint.valueCO2_value = CO2_value;//Add Sensor Data Collection
    currentDataPoint.valueTVOC_value = TVOC_value;//Add Sensor Data Collection
		
		userHandle();//调用此函数给结构体成员赋值 即将采集到的信号量传入结构体中
		gizwitsHandle((dataPoint_t *)&currentDataPoint);//协议处理 调用此函数上报数据到服务器
		
/**判断当前系统工作模式 如果是STOP模式 检测唤醒按键是否按下 按下则切换模式为RUN**/
	if(work_mode == STOP){
			if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)){
				delay_ms(10);
			if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)){
			work_mode = RUN;
			printf("运行模式！");
			}
		}
		
	}

	
/**判断当前系统工作模式 如果是RUN模式则执行功能代码**/
	if(work_mode == RUN){	

/***********************************信号量采集部分**************************************/
		smoke_value = GetValue_ADC_Average(0);//DMA将搬运的数据存入数组中 调用此函数将采集的烟雾浓度数据提取出来
		V_value = 2*GetValue_ADC_Average(1)*3.3/4095;//DMA将搬运的数据存入数组中 调用此函数将采集的电压数据提取出来
		
		SGP30_Read(&CO2_value,&TVOC_value);//读取CO2和TVOC含量
		DHT11_ReadData(&wendu_value,&shidu_value);//调用DHT11数据读取函数 读取湿度和温度
		printf("温度：%d",wendu_value);
		printf("湿度：%d",shidu_value);
		printf("CO2浓度：%d",CO2_value);
		printf("TVOC浓度：%d",TVOC_value);
		printf("烟雾：%d",smoke_value);
		printf("电压：%.2f",V_value);
		
		temp1 = V_value*10;
		temp2 = V_value*100;
		temp1 = temp1%10;//V_value的第一位小数
		temp2 = temp2%100;//V_value的第二位小数

		
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
		
		
/*******************检测待机按键是否按下 按下则切换模式为STOP 进入待机状态*****************/
		if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)){ 
			delay_ms(10);
		if(0 == GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)){ 
			work_mode = STOP;
			printf("待机模式！");
			OLED_Clear();//清屏	
			}
		}
			
	}
}

}




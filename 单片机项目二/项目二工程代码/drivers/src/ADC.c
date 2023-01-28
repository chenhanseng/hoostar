#include "ADC.h"
#include "SysTick.h"

/*
**@函数名：ADC1_InitConfig
**@函数功能：ADC1初始化配置 通道10->PC0 通道11->PC1
**@函数参数：无
**@函数返回值：无
*/

void ADC1_InitConfig(void)
{
	//1.开启 PC口时钟 和 ADC1时钟 复位时钟 设置PC0为模拟输入
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO,ENABLE);
	
	//初始化PC0
	GPIO_InitTypeDef GPIO_InitStruct;//定义一个GPIO结构体
	
	//初始化结构体
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN; //工作模式设置为模拟输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;//选择引脚 PC0 PC1
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//工作速率设置为50MHZ
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);//初始化GPIO
	
	//2.复位ADC1 同时设置ADC1分频因子
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//设置ADC工作频率为 APB2的6分频
	
	//3.初始化ADC1参数 设置ADC1的工作模式以及规则序列的相关信息
	ADC_InitTypeDef ADC_InitStruct;//定义一个与ADC初始化有关的结构体
	
	//初始化结构体
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//工作模式设置为独立工作模式
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//配置扫描模式 即一个通道采集一次完成后按规则组顺序采集下一个通道
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//选择连续转换
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//无外部触发 选择软件触发
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//数据对齐方式为 右对齐
	ADC_InitStruct.ADC_NbrOfChannel = 2;//配置规则组的通道数量
	
	ADC_Init(ADC1,&ADC_InitStruct);//初始化ADC1
	
	//4.配置规则通道参数
	//配置ADC组1 通道转换顺序以及转换时间
	ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_239Cycles5);//通道10转换顺序为1
	ADC_RegularChannelConfig(ADC1,ADC_Channel_11,2,ADC_SampleTime_239Cycles5);//通道11转换顺序为2

	//5.使能ADC1并校准  <*必须先使能再校准*>
	ADC_Cmd(ADC1,ENABLE);//使能ADC
	
	ADC_ResetCalibration(ADC1);//复位校准ADC1
	while(SET == ADC_GetResetCalibrationStatus(ADC1));//等待复位校准完成
	ADC_StartCalibration(ADC1);//校准ADC1
	while(SET == ADC_GetCalibrationStatus(ADC1));//等待校准完成
	
	
	//6.开启软件触发转换
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	
	//7.等待转换完成 读取ADC值
	

}

#include "DMA.h"
#include "UART.h"
#include "SysTick.h" 
uint16_t buf[30][2];//定义一个二维数组保存DMA搬运来的数据 每一列对应一个ADC通道采集的数据 每一行存储16位的数据 共30行
uint8_t SRC[10] = {'A','B','C','D','E','F','G','H','I','\n'};//内存源数组 末位添加\n保证数据不会乱码
uint8_t DST[10];//内存目标数组

/*
**@函数名：DMA_InitConfig_M2M
**@函数功能：DMA初始化配置 使用DMA搬运数据 内存到内存
**@函数参数：无
**@函数返回值：无
*/

void DMA_InitConfig_M2M(void)
{

	//1.打开 DMA 时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//DAM挂载在AHB时钟线下
	
	//2.初始化配置
	DMA_InitTypeDef DMA_InitStruct;//定义一个与DMA初始化有关的结构体
	
	//初始化结构体
	DMA_InitStruct.DMA_BufferSize = 10;//buf数组的大小 共10个元素
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//配置 外设 作为DMA搬运中的 目标地址
	DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;//搬运方向为内存到内存
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)SRC;//配置内存的基地址为 SRC 数组首地址
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//设置内存的数据宽度为一字节
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存每个数组元素都有其地址 所以地址需要偏移
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//DMA模式设置为单次传输
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DST;//配置外设基地址为 DST数组 的地址
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//设置外设的数据宽度为一字节
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;//外设为数组 每个元素都有地址 需要地址偏移
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;//配置DMA1 通道4 的优先级为高
 	
	DMA_Init(DMA1_Channel4,&DMA_InitStruct);//初始化DMA配置 DMA1 通道4
	
	//3.使能
	DMA_Cmd(DMA1_Channel4,ENABLE);//内存到内存 无指定外设 可以选取任意通道
	
	//4.配置相应外设的DAM请求
	//USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//打开USART1 的DMA请求
	
}


/*
**@函数名：Get_DST_value
**@函数功能：获取目标数组的内容
**@函数参数：无
**@函数返回值：无
*/

void Get_DST_value(void)
{
 uint8_t i;
 for(i = 0; i < 10; i++){
  printf("%c\n",DST[i]);
  delay_ms(1000);
 }
}


/*
**@函数名：DMA_InitConfig_M2P
**@函数功能：DMA初始化配置 使用DMA搬运数据 内存到外设
**@函数参数：无
**@函数返回值：无
*/

void DMA_InitConfig_M2P(void)
{

	//1.打开 DMA 时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//DAM挂载在AHB时钟线下
	
	//2.初始化配置
	DMA_InitTypeDef DMA_InitStruct;//定义一个与DMA初始化有关的结构体
	
	//初始化结构体
	DMA_InitStruct.DMA_BufferSize = 10;//buf数组的大小 共10个元素
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//配置外设作为DMA搬运中的目标地址
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//搬运方向为内存到外设
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)SRC;//配置内存的基地址为 SRC 数组首地址
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//设置内存的数据宽度为半字
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存每个数组元素都有其地址 所以地址需要偏移
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;//DMA模式设置为循环传输
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//配置外设基地址为 USART1 的数据寄存器地址
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//设置外设的数据宽度为半字
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设寄存器地址只有一个 不需要地址偏移
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;//配置DMA1 通道1 的优先级为高
 	
	DMA_Init(DMA1_Channel4,&DMA_InitStruct);//初始化DMA配置 DMA1 通道4
	
	//3.使能
	DMA_Cmd(DMA1_Channel4,ENABLE);
	
	//4.配置相应外设的DAM请求
	//选择串口的发送数据寄存器 将数据搬运到TX内即自动通过串口发送到串口调试助手 方便验证
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//打开USART1的DMA请求
	
}


/*
**@函数名：DMA_InitConfig_P2M
**@函数功能：DMA初始化配置 使用DMA搬运数据 外设到内存
**@函数参数：无
**@函数返回值：无
*/

void DMA_InitConfig_P2M(void)
{

	//1.打开 DMA 时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//DAM挂载在AHB时钟线下
	
	//2.初始化配置
	DMA_InitTypeDef DMA_InitStruct;//定义一个与DMA初始化有关的结构体
	
	//初始化结构体
	DMA_InitStruct.DMA_BufferSize = 30*2;//buf数组的大小 共30*2个元素
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;//配置外设作为DMA搬运中的源地址
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//搬运方向为外设到内存 不需要配置内存到内存的搬运方向
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)buf;//配置内存的基地址为我们所定义的buf数组首地址
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//设置内存的数据宽度为半字
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存每个数组元素都有其地址 所以地址需要偏移
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;//DMA模式设置为循环传输
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//配置外设基地址为 ADC1 的数据寄存器地址
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//设置外设的数据宽度为半字
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设寄存器地址只有一个 不需要地址偏移
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;//配置DMA1 通道1 的优先级为最高
 	
	DMA_Init(DMA1_Channel1,&DMA_InitStruct);//初始化DMA配置 DMA1 通道1
	
	//3.使能
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	//4.配置相应外设的DAM请求
	ADC_DMACmd(ADC1,ENABLE);//打开ADC1 的DMA请求
	
}


/*
**@函数名：GetValue_ADC_Average
**@函数功能：获取ADC采集到的30次的平均值
**@函数参数：channel：ADC的通道顺序 即buf数组中的第channel列
**@函数返回值：ADC采集到的30次数据的平均值
*/

uint16_t GetValue_ADC_Average(uint32_t channel)
{
	uint32_t i = 0,sum = 0;
	for(i = 0;i < 30;i++){
	sum += buf[i][channel];
	}
	return (uint16_t)(sum/30);//返回ADC采集到的30次数据的平均值
	
	
}



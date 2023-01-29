/**
************************************************************
* @file         gizwits_product.c
* @brief        Gizwits control protocol processing, and platform-related       hardware initialization 
* @author       Gizwits
* @date         2017-07-19
* @version      V03030000
* @copyright    Gizwits
* 
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/

#include <stdio.h>
#include <string.h>
//#include "hal_key.h"
#include "gizwits_product.h"
//#include "common.h"

static uint32_t timerMsCount;
//uint8_t aRxBuffer;

extern uint8_t wendu_value;//定义温度变量
extern uint8_t shidu_value;//定义湿度变量
extern uint32_t smoke_value;//定义烟雾浓度值
extern uint32_t CO2_value;//定义二氧化碳浓度变量
extern uint32_t TVOC_value;//定义TVOC浓度变量
extern float V_value;//定义电压采集变量


/** User area the current device state structure*/
extern dataPoint_t currentDataPoint;
//extern keysTypedef_t keys;

//extern TIM_HandleTypeDef htim2;
//extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart2;

/**@} */
/**@name Gizwits User Interface
* @{
*/

/**
* @brief Event handling interface

* Description:

* 1. Users can customize the changes in WiFi module status

* 2. Users can add data points in the function of event processing logic, such as calling the relevant hardware peripherals operating interface

* @param [in] info: event queue
* @param [in] data: protocol data
* @param [in] len: protocol data length
* @return NULL
* @ref gizwits_protocol.h
*/
int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *gizdata, uint32_t len)
{
  uint8_t i = 0;
  dataPoint_t *dataPointPtr = (dataPoint_t *)gizdata;
  moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)gizdata;
  protocolTime_t *ptime = (protocolTime_t *)gizdata;
  
#if MODULE_TYPE
  gprsInfo_t *gprsInfoData = (gprsInfo_t *)gizdata;
#else
  moduleInfo_t *ptModuleInfo = (moduleInfo_t *)gizdata;
#endif

  if((NULL == info) || (NULL == gizdata))
  {
    return -1;
  }

  for(i=0; i<info->num; i++)
  {
    switch(info->event[i])
    {




      case WIFI_SOFTAP:
        break;
      case WIFI_AIRLINK:
        break;
      case WIFI_STATION:
        break;
      case WIFI_CON_ROUTER:
 
        break;
      case WIFI_DISCON_ROUTER:
 
        break;
      case WIFI_CON_M2M:
 
        break;
      case WIFI_DISCON_M2M:
        break;
      case WIFI_RSSI:
        GIZWITS_LOG("RSSI %d\n", wifiData->rssi);
        break;
      case TRANSPARENT_DATA:
        GIZWITS_LOG("TRANSPARENT_DATA \n");
        //user handle , Fetch data from [data] , size is [len]
        break;
      case WIFI_NTP:
        GIZWITS_LOG("WIFI_NTP : [%d-%d-%d %02d:%02d:%02d][%d] \n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second,ptime->ntp);
        break;
      case MODULE_INFO:
            GIZWITS_LOG("MODULE INFO ...\n");
      #if MODULE_TYPE
            GIZWITS_LOG("GPRS MODULE ...\n");
            //Format By gprsInfo_t
      #else
            GIZWITS_LOG("WIF MODULE ...\n");
            //Format By moduleInfo_t
            GIZWITS_LOG("moduleType : [%d] \n",ptModuleInfo->moduleType);
      #endif
    break;
      default:
        break;
    }
  }

  return 0;
}

/**
* User data acquisition

* Here users need to achieve in addition to data points other than the collection of data collection, can be self-defined acquisition frequency and design data filtering algorithm

* @param none
* @return none
*/
void userHandle(void)
{
 /*
    currentDataPoint.valuewendu_value = ;//Add Sensor Data Collection
    currentDataPoint.valueshidu_value = ;//Add Sensor Data Collection
    currentDataPoint.valueV_value = ;//Add Sensor Data Collection
    currentDataPoint.valuesmoke_value = ;//Add Sensor Data Collection
    currentDataPoint.valueCO2_value = ;//Add Sensor Data Collection
    currentDataPoint.valueTVOC_value = ;//Add Sensor Data Collection

    */
//		currentDataPoint.valuewendu_value = wendu_value;//Add Sensor Data Collection
//    currentDataPoint.valueshidu_value = shidu_value;//Add Sensor Data Collection
//    currentDataPoint.valueV_value = V_value;//Add Sensor Data Collection
//    currentDataPoint.valuesmoke_value = smoke_value;//Add Sensor Data Collection
//    currentDataPoint.valueCO2_value = CO2_value;//Add Sensor Data Collection
//    currentDataPoint.valueTVOC_value = TVOC_value;//Add Sensor Data Collection
    
}

/**
* Data point initialization function

* In the function to complete the initial user-related data
* @param none
* @return none
* @note The developer can add a data point state initialization value within this function
*/
void userInit(void)
{
    memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));
    
    /** Warning !!! DataPoint Variables Init , Must Within The Data Range **/ 
    /*
      currentDataPoint.valuewendu_value = ;
      currentDataPoint.valueshidu_value = ;
      currentDataPoint.valueV_value = ;
      currentDataPoint.valuesmoke_value = ;
      currentDataPoint.valueCO2_value = ;
      currentDataPoint.valueTVOC_value = ;
    */

}


/**
* @brief Millisecond timing maintenance function, milliseconds increment, overflow to zero

* @param none
* @return none
*/
void gizTimerMs(void)
{
    timerMsCount++;
}

/**
* @brief Read millisecond count

* @param none
* @return millisecond count
*/
uint32_t gizGetTimerCount(void)
{
    return timerMsCount;
}

/**
* @brief MCU reset function

* @param none
* @return none
*/
void mcuRestart(void)
{
    __set_FAULTMASK(1);
		NVIC_SystemReset();
}


/**
* @brief TIMER_IRQ_FUN

* Timer Interrupt handler function

* @param none
* @return none
*/
void TIMER_IRQ_FUN(void)
{
  gizTimerMs();
}

/**
* @brief UART_IRQ_FUN

* UART Serial interrupt function ，For Module communication

* Used to receive serial port protocol data between WiFi module

* @param none
* @return none
*/
void UART_IRQ_FUN(void)
{
  uint8_t value = 0;
  //value = USART_ReceiveData(USART2);//STM32 test demo
  gizPutData(&value, 1);
}
///**@} */

//#ifdef __GNUC__
//  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
//     set to 'Yes') calls __io_putchar() */
//  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif /* __GNUC__ */
///**
//  * @brief  Retargets the C library printf function to the USART.
//  * @param  None
//  * @retval None
//  */
//PUTCHAR_PROTOTYPE
//{
//  /* Place your implementation of fputc here */
//  /* e.g. write a character to the USART1 and Loop until the end of transmission */
//  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
// 
//  return ch;
//}

///**
//  * @brief  Period elapsed callback in non blocking mode 
//  * @param  htim : TIM handle
//  * @retval None
//  */
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	if(htim==&htim2)
//	{
//			keyHandle((keysTypedef_t *)&keys);
//			gizTimerMs();
//	}
//}

///**
//* @brief Timer TIM3 init function

//* @param none
//* @return none
//*/
//void timerInit(void)
//{
//	HAL_TIM_Base_Start_IT(&htim2);
//}

///**
//  * @brief  This function handles USART IDLE interrupt.
//  */
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)  
//{  
//    if(UartHandle->Instance == USART2)  
//    {  
//				gizPutData((uint8_t *)&aRxBuffer, 1);

//        HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);//开启下一次接收中断  
//    }  
//}  

///**
//* @brief USART init function

//* Serial communication between WiFi modules and device MCU
//* @param none
//* @return none
//*/
//void uartInit(void)
//{
//	HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);//开启下一次接收中断  
//}


/**
* @brief Serial port write operation, send data to WiFi module
*
* @param buf      : buf address
* @param len      : buf length
*
* @return : Return effective data length;-1，return failure
*/
int32_t uartWrite(uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
	
    if(NULL == buf)
    {
        return -1;
    }

    for(i=0; i<len; i++)
    {
        USART_SendData(USART3, buf[i]);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
        if(i >=2 && buf[i] == 0xFF)
        {
						USART_SendData(USART3,0x55);
						while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
        }
    }

#ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("MCU2WiFi[%4d:%4d]: ", gizGetTimerCount(), len);
    for(i=0; i<len; i++)
    {
        GIZWITS_LOG("%02x ", buf[i]);

        if(i >=2 && buf[i] == 0xFF)
        {
            GIZWITS_LOG("%02x ", 0x55);
        }
    }
    GIZWITS_LOG("\n");
#endif
		
		return len;
}  

#ifndef __DHT11_H
#define __DHT11_H 
#include "stm32f10x.h"

void DHT11_GPIO_Pin_InitCnofig(void);
void DHT11_Output(void);
void DHT11_Input(void);
void DHT11_SendReset(void);
uint8_t DHT11_CheckResponse(void);
uint8_t DHT11_ReadBit(void);
uint8_t DHT11_ReadByte(void);
uint8_t DHT11_ReadData(uint8_t *wendu,uint8_t *shidu);

#endif

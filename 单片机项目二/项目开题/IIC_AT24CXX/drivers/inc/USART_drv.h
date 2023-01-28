#ifndef _USART_DRV_H
#define _USART_DRV_H



#include "stm32f10x.h"
#include <stdio.h>

extern void USART_Myconfig(uint32_t baudrate);
extern void USARTx_Putchar(USART_TypeDef * USARTx,uint8_t ch);
extern void USARTx_Putstr(USART_TypeDef * USARTx,uint8_t *str);
extern int fputc(int ch, FILE *fp);

#endif


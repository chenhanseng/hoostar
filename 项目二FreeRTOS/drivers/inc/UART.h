#ifndef UART_H_
#define UART_H_

#include "stm32f10x.h"
#include <stdio.h>

extern void UART_InitConfig(void);
extern void UART2_InitConfig(void);
extern void UART3_InitConfig(u32 bound);

#endif




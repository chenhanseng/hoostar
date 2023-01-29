#ifndef SYSTICK_H_
#define SYSTICK_H_
#include "stm32f10x.h"
extern void SysTick_InitConfig(uint8_t AHBCLCK);
extern void delay_ms(uint32_t nms);
extern void delay_us(uint32_t nus);

#endif


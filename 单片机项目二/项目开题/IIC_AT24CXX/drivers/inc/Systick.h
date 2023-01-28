#ifndef _SYSTIC_DRV_H__
#define _SYSTIC_DRV_H__

#include "stm32f10x.h"

extern void SysTic_Config(uint16_t SYSCLK);
extern void Delay_Us(uint16_t nums);
extern void Delay_Ms(uint16_t nums);





#endif


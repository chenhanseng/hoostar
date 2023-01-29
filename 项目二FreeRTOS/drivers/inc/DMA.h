#ifndef DMA_H_
#define DMA_H_
#include "stm32f10x.h"

extern void DMA_InitConfig_P2M(void);
extern void DMA_InitConfig_M2P(void);
extern void DMA_InitConfig_M2M(void);
extern uint16_t GetValue_ADC_Average(uint32_t channel);
extern void Get_DST_value(void);

#endif

#ifndef __SGP30_DRV_H
#define __SGP30_DRV_H

#include "stm32f10x.h"

#define SGP30_read  0xb1  //SGP30的读地址
#define SGP30_write 0xb0  //SGP30的写地址


extern void SGP30_Init(void);
extern void SGP30_Write(u8 a, u8 b);
extern u32 SGP30_Read(void);

#endif



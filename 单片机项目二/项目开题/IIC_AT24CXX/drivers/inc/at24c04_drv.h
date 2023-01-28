#ifndef __AT24C04_DRV_H
#define __AT24C04_DRV_H

#include "stm32f10x.h"

extern uint8_t WriteByte_AT24CXX(uint8_t data_addr,uint8_t data);
extern uint8_t ReadByte_AT24CXX(uint8_t dataddr);


#endif


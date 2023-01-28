#ifndef IIC_H_
#define IIC_H_
#include "stm32f10x.h"

extern void IIC_GPIO_Pin_InitCnofig(void);
extern uint8_t EEPROM_Write_Byte(uint8_t addr,uint8_t data);
extern uint8_t EEPROM_Read_Byte(uint8_t addr);
extern void SGP30_InitConfig(void);
extern void SGP30_Write(uint8_t a,uint8_t b);
extern void SGP30_Read(uint16_t * CO2,uint16_t* TVOC);


#endif






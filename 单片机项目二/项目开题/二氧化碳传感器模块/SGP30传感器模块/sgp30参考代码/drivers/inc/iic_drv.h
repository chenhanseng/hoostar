#ifndef __IIC_DRV_H
#define __IIC_DRV_H

#include "stm32f10x.h"

#define SGP30_SCL_GPIO_CLK		RCC_APB2Periph_GPIOA
#define SGP30_SDA_GPIO_SDA		RCC_APB2Periph_GPIOA
#define SGP30_SCL_GPIO_PIN		GPIO_Pin_11
#define SGP30_SDA_GPIO_PIN		GPIO_Pin_12
#define SGP30_SCL_GPIO_PORT		GPIOA
#define SGP30_SDA_GPIO_PORT		GPIOA

#define SDA_HIGH()		GPIO_SetBits(SGP30_SDA_GPIO_PORT, SGP30_SDA_GPIO_PIN)
#define SDA_LOW()		GPIO_ResetBits(SGP30_SDA_GPIO_PORT, SGP30_SDA_GPIO_PIN)
#define SGP30_SDA_READ()		GPIO_ReadInputDataBit(SGP30_SDA_GPIO_PORT, SGP30_SDA_GPIO_PIN)

#define SCL_HIGH()		GPIO_SetBits(SGP30_SCL_GPIO_PORT, SGP30_SCL_GPIO_PIN)
#define SCL_LOW()		GPIO_ResetBits(SGP30_SCL_GPIO_PORT, SGP30_SCL_GPIO_PIN)


extern void SGP30_GPIO_Init(void);
extern void SDA_OUT(void);
extern void SDA_IN(void);
extern void SGP30_IIC_Start(void);
extern void SGP30_IIC_Stop(void);
extern u8 SGP30_IIC_Wait_Ack(void);
extern void SGP30_IIC_Ack(void);
extern void SGP30_IIC_NAck(void);
extern void SGP30_IIC_Send_Byte(u8 txd);
extern u16 SGP30_IIC_Read_Byte(u8 ack);

#endif



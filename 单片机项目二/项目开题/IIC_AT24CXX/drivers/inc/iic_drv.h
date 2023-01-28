#ifndef __IIC_DRV_H
#define __IIC_DRV_H
/*------------------头文件包含-----------------*/
#include "stm32f10x.h"
/*--------------------------------------------*/



/*------------------变量定义-----------------*/
typedef enum ack_flag{
	IIC_ACK = 0,
	IIC_NOACK,
	IIC_MAX,
}en_ack_flag;
/*--------------------------------------------*/

/*------------------宏定义--------------------*/
#define RCC_IIC_PORT	RCC_APB2Periph_GPIOB
#define IIC_SCK_PIN		GPIO_Pin_10
#define IIC_SDA_PIN		GPIO_Pin_11
#define IIC_PORT		GPIOB

#define SDA_HIGH()		GPIO_SetBits(IIC_PORT, IIC_SDA_PIN)
#define SDA_LOW()		GPIO_ResetBits(IIC_PORT, IIC_SDA_PIN)
#define SDA_READ()		GPIO_ReadInputDataBit(IIC_PORT, IIC_SDA_PIN)

#define SCL_HIGH()		GPIO_SetBits(IIC_PORT, IIC_SCK_PIN)
#define SCL_LOW()		GPIO_ResetBits(IIC_PORT, IIC_SCK_PIN)
/*--------------------------------------------*/


/*--------------------函数声明-----------------*/
extern void IIC_Config(void);
extern void sdaIn(void);
extern void sdaOut(void);
extern void IIC_Start(void);
extern void IIC_Stop(void);
extern void IIC_SendAck(void);
extern void IIC_SendNoack(void);
extern en_ack_flag IIC_WaitforACK(void);
extern void IIC_SendByte(uint8_t data);
extern uint8_t IIC_RcvByte(en_ack_flag ack);
/*--------------------------------------------*/




#endif


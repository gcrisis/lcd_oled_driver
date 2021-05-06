#ifndef _IIC_SOFT_H_
#define _IIC_SOFT_H_

#include "main.h"

#define IIC_SOFT_SCL_PIN         IIC_SCL_SOFT_Pin
#define IIC_SOFT_SDA_PIN         IIC_SDA_SOFT_Pin

#define IIC_SOFT_SCL_GPIO        GPIOC
#define IIC_SOFT_SDA_GPIO        GPIOC

#define __IIC_SCL_SET()     LL_GPIO_SetOutputPin(IIC_SOFT_SCL_GPIO, IIC_SOFT_SCL_PIN)
#define __IIC_SCL_CLR()     LL_GPIO_ResetOutputPin(IIC_SOFT_SCL_GPIO, IIC_SOFT_SCL_PIN)

#define __IIC_SDA_SET()	LL_GPIO_SetOutputPin(IIC_SOFT_SDA_GPIO, IIC_SOFT_SDA_PIN)
#define __IIC_SDA_CLR()     LL_GPIO_ResetOutputPin(IIC_SOFT_SDA_GPIO, IIC_SOFT_SDA_PIN)

#define __IIC_SDA_IN()     	do { \
								LL_GPIO_InitTypeDef tGPIO; \
								tGPIO.Pin = IIC_SOFT_SDA_PIN; \
								tGPIO.Mode = LL_GPIO_MODE_INPUT; \
								tGPIO.Pull = LL_GPIO_PULL_UP; \
								tGPIO.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;\
								tGPIO.Speed = LL_GPIO_SPEED_FREQ_HIGH; \
								LL_GPIO_Init(IIC_SOFT_SDA_GPIO, &tGPIO); \
							}while(0)				

#define __IIC_SDA_OUT() 	do { \
								LL_GPIO_InitTypeDef tGPIO; \
								tGPIO.Pin = IIC_SOFT_SDA_PIN; \
								tGPIO.Mode = LL_GPIO_MODE_OUTPUT; \
								tGPIO.Pull = LL_GPIO_PULL_UP; \
								tGPIO.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;\
								tGPIO.Speed = LL_GPIO_SPEED_FREQ_HIGH; \
								LL_GPIO_Init(IIC_SOFT_SDA_GPIO, &tGPIO); \
							}while(0)   



#define __IIC_SDA_READ()    LL_GPIO_IsInputPinSet(IIC_SOFT_SDA_GPIO, IIC_SOFT_SDA_PIN)

extern void iic_init(void);
extern void iic_start(void);
extern void iic_stop(void);
extern int iic_wait_for_ack(void);
extern void iic_write_byte(uint8_t chData);

#endif


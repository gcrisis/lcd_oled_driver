/******************************************************************************
**************************Hardware interface layer*****************************
* | file      		:	DEV_Config.c
* |	version			:	V1.0
* | date			:	2020-06-17
* | function		:	Provide the hardware underlying interface	
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include "stm32f1xx_ll_gpio.h"
#include "main.h"
#include <stdint.h>
#include <stdlib.h>

/*----choose your lcd/oled device----*/

//#define OLED_0IN91
//#define OLED_0IN95_RGB
//#define OLED_0IN96
//#define OLED_1IN3
//#define OLED_1IN3_C
//#define OLED_1IN5
//#define OLED_1IN5_RGB
#define LCD_ILI9341

/*------------------------------------*/

/*----choose your lcd/oled interface----*/

//#define USE_SPI_4W 		
//#define USE_IIC 		
//#define USE_IIC_SOFT	
#define USE_FSMC

/*-------------------------------------*/	
/**
 * data
**/          
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#define IIC_CMD		0X00
#define IIC_RAM		0X40

//interface gpio define
#define SPI1_SCK_Pin LL_GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define SPI1_MISO_Pin LL_GPIO_PIN_6
#define SPI1_MISO_GPIO_Port GPIOA
#define SPI1_MOSI_Pin LL_GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port GPIOA
#define IIC_SCL_SOFT_Pin LL_GPIO_PIN_0
#define IIC_SCL_SOFT_GPIO_Port GPIOC
#define IIC_SDA_SOFT_Pin LL_GPIO_PIN_2
#define IIC_SDA_SOFT_GPIO_Port GPIOC
#define OLED_DC_Pin LL_GPIO_PIN_2
#define OLED_DC_GPIO_Port GPIOA
#define OLED_RST_Pin LL_GPIO_PIN_0
#define OLED_RST_GPIO_Port GPIOA
#define OLED_CS_Pin LL_GPIO_PIN_4
#define OLED_CS_GPIO_Port GPIOA
#define I2C1_SCL_Pin LL_GPIO_PIN_8
#define I2C1_SCL_GPIO_Port GPIOB
#define I2C1_SDA_Pin LL_GPIO_PIN_9
#define I2C1_SDA_GPIO_Port GPIOB
#define LED_PIN LL_GPIO_PIN_14


//OLED GPIO set/reset
#define OLED_CS_0		LL_GPIO_ResetOutputPin(OLED_CS_GPIO_Port, OLED_CS_Pin)
#define OLED_CS_1		LL_GPIO_SetOutputPin(OLED_CS_GPIO_Port, OLED_CS_Pin)

#define OLED_DC_0		LL_GPIO_ResetOutputPin(OLED_DC_GPIO_Port, OLED_DC_Pin)
#define OLED_DC_1		LL_GPIO_SetOutputPin(OLED_DC_GPIO_Port, OLED_DC_Pin)

#define OLED_RST_0		LL_GPIO_ResetOutputPin(OLED_RST_GPIO_Port, OLED_RST_Pin)
#define OLED_RST_1		LL_GPIO_SetOutputPin(OLED_RST_GPIO_Port, OLED_RST_Pin)


/*------------------------------------------------------------------------------------------------------*/

#endif

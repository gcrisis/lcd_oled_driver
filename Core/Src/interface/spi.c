/**
  ******************************************************************************
  * File Name          : SPI.c
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */


/* SPI1 init function */
void MX_SPI1_Init(void)
{
LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA6   ------> SPI1_MISO
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV16;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  
  LL_SPI_Enable(SPI1);
}

/* USER CODE BEGIN 1 */
uint8_t SPI4W_Write_Byte(uint8_t value)
{
#if 0
    HAL_SPI_Transmit(&hspi1, &value, 1, 500);
#elif 0
    char i;
    for(i = 0; i < 8; i++) {
        SPI_SCK_0;
        if(value & 0X80)
            SPI_MOSI_1;
        else
            SPI_MOSI_0;
        Driver_Delay_us(10);
        SPI_SCK_1;
        Driver_Delay_us(10);
        value = (value << 1);
    }
#else
    while(LL_SPI_IsActiveFlag_TXE(SPI1)==0);
    LL_SPI_TransmitData8(SPI1,value);

    while(LL_SPI_IsActiveFlag_BSY(SPI1)==1) ; //Wait for

   // while((SPI1->SR & (1 << 0)) == 0) ; // Wait for the receiving area to be empty

    return 0;
#endif
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

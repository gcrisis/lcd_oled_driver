#include "fsmc.h"

SRAM_HandleTypeDef hsram1;
void MX_FSMC_Init(void)
{

  /* USER CODE BEGIN FSMC_Init 0 */
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOF);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOE);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOG);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_FSMC);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
    ////////////////////fsmc////////
   GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_4|LL_GPIO_PIN_5|
                         LL_GPIO_PIN_8|LL_GPIO_PIN_9|LL_GPIO_PIN_10|LL_GPIO_PIN_14|
                         LL_GPIO_PIN_15;
   GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
   GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
   GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
   LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
   GPIO_InitStruct.Pin = LL_GPIO_PIN_7|LL_GPIO_PIN_8|LL_GPIO_PIN_9|LL_GPIO_PIN_10|
                         LL_GPIO_PIN_11|LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14|
                         LL_GPIO_PIN_15;
   LL_GPIO_Init(GPIOE, &GPIO_InitStruct);
   GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
   LL_GPIO_Init(GPIOF, &GPIO_InitStruct);
   GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
   LL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE END FSMC_Init 0 */

  FSMC_NORSRAM_TimingTypeDef Timing = {0};
  FSMC_NORSRAM_TimingTypeDef ExtTiming = {0};

  /* USER CODE BEGIN FSMC_Init 1 */
  //LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_FSMC);
  /* USER CODE END FSMC_Init 1 */

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK4;
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  /* Timing */
  Timing.AddressSetupTime = 1;
  Timing.AddressHoldTime = 0;
  Timing.DataSetupTime = 4;
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 0;
  Timing.DataLatency = 0;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */
  ExtTiming.AddressSetupTime = 1;
  ExtTiming.AddressHoldTime = 0;
  ExtTiming.DataSetupTime = 4;
  ExtTiming.BusTurnAroundDuration = 0;
  ExtTiming.CLKDivision = 0;
  ExtTiming.DataLatency = 0;
  ExtTiming.AccessMode = FSMC_ACCESS_MODE_A;

  if (HAL_SRAM_Init(&hsram1, &Timing, &ExtTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /** Disconnect NADV
  */

  __HAL_AFIO_FSMCNADV_DISCONNECTED();

  /* USER CODE BEGIN FSMC_Init 2 */

  /* USER CODE END FSMC_Init 2 */
}


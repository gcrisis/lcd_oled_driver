#include "delay.h"

void delay_us(uint32_t micros){
	if(micros>200000)
	{
		micros=200000;
	}
	LL_InitTick(72*micros,1U);
	__IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
  /* Add this code to indicate that local variable is not used */
  	((void)tmp);
	while(1)
	{
		if((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
		{
			break;
		}
	}

}

void delay_ms(uint32_t millis){
	LL_Init1msTick(72000000);
	LL_mDelay(millis);
}
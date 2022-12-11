#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"

static unsigned int fac_us = 0;

void delay_init(void)
{
	fac_us = SystemCoreClock / 1000000;
}


void delay_ms(unsigned int ms)
{
	vTaskDelay(ms / portTICK_PERIOD_MS);			
}


void delay_us(unsigned int us)
{
	unsigned int ticks = us * fac_us;
	unsigned int reload = SysTick->LOAD;
	unsigned int told, tnow, tcnt = 0;

	told = SysTick->VAL;
	while(tcnt < ticks)
	{
		tnow = SysTick->VAL;
		if(tnow != told)
		{
			if(tnow < told) tcnt += (told-tnow);
			else tcnt += (reload-tnow+told);
			told = tnow;
		}
	}	
}


// Return system uptime in 1milliseconds (rollover in 49 days)
unsigned int sys_time_ms(void)
{
	return (xTaskGetTickCount() / portTICK_PERIOD_MS);
}


unsigned int sys_time_elapsed(unsigned int last)
{
	return TIMER_DIFF(sys_time_ms(), last);
}


//------------------End of File----------------------------

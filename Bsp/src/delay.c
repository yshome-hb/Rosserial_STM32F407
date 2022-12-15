#include "delay.h"

static uint32_t fac_us = 0;

/**
 * @brief     初始化延迟函数
 * @param     无
 * @retval    无
 */  
void delay_init(void)
{
	fac_us = SystemCoreClock / 1000000;
}

/**
 * @brief     延时us
 * @param     us: 要延时的us数
 * @note      us取值范围: 0~8947848(最大值即2^32 / fac_us @fac_us = 168)
 * @retval    无
 */ 
void delay_us(uint32_t us)
{
    uint32_t ticks = us * fac_us;
    uint32_t reload = SysTick->LOAD;
    uint32_t told, tnow, tcnt = 0;

    told = SysTick->VAL;
    while(tcnt < ticks){
        tnow = SysTick->VAL;
        if(tnow != told){
            if(tnow < told) 
                tcnt += (told - tnow);
            else 
                tcnt += (reload - tnow + told);
            told = tnow;
        }
    }
}


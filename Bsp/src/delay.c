#include "delay.h"

static uint32_t fac_us = 0;

/**
 * @brief     ��ʼ���ӳٺ���
 * @param     ��
 * @retval    ��
 */  
void delay_init(void)
{
	fac_us = SystemCoreClock / 1000000;
}

/**
 * @brief     ��ʱus
 * @param     us: Ҫ��ʱ��us��
 * @note      usȡֵ��Χ: 0~8947848(���ֵ��2^32 / fac_us @fac_us = 168)
 * @retval    ��
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


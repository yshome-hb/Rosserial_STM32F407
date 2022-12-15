#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "stm32f4xx.h"

void WFI_SET(void);
void INTX_DISABLE(void);
void INTX_ENABLE(void);
void MSR_MSP(uint32_t addr);

#define TIMER_DIFF(a, b)     ((uint32_t)((a)-(b)))

#define system_time_get() HAL_GetTick()

static inline uint32_t system_time_elapsed(uint32_t last)
{
    return TIMER_DIFF(system_time_get(), last);
}

uint8_t system_clock_init(uint32_t clock);

#endif


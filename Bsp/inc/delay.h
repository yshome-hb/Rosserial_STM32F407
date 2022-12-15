#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f4xx.h"

#define delay_ms(ms)    vTaskDelay(ms)

void delay_init(void);
void delay_us(uint32_t us);

#endif


#ifndef __DELAY_H__
#define __DELAY_H__ 

#include "stm32f4xx.h" 

#define TIMER_DIFF(a, b)     ((uint32_t)((a)-(b)))

extern void delay_init(void);
extern void delay_ms(unsigned int ms);
extern void delay_us(unsigned int us);

extern unsigned int sys_time_ms(void);
extern unsigned int sys_time_elapsed(unsigned int last);

#endif

//------------------End of File----------------------------

#ifndef __DELAY_H__
#define __DELAY_H__ 

#include "stm32f4xx.h" 

extern void delay_init(void);
extern unsigned int millis(void);
extern void delay_ms(unsigned int nms);
extern void delay_us(unsigned int nus);


#endif

//------------------End of File----------------------------

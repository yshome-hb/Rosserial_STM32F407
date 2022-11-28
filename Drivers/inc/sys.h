#ifndef __SYS_H__
#define __SYS_H__

#include "stm32f4xx.h" 

void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(uint32_t addr);//设置堆栈地址 

#endif

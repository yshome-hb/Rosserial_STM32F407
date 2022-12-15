#ifndef __IO_H__
#define __IO_H__

#include "stm32f4xx.h" 

typedef enum
{
    OUTPUT_LED_1,
    OUTPUT_BUZZER,
	OUTPUT_MAX,

}io_output_num;

typedef struct
{
	GPIO_TypeDef* port; 
	uint16_t pin;
	uint32_t mode;
	uint8_t level;
	
}io_output_t;

extern void io_output_init(void);
extern void io_output_set(uint8_t id, uint8_t level);
extern void io_output_toggle(uint8_t id);
extern uint8_t io_output_get(uint8_t id);

#endif

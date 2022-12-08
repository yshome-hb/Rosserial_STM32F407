#include "stm32f4xx.h" 
#include "io.h"

#define OUTPUT_LED_1_GPIO			GPIOA
#define OUTPUT_LED_1_PIN			GPIO_Pin_12

typedef struct
{
	GPIO_TypeDef* gpio; 
	uint16_t pin;
	GPIOOType_TypeDef type;
	uint8_t level;
	
}io_output_t;


static io_output_t io_output[OUTPUT_MAX] = {0};

void io_output_init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	

	io_output[OUTPUT_LED_1].gpio = OUTPUT_LED_1_GPIO;
	io_output[OUTPUT_LED_1].pin = OUTPUT_LED_1_PIN;
	io_output[OUTPUT_LED_1].type = GPIO_OType_PP;	
	io_output[OUTPUT_LED_1].level = 1;		

	for(uint16_t i = 0; i < OUTPUT_MAX; i++)
	{
		GPIO_InitStructure.GPIO_Pin = io_output[i].pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = io_output[i].type;
		GPIO_Init(io_output[i].gpio, &GPIO_InitStructure);
		GPIO_WriteBit(io_output[i].gpio, io_output[i].pin, (BitAction)io_output[i].level);
	}	
}


void io_output_set(uint8_t id, uint8_t level)
{
 	if(id >= OUTPUT_MAX) return;

	io_output[id].level = (level > 0);
	GPIO_WriteBit(io_output[id].gpio, io_output[id].pin, (BitAction)io_output[id].level);
}


void io_output_toggle(uint8_t id)
{
 	if(id >= OUTPUT_MAX) return;

	io_output[id].level = !io_output[id].level;
	GPIO_WriteBit(io_output[id].gpio, io_output[id].pin, (BitAction)io_output[id].level);
}


uint8_t io_output_get(uint8_t id)
{
 	if(id >= OUTPUT_MAX) return 0;

	return io_output[id].level;
}

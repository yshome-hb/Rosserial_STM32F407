
#include "io.h"

#define OUTPUT_GPIO_CLK_ENABLE() {	\
	do{								\
		__GPIOA_CLK_ENABLE(); 		\
	}while(0);						\
}

#define OUTPUT_LED_1_GPIO		GPIOA
#define OUTPUT_LED_1_PIN		GPIO_PIN_12

#define OUTPUT_BUZZER_GPIO		GPIOA
#define OUTPUT_BUZZER_PIN		GPIO_PIN_8

static io_output_t io_output[OUTPUT_MAX] = {0};

void io_output_init(void)
{
	io_output[OUTPUT_LED_1].port = OUTPUT_LED_1_GPIO;
	io_output[OUTPUT_LED_1].pin = OUTPUT_LED_1_PIN;
	io_output[OUTPUT_LED_1].mode = GPIO_MODE_OUTPUT_PP;
	io_output[OUTPUT_LED_1].level = 1;

	io_output[OUTPUT_BUZZER].port = OUTPUT_BUZZER_GPIO;
	io_output[OUTPUT_BUZZER].pin = OUTPUT_BUZZER_PIN;
	io_output[OUTPUT_BUZZER].mode = GPIO_MODE_OUTPUT_PP;
	io_output[OUTPUT_BUZZER].level = 1;

	OUTPUT_GPIO_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	for(int i = 0; i < OUTPUT_MAX; i++){
		GPIO_InitStruct.Pin = io_output[i].pin;
		GPIO_InitStruct.Mode = io_output[i].mode;
		HAL_GPIO_Init(io_output[i].port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(io_output[i].port, io_output[i].pin, (GPIO_PinState)io_output[i].level);
	}	
}

void io_output_set(uint8_t id, uint8_t level)
{
 	if(id >= OUTPUT_MAX) return;
	io_output[id].level = (level > 0);
	HAL_GPIO_WritePin(io_output[id].port, io_output[id].pin, (GPIO_PinState)io_output[id].level);
}


void io_output_toggle(uint8_t id)
{
 	if(id >= OUTPUT_MAX) return;
	io_output[id].level = !io_output[id].level;
	HAL_GPIO_WritePin(io_output[id].port, io_output[id].pin, (GPIO_PinState)io_output[id].level);
}


uint8_t io_output_get(uint8_t id)
{
 	if(id >= OUTPUT_MAX) return 0;
	return io_output[id].level;
}

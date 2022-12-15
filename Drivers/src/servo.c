#include "servo.h"

/**************** SERVO defines ****************/
#define SERVO_TIM_DEV			TIM14
#define SERVO_TIM_CLK_ENABLE	__TIM14_CLK_ENABLE
#define SERVO_TIM_IRQn			TIM8_TRG_COM_TIM14_IRQn
#define SERVO_IRQ_Handler		TIM8_TRG_COM_TIM14_IRQHandler

#define SERVO_TIM_PERIOD		2550 // us
#define SERVO_TIM_PRESCALER		( SystemCoreClock / (2 * 1000000) - 1)

#define SERVO_GPIO_CLK_ENABLE() {	\
	do{								\
		__GPIOB_CLK_ENABLE(); 		\
		__GPIOC_CLK_ENABLE(); 		\
	}while(0);						\
}

#define SERVO_0_GPIO			GPIOB
#define SERVO_0_PIN				GPIO_PIN_15

#define SERVO_1_GPIO			GPIOB
#define SERVO_1_PIN				GPIO_PIN_14

#define SERVO_2_GPIO			GPIOC
#define SERVO_2_PIN				GPIO_PIN_9

#define SERVO_3_GPIO			GPIOC
#define SERVO_3_PIN				GPIO_PIN_8

#define SERVO_4_GPIO			GPIOC
#define SERVO_4_PIN				GPIO_PIN_7

#define SERVO_5_GPIO			GPIOC
#define SERVO_5_PIN				GPIO_PIN_6

#define SERVO_6_GPIO			GPIOC
#define SERVO_6_PIN				GPIO_PIN_6

#define SERVO_7_GPIO			GPIOC
#define SERVO_7_PIN				GPIO_PIN_6

static TIM_HandleTypeDef servo_tim_handle;
static servo_handle_t servos[MAX_SERVOS];
static uint8_t servo_count = 0;

/*IRQ Handler*/
void SERVO_IRQ_Handler(void)
{
	HAL_TIM_IRQHandler(&servo_tim_handle);
}

void SERVO_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(servos[servo_count].enable){
		HAL_GPIO_WritePin(servos[servo_count].port, servos[servo_count].pin, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&servo_tim_handle, TIM_CHANNEL_1, servos[servo_count].ticks);
	}else{
		__HAL_TIM_SET_COMPARE(&servo_tim_handle, TIM_CHANNEL_1, SERVO_PULSE_DEFAULT);
	}
}

void SERVO_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(servos[servo_count].port, servos[servo_count].pin, GPIO_PIN_RESET);
	servo_count++;
	servo_count &= (MAX_SERVOS - 1);
}

void servo_init(void)
{
	TIM_OC_InitTypeDef TIM_OC_InitStruct;
	
	SERVO_TIM_CLK_ENABLE();

    servo_tim_handle.Instance = SERVO_TIM_DEV;
    servo_tim_handle.Init.Prescaler = SERVO_TIM_PRESCALER;
    servo_tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    servo_tim_handle.Init.Period = SERVO_TIM_PERIOD;
    servo_tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    servo_tim_handle.Init.RepetitionCounter = 0;
    servo_tim_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_OC_Init(&servo_tim_handle);

    TIM_OC_InitStruct.OCMode = TIM_OCMODE_TOGGLE;
    TIM_OC_InitStruct.Pulse = SERVO_PULSE_DEFAULT;
    TIM_OC_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OC_InitStruct.OCIdleState = TIM_OCIDLESTATE_RESET;
    HAL_TIM_OC_ConfigChannel(&servo_tim_handle, &TIM_OC_InitStruct, TIM_CHANNEL_1);
    __HAL_TIM_DISABLE_OCxPRELOAD(&servo_tim_handle, TIM_CHANNEL_1);

	HAL_TIM_RegisterCallback(&servo_tim_handle, HAL_TIM_PERIOD_ELAPSED_CB_ID, SERVO_TIM_PeriodElapsedCallback);
	HAL_TIM_RegisterCallback(&servo_tim_handle, HAL_TIM_OC_DELAY_ELAPSED_CB_ID, SERVO_TIM_OC_DelayElapsedCallback);

    HAL_NVIC_SetPriority(SERVO_TIM_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(SERVO_TIM_IRQn);

	HAL_TIM_OC_Start_IT(&servo_tim_handle, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&servo_tim_handle);

	servos[0].port = SERVO_0_GPIO;
	servos[0].pin = SERVO_0_PIN;

	servos[1].port = SERVO_1_GPIO;
	servos[1].pin = SERVO_1_PIN;

	servos[2].port = SERVO_2_GPIO;
	servos[2].pin = SERVO_2_PIN;

	servos[3].port = SERVO_3_GPIO;
	servos[3].pin = SERVO_3_PIN;

	servos[4].port = SERVO_4_GPIO;
	servos[4].pin = SERVO_4_PIN;

	servos[5].port = SERVO_5_GPIO;
	servos[5].pin = SERVO_5_PIN;

	servos[6].port = SERVO_6_GPIO;
	servos[6].pin = SERVO_6_PIN;

	servos[7].port = SERVO_7_GPIO;
	servos[7].pin = SERVO_7_PIN;

	SERVO_GPIO_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	for(int i = 0; i < MAX_SERVOS; i++){
		servos[i].enable = false;
		servos[i].ticks = SERVO_PULSE_DEFAULT;
		GPIO_InitStruct.Pin = servos[i].pin;
		HAL_GPIO_Init(servos[i].port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(servos[i].port, servos[i].pin, GPIO_PIN_RESET);
	}
}

void servo_attach(uint8_t id, uint16_t ticks)
{
	if(id >= MAX_SERVOS) return;
	servos[id].enable = true;
	if(ticks > SERVO_PULSE_MAX)
		servos[id].ticks = SERVO_PULSE_MAX;
	else if(ticks < SERVO_PULSE_MIN)
		servos[id].ticks = SERVO_PULSE_MIN;
	else
		servos[id].ticks = ticks;
}

void servo_detach(uint8_t id)
{
	if(id >= MAX_SERVOS) return;
	servos[id].enable = false;
	// HAL_GPIO_WritePin(servos[id].port, servos[id].pin, GPIO_PIN_RESET);
}


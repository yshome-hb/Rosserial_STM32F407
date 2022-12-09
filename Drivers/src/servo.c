
#include "servo.h"

/**************** SERVO defines ****************/
#define SERVO_TIM_DEV			TIM14
#define SERVO_TIM_RCC			RCC_APB1Periph_TIM14
#define SERVO_TIM_IRQn			TIM8_TRG_COM_TIM14_IRQn
#define SERVO_IRQ_Handler		TIM8_TRG_COM_TIM14_IRQHandler

#define SERVO_TIM_PERIOD		2550 // us
#define SERVO_TIM_PRESCALER		( SystemCoreClock / (2 * 1000000) - 1)

#define SERVO_GPIO_RCC			( RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC )

#define SERVO_0_GPIO			GPIOB
#define SERVO_0_PIN				GPIO_Pin_15

#define SERVO_1_GPIO			GPIOB
#define SERVO_1_PIN				GPIO_Pin_14

#define SERVO_2_GPIO			GPIOC
#define SERVO_2_PIN				GPIO_Pin_9

#define SERVO_3_GPIO			GPIOC
#define SERVO_3_PIN				GPIO_Pin_8

#define SERVO_4_GPIO			GPIOC
#define SERVO_4_PIN				GPIO_Pin_7

#define SERVO_5_GPIO			GPIOC
#define SERVO_5_PIN				GPIO_Pin_6

#define SERVO_6_GPIO			GPIOC
#define SERVO_6_PIN				GPIO_Pin_6

#define SERVO_7_GPIO			GPIOC
#define SERVO_7_PIN				GPIO_Pin_6

static servo_handle_t servos[MAX_SERVOS];
static uint8_t servo_count = 0;

/*IRQ Handler*/
void SERVO_IRQ_Handler(void)
{
	if(TIM_GetITStatus(SERVO_TIM_DEV, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(SERVO_TIM_DEV, TIM_IT_Update);
		if(servos[servo_count].enable){
			GPIO_SetBits(servos[servo_count].gpio, servos[servo_count].pin);
			TIM_SetCompare1(SERVO_TIM_DEV, servos[servo_count].ticks);
		}else{
			TIM_SetCompare1(SERVO_TIM_DEV, SERVO_PULSE_DEFAULT);
		}
	}
	else if(TIM_GetITStatus(SERVO_TIM_DEV, TIM_IT_CC1) != RESET){
		TIM_ClearITPendingBit(SERVO_TIM_DEV, TIM_IT_CC1);
		// if(servos[servo_count].enable)
			GPIO_ResetBits(servos[servo_count].gpio, servos[servo_count].pin);
		servo_count++;
		servo_count &= (MAX_SERVOS - 1);
	}
}

void servo_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(SERVO_TIM_RCC, ENABLE);

	TIM_TimeBaseInitStructure.TIM_Period = SERVO_TIM_PERIOD;
	TIM_TimeBaseInitStructure.TIM_Prescaler = SERVO_TIM_PRESCALER; 
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(SERVO_TIM_DEV, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse = SERVO_PULSE_DEFAULT;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(SERVO_TIM_DEV, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(SERVO_TIM_DEV, TIM_OCPreload_Disable);
	TIM_CtrlPWMOutputs(SERVO_TIM_DEV, DISABLE); 

	NVIC_InitStructure.NVIC_IRQChannel = SERVO_TIM_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ClearITPendingBit(SERVO_TIM_DEV, TIM_IT_Update | TIM_IT_CC1);
	TIM_ITConfig(SERVO_TIM_DEV, TIM_IT_Update | TIM_IT_CC1, ENABLE);

	TIM_Cmd(SERVO_TIM_DEV, ENABLE);

	servos[0].gpio = SERVO_0_GPIO;
	servos[0].pin = SERVO_0_PIN;

	servos[1].gpio = SERVO_1_GPIO;
	servos[1].pin = SERVO_1_PIN;

	servos[2].gpio = SERVO_2_GPIO;
	servos[2].pin = SERVO_2_PIN;

	servos[3].gpio = SERVO_3_GPIO;
	servos[3].pin = SERVO_3_PIN;

	servos[4].gpio = SERVO_4_GPIO;
	servos[4].pin = SERVO_4_PIN;

	servos[5].gpio = SERVO_5_GPIO;
	servos[5].pin = SERVO_5_PIN;

	servos[6].gpio = SERVO_6_GPIO;
	servos[6].pin = SERVO_6_PIN;

	servos[7].gpio = SERVO_7_GPIO;
	servos[7].pin = SERVO_7_PIN;

	RCC_AHB1PeriphClockCmd(SERVO_GPIO_RCC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	for(int i = 0; i < MAX_SERVOS; i++){
		servos[i].enable = false;
		servos[i].ticks = SERVO_PULSE_DEFAULT;
		GPIO_InitStructure.GPIO_Pin = servos[i].pin;
		GPIO_Init(servos[i].gpio, &GPIO_InitStructure);
		GPIO_ResetBits(servos[i].gpio, servos[i].pin);
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
	// GPIO_ResetBits(servos[id].gpio, servos[id].pin);
}


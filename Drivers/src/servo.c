
#include "servo.h"

/**************** SERVO defines ****************/
#define SERVO_TIM_DEV                  	TIM14
#define SERVO_TIM_RCC                  	RCC_APB1Periph_TIM14
#define SERVO_TIM_IRQn                 	TIM8_TRG_COM_TIM14_IRQn
#define SERVO_IRQ_Handler               TIM8_TRG_COM_TIM14_IRQHandler

/*IRQ Handler*/
void SERVO_IRQ_Handler(void)
{
	if(TIM_GetITStatus(SERVO_TIM_DEV, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(SERVO_TIM_DEV, TIM_IT_Update);
	}
	else if(TIM_GetITStatus(SERVO_TIM_DEV, TIM_IT_CC1) != RESET){
		TIM_ClearITPendingBit(SERVO_TIM_DEV, TIM_IT_CC1);
	}
}

void servo_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(SERVO_TIM_RCC, ENABLE);

	TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 168;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(SERVO_TIM_DEV, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 0x4000;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(SERVO_TIM_DEV, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(SERVO_TIM_DEV, TIM_OCPreload_Enable);
	TIM_CtrlPWMOutputs(SERVO_TIM_DEV, DISABLE); 

	NVIC_InitStructure.NVIC_IRQChannel = SERVO_TIM_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ClearITPendingBit(SERVO_TIM_DEV, TIM_IT_Update | TIM_IT_CC1);
	TIM_ITConfig(SERVO_TIM_DEV, TIM_IT_Update | TIM_IT_CC1, ENABLE);

	TIM_Cmd(SERVO_TIM_DEV, ENABLE);
}




#include "stm32f4xx.h" 
#include "debug.h"

/**************** DEBUG defines ****************/
#define DEBUG_UART_DEV				USART1
#define DEBUG_UART_RCC				RCC_APB2Periph_USART1
#define DEBUG_UART_IRQn				USART1_IRQn
#define DEBUG_IRQ_Handler			USART1_IRQHandler

/*Tx*/
#define DEBUG_TX_GPIO_RCC           RCC_AHB1Periph_GPIOA
#define DEBUG_TX_GPIO               GPIOA
#define DEBUG_TX_PIN                GPIO_Pin_9
#define DEBUG_TX_PIN_SRC			GPIO_PinSource9
#define DEBUG_TX_AF					GPIO_AF_USART1

/*Rx*/
#define DEBUG_RX_GPIO_RCC           RCC_AHB1Periph_GPIOA
#define DEBUG_RX_GPIO               GPIOA
#define DEBUG_RX_PIN                GPIO_Pin_10
#define DEBUG_RX_PIN_SRC			GPIO_PinSource10
#define DEBUG_RX_AF					GPIO_AF_USART1

log_level_t g_log_level = LOG_LEVEL_INFO;

/* Suport printf function, useMicroLib is unnecessary */
#ifdef __CC_ARM
	#pragma import(__use_no_semihosting)
	struct __FILE
	{
		int handle;
	};
  
	FILE __stdout;
  
	void _sys_exit(int x)
	{
		x = x;
	}
#endif

#ifdef __GNUC__
	/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
 
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{  
	USART_SendData(DEBUG_UART_DEV, ch);
	while ( USART_GetFlagStatus(DEBUG_UART_DEV, USART_FLAG_TC) == RESET );    
	return ch;
}

/**
  * @brief  initialize DEBUG   
  * @param  bound: UART BaudRate
  * @retval None
  */
void debug_printf_init(unsigned int bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(DEBUG_TX_GPIO_RCC | DEBUG_RX_GPIO_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(DEBUG_UART_RCC, ENABLE);

	GPIO_PinAFConfig(DEBUG_TX_GPIO, DEBUG_TX_PIN_SRC, DEBUG_TX_AF);
	GPIO_PinAFConfig(DEBUG_RX_GPIO, DEBUG_RX_PIN_SRC, DEBUG_RX_AF);

	/* Configure the UART1 TX pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = DEBUG_TX_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(DEBUG_TX_GPIO, &GPIO_InitStructure);

	/* Configure the UART1 RX pin */
	GPIO_InitStructure.GPIO_Pin = DEBUG_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(DEBUG_RX_GPIO, &GPIO_InitStructure);

	/*Configure UART param*/
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(DEBUG_UART_DEV, &USART_InitStructure);

	USART_Cmd(DEBUG_UART_DEV, ENABLE); 
	USART_ClearFlag(DEBUG_UART_DEV, USART_FLAG_TC);  
}


void debug_printf_begin(void)
{

#if 0	/* add log time header here */
    long current = time(NULL);
	struct tm* ptm = localtime(&current);

	struct timeval tv;
	gettimeofday(&tv, NULL);

	printf("%02d:%02d:%02d.%03d ", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, (int)tv.tv_usec / 1000);
#endif
}


void debug_printf_end(void)
{
	printf("\r\n");
}


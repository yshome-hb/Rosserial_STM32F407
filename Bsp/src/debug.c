#include "stm32f4xx.h" 
#include "debug.h"

/**************** DEBUG defines ****************/
#define DEBUG_UART_DEV				USART1
#define DEBUG_UART_CLK_ENABLE		__USART1_CLK_ENABLE
#define DEBUG_UART_IRQn				USART1_IRQn
#define DEBUG_IRQ_Handler			USART1_IRQHandler

/*Tx*/
#define DEBUG_TX_GPIO_CLK_ENABLE    __GPIOA_CLK_ENABLE
#define DEBUG_TX_PORT               GPIOA
#define DEBUG_TX_PIN                GPIO_PIN_9
#define DEBUG_TX_AF					GPIO_AF7_USART1

/*Rx*/
#define DEBUG_RX_GPIO_CLK_ENABLE    __GPIOA_CLK_ENABLE
#define DEBUG_RX_PORT               GPIOA
#define DEBUG_RX_PIN                GPIO_PIN_10
#define DEBUG_RX_AF					GPIO_AF7_USART1

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
	DEBUG_UART_DEV->DR = ch;
	while((DEBUG_UART_DEV->SR & USART_FLAG_TC) == RESET);    
	return ch;
}

/**
  * @brief  initialize DEBUG   
  * @param  bound: UART BaudRate
  * @retval None
  */
void debug_printf_init(unsigned int baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
	UART_HandleTypeDef UART_Handle;

    DEBUG_UART_CLK_ENABLE();
    DEBUG_TX_GPIO_CLK_ENABLE();
    DEBUG_RX_GPIO_CLK_ENABLE();

	GPIO_InitStruct.Pin = DEBUG_TX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = DEBUG_TX_AF;
	HAL_GPIO_Init(DEBUG_TX_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DEBUG_RX_PIN;
    GPIO_InitStruct.Alternate = DEBUG_RX_AF;
    HAL_GPIO_Init(DEBUG_RX_PORT, &GPIO_InitStruct);

    UART_Handle.Instance = DEBUG_UART_DEV;
    UART_Handle.Init.BaudRate = baudrate;
    UART_Handle.Init.WordLength = UART_WORDLENGTH_8B;
    UART_Handle.Init.StopBits = UART_STOPBITS_1;
    UART_Handle.Init.Parity = UART_PARITY_NONE;
    UART_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UART_Handle.Init.Mode = UART_MODE_TX;
    HAL_UART_Init(&UART_Handle);
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


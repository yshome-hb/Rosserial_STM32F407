#include "stm32f4xx.h" 
#include "usart.h"	

/**************** PORT1 defines ****************/
#define PORT1_UART_DEV                  USART3
#define PORT1_UART_RCC                  RCC_APB1Periph_USART3
#define PORT1_UART_IRQn                 USART3_IRQn
#define PORT1_IRQ_Handler               USART3_IRQHandler

#define PORT1_DMA_RCC					RCC_AHBPERIPH_DMA1
#define PORT1_DMA_TXCH					DMA1_Channel4
#define PORT1_DMA_RXCH					DMA1_Channel5

/*Tx*/
#define PORT1_TX_GPIO_RCC           	RCC_AHB1Periph_GPIOD
#define PORT1_TX_GPIO               	GPIOD
#define PORT1_TX_PIN                	GPIO_Pins_8
#define PORT1_TX_PIN_SRC				GPIO_PinSource8
#define PORT1_TX_AF						GPIO_AF_USART3 

/*Rx*/
#define PORT1_RX_GPIO_RCC           	RCC_AHB1Periph_GPIOD
#define PORT1_RX_GPIO               	GPIOD
#define PORT1_RX_PIN                	GPIO_Pins_9
#define PORT1_RX_PIN_SRC				GPIO_PinSource9
#define PORT1_RX_AF						GPIO_AF_USART3


void uart_port1_init(unsigned int bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(PORT1_TX_GPIO_RCC | PORT1_RX_GPIO_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(PORT1_UART_RCC, ENABLE);

	GPIO_PinAFConfig(PORT1_TX_GPIO, PORT1_TX_PIN_SRC, PORT1_TX_AF);
	GPIO_PinAFConfig(PORT1_RX_GPIO, PORT1_RX_PIN_SRC, PORT1_RX_AF);

  	/* Configure the PORT1 TX pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = PORT1_TX_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(PORT1_TX_GPIO, &GPIO_InitStructure);

  	/* Configure the PORT1 RX pin */
	GPIO_InitStructure.GPIO_Pin = PORT1_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(PORT1_RX_GPIO, &GPIO_InitStructure);

	/*Configure UART param*/
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(PORT1_UART_DEV, &USART_InitStructure);

	USART_Cmd(PORT1_UART_DEV, ENABLE); 
	USART_ClearFlag(PORT1_UART_DEV, USART_FLAG_TC);
}


// void uart1_setdma(uint8_t *tx_addr, uint16_t tx_size, uint8_t *rx_addr, uint16_t rx_size)
// {
// 	DMA_InitType DMA_InitStructure;

// 	if((!tx_addr) && (!tx_size) && (!rx_addr) && (!rx_size))
// 		return;

// 	RCC_AHBPeriphClockCmd(PORT1_DMA_RCC, ENABLE);	

// 	if(tx_addr != NULL && tx_size > 0 ){

// 		DMA_Reset(PORT1_DMA_TXCH);
// 		DMA_DefaultInitParaConfig(&DMA_InitStructure);
// 		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(PORT1_UART_DEV->DT);
// 		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)tx_addr;
// 		DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;
// 		DMA_InitStructure.DMA_BufferSize = tx_size;
// 		DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
// 		DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
// 		DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
// 		DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
// 		DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
// 		DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;
// 		DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
// 		DMA_Init(PORT1_DMA_TXCH, &DMA_InitStructure);
		
// 		USART_DMACmd( PORT1_UART_DEV, USART_DMAReq_Tx, ENABLE);
// 	}

// 	if(rx_addr != NULL && rx_size > 0 ){

// 		DMA_Reset(PORT1_DMA_RXCH);
// 		DMA_DefaultInitParaConfig(&DMA_InitStructure);
// 		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(PORT1_UART_DEV->DT);
// 		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rx_addr;
// 		DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
// 		DMA_InitStructure.DMA_BufferSize = rx_size;
// 		DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
// 		DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
// 		DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
// 		DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
// 		DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
// 		DMA_InitStructure.DMA_Priority = DMA_PRIORITY_MEDIUM;
// 		DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
// 		DMA_Init(PORT1_DMA_RXCH, &DMA_InitStructure);
		
// 		USART_DMACmd( PORT1_UART_DEV, USART_DMAReq_Rx, ENABLE);
// 	}
// }


void uart_port1_deinit(void)
{
	USART_INTConfig(PORT1_UART_DEV, USART_INT_RDNE, DISABLE);
	USART_Cmd(PORT1_UART_DEV, DISABLE);
	USART_Reset(PORT1_UART_DEV);
}


void uart_port1_send(unsigned char c)
{
	while(USART_GetFlagStatus(PORT1_UART_DEV, USART_FLAG_TRAC) == RESET);
	USART_SendData(PORT1_UART_DEV, c);
}

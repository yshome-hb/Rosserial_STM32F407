#include "stm32f4xx.h" 
#include "usart.h"	

/**************** PORT1 defines ****************/
#define PORT1_UART_DEV                  USART3
#define PORT1_UART_RCC                  RCC_APB1Periph_USART3
#define PORT1_DMA_RCC					RCC_AHB1Periph_DMA1
#define PORT1_UART_IRQn                 USART3_IRQn
#define PORT1_IRQ_Handler               USART3_IRQHandler

/*Tx*/
#define PORT1_TX_GPIO_RCC				RCC_AHB1Periph_GPIOD
#define PORT1_TX_GPIO					GPIOD
#define PORT1_TX_PIN					GPIO_Pin_8
#define PORT1_TX_PIN_SRC				GPIO_PinSource8
#define PORT1_TX_AF						GPIO_AF_USART3
#define PORT1_TX_DMA_STREAM				DMA1_Stream4
#define PORT1_TX_DMA_CHN				DMA_Channel_7
#define PORT1_TX_DMA_IRQn               DMA1_Stream4_IRQn
#define PORT1_TX_DMA_FLAG				DMA_FLAG_TCIF4
#define PORT1_TX_DMA_IRQ_Handler        DMA1_Stream4_IRQHandler

/*Rx*/
#define PORT1_RX_GPIO_RCC				RCC_AHB1Periph_GPIOD
#define PORT1_RX_GPIO					GPIOD
#define PORT1_RX_PIN					GPIO_Pin_9
#define PORT1_RX_PIN_SRC				GPIO_PinSource9
#define PORT1_RX_AF						GPIO_AF_USART3
#define PORT1_RX_DMA_STREAM				DMA1_Stream1
#define PORT1_RX_DMA_CHN				DMA_Channel_4
#define PORT1_RX_DMA_IRQn               DMA1_Stream1_IRQn
#define PORT1_RX_DMA_FLAG				DMA_FLAG_TCIF1
#define PORT1_RX_DMA_IRQ_Handler        DMA1_Stream1_IRQHandler

static uart_port_handle_t uart_port1;

void PORT1_IRQ_Handler(void)
{
	if(USART_GetITStatus(PORT1_UART_DEV, USART_IT_IDLE) != RESET){
		USART_ReceiveData(PORT1_UART_DEV);
		DMA_Cmd(PORT1_RX_DMA_STREAM, DISABLE);
		DMA_ClearFlag(PORT1_RX_DMA_STREAM, PORT1_RX_DMA_FLAG);
		if(uart_port1.recv_cb)
			uart_port1.recv_cb(uart_port1.dma_rx_addr, uart_port1.dma_rx_len - DMA_GetCurrDataCounter(PORT1_RX_DMA_STREAM));
		DMA_SetCurrDataCounter(PORT1_RX_DMA_STREAM, uart_port1.dma_rx_len);
		DMA_Cmd(PORT1_RX_DMA_STREAM, ENABLE);
	}
}

void PORT1_TX_DMA_IRQ_Handler(void)
{
	if(DMA_GetFlagStatus(PORT1_TX_DMA_STREAM, PORT1_TX_DMA_FLAG) != RESET){
		DMA_ClearFlag(PORT1_TX_DMA_STREAM, PORT1_TX_DMA_FLAG);
		DMA_Cmd(PORT1_TX_DMA_STREAM, DISABLE);
		if(uart_port1.send_cb)
			uart_port1.send_cb();
	}
}

void PORT1_RX_DMA_IRQ_Handler(void)
{
	if(DMA_GetFlagStatus(PORT1_RX_DMA_STREAM, PORT1_RX_DMA_FLAG) != RESET){
		DMA_ClearFlag(PORT1_RX_DMA_STREAM, PORT1_RX_DMA_FLAG);
		if(uart_port1.recv_cb)
			uart_port1.recv_cb(uart_port1.dma_rx_addr, uart_port1.dma_rx_len - DMA_GetCurrDataCounter(PORT1_RX_DMA_STREAM));
		DMA_SetCurrDataCounter(PORT1_RX_DMA_STREAM, uart_port1.dma_rx_len);
		DMA_Cmd(PORT1_RX_DMA_STREAM, ENABLE);
	}
}

void uart_port1_init(unsigned int bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
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

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = PORT1_UART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void uart_port1_deinit(void)
{
	DMA_Cmd(PORT1_TX_DMA_STREAM, DISABLE);
	DMA_Cmd(PORT1_RX_DMA_STREAM, DISABLE);
	DMA_ITConfig(PORT1_TX_DMA_STREAM, DMA_IT_TC, DISABLE);
	DMA_ITConfig(PORT1_RX_DMA_STREAM, DMA_IT_TC, DISABLE);
	DMA_ClearFlag(PORT1_TX_DMA_STREAM, PORT1_TX_DMA_FLAG);
	DMA_ClearFlag(PORT1_RX_DMA_STREAM, PORT1_RX_DMA_FLAG);
	USART_ITConfig(PORT1_UART_DEV, USART_IT_IDLE, DISABLE);
	USART_Cmd(PORT1_UART_DEV, DISABLE);
	USART_DeInit(PORT1_UART_DEV);
}

void uart_port1_txdma_setup(unsigned char *addr, unsigned short size, uart_port_send_callback_t send_cb)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = PORT1_TX_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_AHB1PeriphClockCmd(PORT1_DMA_RCC, ENABLE);	

	DMA_DeInit(PORT1_TX_DMA_STREAM);
	while(DMA_GetCmdStatus(PORT1_TX_DMA_STREAM) != DISABLE);
		
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	

	DMA_InitStructure.DMA_Channel = PORT1_TX_DMA_CHN;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(PORT1_UART_DEV->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)addr;
	DMA_InitStructure.DMA_BufferSize = size;

	DMA_Init(PORT1_TX_DMA_STREAM, &DMA_InitStructure);
	DMA_ITConfig(PORT1_TX_DMA_STREAM, DMA_IT_TC, ENABLE);

	USART_DMACmd(PORT1_UART_DEV, USART_DMAReq_Tx, ENABLE);

	uart_port1.dma_tx_addr = (void *)addr;
	uart_port1.dma_tx_len = size;
	uart_port1.send_cb = send_cb;
}

void uart_port1_rxdma_setup(unsigned char *addr, unsigned short size, uart_port_receive_callback_t recv_cb)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = PORT1_RX_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_AHB1PeriphClockCmd(PORT1_DMA_RCC, ENABLE);	

	DMA_DeInit(PORT1_RX_DMA_STREAM);
	while(DMA_GetCmdStatus(PORT1_RX_DMA_STREAM) != DISABLE);
		
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	

	DMA_InitStructure.DMA_Channel = PORT1_RX_DMA_CHN;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(PORT1_UART_DEV->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)addr;
	DMA_InitStructure.DMA_BufferSize = size;

	DMA_Init(PORT1_RX_DMA_STREAM, &DMA_InitStructure);
	DMA_ITConfig(PORT1_RX_DMA_STREAM, DMA_IT_TC, ENABLE);

	USART_DMACmd(PORT1_UART_DEV, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(PORT1_RX_DMA_STREAM, ENABLE);

	USART_ITConfig(PORT1_UART_DEV, USART_IT_IDLE, ENABLE);

	uart_port1.dma_rx_addr = (void *)addr;
	uart_port1.dma_rx_len = size;
	uart_port1.recv_cb = recv_cb;
}

void uart_port1_put(unsigned char c)
{
	while(USART_GetFlagStatus(PORT1_UART_DEV, USART_FLAG_TC) == RESET);
	USART_SendData(PORT1_UART_DEV, c);
}

void uart_port1_dma_send(unsigned char *addr, unsigned short size)
{
	while(DMA_GetCmdStatus(PORT1_TX_DMA_STREAM) != DISABLE);
	DMA_MemoryTargetConfig(PORT1_TX_DMA_STREAM, (uint32_t)addr, DMA_Memory_0);
	DMA_SetCurrDataCounter(PORT1_TX_DMA_STREAM, size);
	DMA_Cmd(PORT1_TX_DMA_STREAM, ENABLE);
	uart_port1.dma_tx_addr = (void *)addr;
	uart_port1.dma_tx_len = size;
}


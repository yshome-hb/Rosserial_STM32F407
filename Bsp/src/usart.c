
#include "usart.h"

/**************** PORT1 defines ****************/
#define PORT1_UART_DEV                  USART3
#define PORT1_UART_CLK_ENABLE           __USART3_CLK_ENABLE
#define PORT1_DMA_CLK_ENABLE			__DMA1_CLK_ENABLE
#define PORT1_UART_IRQn                 USART3_IRQn
#define PORT1_IRQ_Handler               USART3_IRQHandler

/*Tx Pin*/
#define PORT1_TX_GPIO_CLK_ENABLE		__GPIOD_CLK_ENABLE
#define PORT1_TX_PORT					GPIOD
#define PORT1_TX_PIN					GPIO_PIN_8
#define PORT1_TX_AF						GPIO_AF7_USART3

/*Tx DMA*/
#define PORT1_TX_DMA_STREAM				DMA1_Stream4
#define PORT1_TX_DMA_CHN				DMA_CHANNEL_7
#define PORT1_TX_DMA_IRQn               DMA1_Stream4_IRQn
#define PORT1_TX_DMA_IRQ_Handler        DMA1_Stream4_IRQHandler

/*Rx PIN*/
#define PORT1_RX_GPIO_CLK_ENABLE		__GPIOD_CLK_ENABLE
#define PORT1_RX_PORT					GPIOD
#define PORT1_RX_PIN					GPIO_PIN_9
#define PORT1_RX_AF						GPIO_AF7_USART3

/*Rx DMA*/
#define PORT1_RX_DMA_STREAM				DMA1_Stream1
#define PORT1_RX_DMA_CHN				DMA_CHANNEL_4
#define PORT1_RX_DMA_IRQn               DMA1_Stream1_IRQn
#define PORT1_RX_DMA_IRQ_Handler        DMA1_Stream1_IRQHandler

static uart_port_handle_t uart_port1;

/*IRQ Handler*/
void PORT1_IRQ_Handler(void)
{
	HAL_UART_IRQHandler(&uart_port1.uart_handle);
}

void PORT1_TX_DMA_IRQ_Handler(void)
{
	HAL_DMA_IRQHandler(&uart_port1.dma_tx_handle);
}

void PORT1_RX_DMA_IRQ_Handler(void)
{
	HAL_DMA_IRQHandler(&uart_port1.dma_rx_handle);
}

/**************** PORT2 defines ****************/
#define PORT2_UART_DEV                  USART2
#define PORT2_UART_CLK_ENABLE           __USART2_CLK_ENABLE
#define PORT2_DMA_CLK_ENABLE			__DMA1_CLK_ENABLE
#define PORT2_UART_IRQn                 USART2_IRQn
#define PORT2_IRQ_Handler               USART2_IRQHandler

/*Tx Pin*/
#define PORT2_TX_GPIO_CLK_ENABLE		__GPIOD_CLK_ENABLE
#define PORT2_TX_PORT					GPIOD
#define PORT2_TX_PIN					GPIO_PIN_5
#define PORT2_TX_AF						GPIO_AF7_USART2

/*Tx DMA*/
#define PORT2_TX_DMA_STREAM				DMA1_Stream6
#define PORT2_TX_DMA_CHN				DMA_CHANNEL_4
#define PORT2_TX_DMA_IRQn               DMA1_Stream6_IRQn
#define PORT2_TX_DMA_IRQ_Handler        DMA1_Stream6_IRQHandler

/*Rx PIN*/
#define PORT2_RX_GPIO_CLK_ENABLE		__GPIOD_CLK_ENABLE
#define PORT2_RX_PORT					GPIOD
#define PORT2_RX_PIN					GPIO_PIN_6
#define PORT2_RX_AF						GPIO_AF7_USART2

/*Rx DMA*/
#define PORT2_RX_DMA_STREAM				DMA1_Stream5
#define PORT2_RX_DMA_CHN				DMA_CHANNEL_4
#define PORT2_RX_DMA_IRQn               DMA1_Stream5_IRQn
#define PORT2_RX_DMA_IRQ_Handler        DMA1_Stream5_IRQHandler

static uart_port_handle_t uart_port2;

/*IRQ Handler*/
void PORT2_IRQ_Handler(void)
{
	HAL_UART_IRQHandler(&uart_port2.uart_handle);
}

void PORT2_TX_DMA_IRQ_Handler(void)
{
	HAL_DMA_IRQHandler(&uart_port2.dma_tx_handle);
}

void PORT2_RX_DMA_IRQ_Handler(void)
{
	HAL_DMA_IRQHandler(&uart_port2.dma_rx_handle);
}

static void uart_port_setup(uart_port_handle_t *uart_port)
{
    uart_port->uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
    uart_port->uart_handle.Init.StopBits = UART_STOPBITS_1;
    uart_port->uart_handle.Init.Parity = UART_PARITY_NONE;
    uart_port->uart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart_port->uart_handle.Init.Mode = UART_MODE_TX_RX;

	HAL_UART_Init(&uart_port->uart_handle);

    HAL_NVIC_SetPriority(uart_port->uart_irq_num, 9, 0);
    HAL_NVIC_EnableIRQ(uart_port->uart_irq_num);
}

void uart_port_deinit(uart_port_handle_t *uart_port)
{
	HAL_UART_Abort(&uart_port->uart_handle);
    HAL_NVIC_DisableIRQ(uart_port->uart_irq_num);
	HAL_UART_DeInit(&uart_port->uart_handle);
}

void uart_port_txdma_setup(uart_port_handle_t *uart_port)
{
	__HAL_LINKDMA(&uart_port->uart_handle, hdmatx, uart_port->dma_tx_handle);

    uart_port->dma_tx_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    uart_port->dma_tx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    uart_port->dma_tx_handle.Init.MemInc = DMA_MINC_ENABLE;
    uart_port->dma_tx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    uart_port->dma_tx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    uart_port->dma_tx_handle.Init.Mode = DMA_NORMAL;
    uart_port->dma_tx_handle.Init.Priority = DMA_PRIORITY_LOW;
    uart_port->dma_tx_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    uart_port->dma_tx_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    uart_port->dma_tx_handle.Init.MemBurst = DMA_MBURST_SINGLE;
    uart_port->dma_tx_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&uart_port->dma_tx_handle);

    HAL_NVIC_SetPriority(uart_port->dma_tx_irq_num, 10, 0);
    HAL_NVIC_EnableIRQ(uart_port->dma_tx_irq_num);
}

void uart_port_rxdma_setup(uart_port_handle_t *uart_port)
{
	__HAL_LINKDMA(&uart_port->uart_handle, hdmarx, uart_port->dma_rx_handle);

    uart_port->dma_rx_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    uart_port->dma_rx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    uart_port->dma_rx_handle.Init.MemInc = DMA_MINC_ENABLE;
    uart_port->dma_rx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    uart_port->dma_rx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    uart_port->dma_rx_handle.Init.Mode = DMA_NORMAL;
    uart_port->dma_rx_handle.Init.Priority = DMA_PRIORITY_MEDIUM;
    uart_port->dma_rx_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    uart_port->dma_rx_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    uart_port->dma_rx_handle.Init.MemBurst = DMA_MBURST_SINGLE;
    uart_port->dma_rx_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&uart_port->dma_rx_handle);

    HAL_NVIC_SetPriority(uart_port->dma_rx_irq_num, 9, 0);
    HAL_NVIC_EnableIRQ(uart_port->dma_rx_irq_num);
}

void uart_port_put(uart_port_handle_t *uart_port, unsigned char c)
{
	while((uart_port->uart_handle.Instance->SR & USART_FLAG_TC) == RESET);
	uart_port->uart_handle.Instance->DR = c;
}

void uart_port_send_it(uart_port_handle_t *uart_port, uint8_t *pdata, uint16_t length, uart_port_send_callback_t send_cb)
{
	HAL_UART_RegisterCallback(&uart_port->uart_handle, HAL_UART_TX_COMPLETE_CB_ID, (pUART_CallbackTypeDef)send_cb);
	HAL_UART_Transmit_IT(&uart_port->uart_handle, pdata, length);
}

void uart_port_send_dma(uart_port_handle_t *uart_port, uint8_t *pdata, uint16_t length, uart_port_send_callback_t send_cb)
{
	HAL_UART_RegisterCallback(&uart_port->uart_handle, HAL_UART_TX_COMPLETE_CB_ID, (pUART_CallbackTypeDef)send_cb);
	HAL_UART_Transmit_DMA(&uart_port->uart_handle, pdata, length);
}

void uart_port_receive_it(uart_port_handle_t *uart_port, uint8_t *pdata, uint16_t length, uart_port_receive_callback_t recv_cb)
{
	HAL_UART_RegisterRxEventCallback(&uart_port->uart_handle, (pUART_RxEventCallbackTypeDef)recv_cb);
	HAL_UARTEx_ReceiveToIdle_IT(&uart_port->uart_handle, pdata, length);
}

void uart_port_receive_dma(uart_port_handle_t *uart_port, uint8_t *pdata, uint16_t length, uart_port_receive_callback_t recv_cb)
{
	HAL_UART_RegisterRxEventCallback(&uart_port->uart_handle, (pUART_RxEventCallbackTypeDef)recv_cb);
	HAL_UARTEx_ReceiveToIdle_DMA(&uart_port->uart_handle, pdata, length);
}

uart_port_handle_t *uart_port1_init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
	
	PORT1_UART_CLK_ENABLE();
	PORT1_DMA_CLK_ENABLE();
	PORT1_TX_GPIO_CLK_ENABLE();
	PORT1_RX_GPIO_CLK_ENABLE();

  	/* Configure the PORT1 TX pin */
	GPIO_InitStruct.Pin = PORT1_TX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = PORT1_TX_AF;
	HAL_GPIO_Init(PORT1_TX_PORT, &GPIO_InitStruct);

  	/* Configure the PORT1 RX pin */
	GPIO_InitStruct.Pin = PORT1_RX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	HAL_GPIO_Init(PORT1_RX_PORT, &GPIO_InitStruct);

	uart_port1.uart_handle.Instance = PORT1_UART_DEV;
	uart_port1.uart_handle.Init.BaudRate = baudrate;
	uart_port1.uart_irq_num = PORT1_UART_IRQn;

    uart_port1.dma_tx_handle.Instance = PORT1_TX_DMA_STREAM;
    uart_port1.dma_tx_handle.Init.Channel = PORT1_TX_DMA_CHN;
	uart_port1.dma_tx_irq_num = PORT1_TX_DMA_IRQn;

    uart_port1.dma_rx_handle.Instance = PORT1_RX_DMA_STREAM;
    uart_port1.dma_rx_handle.Init.Channel = PORT1_RX_DMA_CHN;
	uart_port1.dma_rx_irq_num = PORT1_RX_DMA_IRQn;

	uart_port_setup(&uart_port1);
	return &uart_port1;
}

uart_port_handle_t *uart_port2_init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
	
	PORT2_UART_CLK_ENABLE();
	PORT2_DMA_CLK_ENABLE();
	PORT2_TX_GPIO_CLK_ENABLE();
	PORT2_RX_GPIO_CLK_ENABLE();

  	/* Configure the PORT2 TX pin */
	GPIO_InitStruct.Pin = PORT2_TX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = PORT2_TX_AF;
	HAL_GPIO_Init(PORT2_TX_PORT, &GPIO_InitStruct);

  	/* Configure the PORT2 RX pin */
	GPIO_InitStruct.Pin = PORT2_RX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	HAL_GPIO_Init(PORT2_RX_PORT, &GPIO_InitStruct);

	uart_port2.uart_handle.Instance = PORT2_UART_DEV;
	uart_port2.uart_handle.Init.BaudRate = baudrate;
	uart_port2.uart_irq_num = PORT2_UART_IRQn;

    uart_port2.dma_tx_handle.Instance = PORT2_TX_DMA_STREAM;
    uart_port2.dma_tx_handle.Init.Channel = PORT2_TX_DMA_CHN;
	uart_port2.dma_tx_irq_num = PORT2_TX_DMA_IRQn;

    uart_port2.dma_rx_handle.Instance = PORT2_RX_DMA_STREAM;
    uart_port2.dma_rx_handle.Init.Channel = PORT2_RX_DMA_CHN;
	uart_port2.dma_rx_irq_num = PORT2_RX_DMA_IRQn;

	uart_port_setup(&uart_port2);
	return &uart_port2;
}

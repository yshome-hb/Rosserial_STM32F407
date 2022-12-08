
#include "usart.h"

/**************** PORT1 defines ****************/
#define PORT1_UART_DEV                  USART3
#define PORT1_UART_RCC                  RCC_APB1Periph_USART3
#define PORT1_DMA_RCC					RCC_AHB1Periph_DMA1
#define PORT1_UART_IRQn                 USART3_IRQn
#define PORT1_IRQ_Handler               USART3_IRQHandler

/*Tx Pin*/
#define PORT1_TX_GPIO_RCC				RCC_AHB1Periph_GPIOD
#define PORT1_TX_GPIO					GPIOD
#define PORT1_TX_PIN					GPIO_Pin_8
#define PORT1_TX_PIN_SRC				GPIO_PinSource8
#define PORT1_TX_AF						GPIO_AF_USART3

/*Tx DMA*/
#define PORT1_TX_DMA_STREAM				DMA1_Stream4
#define PORT1_TX_DMA_CHN				DMA_Channel_7
#define PORT1_TX_DMA_IRQn               DMA1_Stream4_IRQn
#define PORT1_TX_DMA_FLAG				DMA_FLAG_TCIF4
#define PORT1_TX_DMA_IRQ_Handler        DMA1_Stream4_IRQHandler

/*Rx PIN*/
#define PORT1_RX_GPIO_RCC				RCC_AHB1Periph_GPIOD
#define PORT1_RX_GPIO					GPIOD
#define PORT1_RX_PIN					GPIO_Pin_9
#define PORT1_RX_PIN_SRC				GPIO_PinSource9
#define PORT1_RX_AF						GPIO_AF_USART3

/*Rx DMA*/
#define PORT1_RX_DMA_STREAM				DMA1_Stream1
#define PORT1_RX_DMA_CHN				DMA_Channel_4
#define PORT1_RX_DMA_IRQn               DMA1_Stream1_IRQn
#define PORT1_RX_DMA_FLAG				DMA_FLAG_TCIF1
#define PORT1_RX_DMA_IRQ_Handler        DMA1_Stream1_IRQHandler

/*IRQ Handler*/
void PORT1_IRQ_Handler(void)
{
	if(USART_GetITStatus(PORT1_UART_DEV, USART_IT_IDLE) != RESET){
		USART_ReceiveData(PORT1_UART_DEV);
		if(DMA_GetCmdStatus(PORT1_RX_DMA_STREAM) == DISABLE){
			if(uart_port1.recv_cb)
				uart_port1.recv_cb(uart_port1.rx_addr, uart_port1.rx_cnt);
			uart_port1.rx_cnt = 0;
		}else{
			DMA_Cmd(PORT1_RX_DMA_STREAM, DISABLE);
			DMA_ClearFlag(PORT1_RX_DMA_STREAM, PORT1_RX_DMA_FLAG);
			uart_port1.rx_cnt = uart_port1.rx_len - DMA_GetCurrDataCounter(PORT1_RX_DMA_STREAM);
			if(uart_port1.recv_cb)
				uart_port1.recv_cb(uart_port1.rx_addr, uart_port1.rx_cnt);
			DMA_SetCurrDataCounter(PORT1_RX_DMA_STREAM, uart_port1.rx_len);
			DMA_Cmd(PORT1_RX_DMA_STREAM, ENABLE);
		}
	}
	else if(USART_GetITStatus(PORT1_UART_DEV, USART_IT_TC) != RESET){
		USART_ClearITPendingBit(PORT1_UART_DEV, USART_IT_TC);
		if(uart_port1.tx_cnt < uart_port1.tx_len){
			USART_SendData(PORT1_UART_DEV, uart_port1.tx_addr[uart_port1.tx_cnt++]);
		}else if(uart_port1.send_cb){
			uart_port1.send_cb();
		}
	}
	else if(USART_GetITStatus(PORT1_UART_DEV, USART_IT_RXNE) != RESET){
		uart_port1.rx_addr[uart_port1.rx_cnt++] = USART_ReceiveData(PORT1_UART_DEV);
		if(uart_port1.rx_cnt >= uart_port1.rx_len){
			if(uart_port1.recv_cb)
				uart_port1.recv_cb(uart_port1.rx_addr, uart_port1.rx_cnt);
			uart_port1.rx_cnt = 0;
		}
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
		uart_port1.rx_cnt = uart_port1.rx_len - DMA_GetCurrDataCounter(PORT1_RX_DMA_STREAM);
		if(uart_port1.recv_cb)
			uart_port1.recv_cb(uart_port1.rx_addr, uart_port1.rx_cnt);
		DMA_SetCurrDataCounter(PORT1_RX_DMA_STREAM, uart_port1.rx_len);
		DMA_Cmd(PORT1_RX_DMA_STREAM, ENABLE);
	}
}


/**************** PORT2 defines ****************/
#define PORT2_UART_DEV                  USART2
#define PORT2_UART_RCC                  RCC_APB1Periph_USART2
#define PORT2_DMA_RCC					RCC_AHB1Periph_DMA1
#define PORT2_UART_IRQn                 USART2_IRQn
#define PORT2_IRQ_Handler               USART2_IRQHandler

/*Tx Pin*/
#define PORT2_TX_GPIO_RCC				RCC_AHB1Periph_GPIOD
#define PORT2_TX_GPIO					GPIOD
#define PORT2_TX_PIN					GPIO_Pin_5
#define PORT2_TX_PIN_SRC				GPIO_PinSource5
#define PORT2_TX_AF						GPIO_AF_USART2

/*Tx DMA*/
#define PORT2_TX_DMA_STREAM				DMA1_Stream6
#define PORT2_TX_DMA_CHN				DMA_Channel_4
#define PORT2_TX_DMA_IRQn               DMA1_Stream6_IRQn
#define PORT2_TX_DMA_FLAG				DMA_FLAG_TCIF6
#define PORT2_TX_DMA_IRQ_Handler        DMA1_Stream6_IRQHandler

/*Rx PIN*/
#define PORT2_RX_GPIO_RCC				RCC_AHB1Periph_GPIOD
#define PORT2_RX_GPIO					GPIOD
#define PORT2_RX_PIN					GPIO_Pin_6
#define PORT2_RX_PIN_SRC				GPIO_PinSource6
#define PORT2_RX_AF						GPIO_AF_USART2

/*Rx DMA*/
#define PORT2_RX_DMA_STREAM				DMA1_Stream5
#define PORT2_RX_DMA_CHN				DMA_Channel_4
#define PORT2_RX_DMA_IRQn               DMA1_Stream5_IRQn
#define PORT2_RX_DMA_FLAG				DMA_FLAG_TCIF5
#define PORT2_RX_DMA_IRQ_Handler        DMA1_Stream5_IRQHandler

/*IRQ Handler*/
void PORT2_IRQ_Handler(void)
{
	if(USART_GetITStatus(PORT2_UART_DEV, USART_IT_IDLE) != RESET){
		USART_ReceiveData(PORT2_UART_DEV);
		if(DMA_GetCmdStatus(PORT2_RX_DMA_STREAM) == DISABLE){
			if(uart_port2.recv_cb)
				uart_port2.recv_cb(uart_port2.rx_addr, uart_port2.rx_cnt);
			uart_port2.rx_cnt = 0;
		}else{
			DMA_Cmd(PORT2_RX_DMA_STREAM, DISABLE);
			DMA_ClearFlag(PORT2_RX_DMA_STREAM, PORT2_RX_DMA_FLAG);
			uart_port2.rx_cnt = uart_port2.rx_len - DMA_GetCurrDataCounter(PORT2_RX_DMA_STREAM);
			if(uart_port2.recv_cb)
				uart_port2.recv_cb(uart_port2.rx_addr, uart_port2.rx_cnt);
			DMA_SetCurrDataCounter(PORT2_RX_DMA_STREAM, uart_port2.rx_len);
			DMA_Cmd(PORT2_RX_DMA_STREAM, ENABLE);
		}
	}
	else if(USART_GetITStatus(PORT2_UART_DEV, USART_IT_TC) != RESET){
		USART_ClearITPendingBit(PORT2_UART_DEV, USART_IT_TC);
		if(uart_port2.tx_cnt < uart_port2.tx_len){
			USART_SendData(PORT2_UART_DEV, uart_port2.tx_addr[uart_port2.tx_cnt++]);
		}else if(uart_port2.send_cb){
			uart_port2.send_cb();
		}
	}
	else if(USART_GetITStatus(PORT2_UART_DEV, USART_IT_RXNE) != RESET){
		uart_port2.rx_addr[uart_port2.rx_cnt++] = USART_ReceiveData(PORT2_UART_DEV);
		if(uart_port2.rx_cnt >= uart_port2.rx_len){
			if(uart_port2.recv_cb)
				uart_port2.recv_cb(uart_port2.rx_addr, uart_port2.rx_cnt);
			uart_port2.rx_cnt = 0;
		}
	}
}

void PORT2_TX_DMA_IRQ_Handler(void)
{
	if(DMA_GetFlagStatus(PORT2_TX_DMA_STREAM, PORT2_TX_DMA_FLAG) != RESET){
		DMA_ClearFlag(PORT2_TX_DMA_STREAM, PORT2_TX_DMA_FLAG);
		DMA_Cmd(PORT2_TX_DMA_STREAM, DISABLE);
		if(uart_port2.send_cb)
			uart_port2.send_cb();
	}
}

void PORT2_RX_DMA_IRQ_Handler(void)
{
	if(DMA_GetFlagStatus(PORT2_RX_DMA_STREAM, PORT2_RX_DMA_FLAG) != RESET){
		DMA_ClearFlag(PORT2_RX_DMA_STREAM, PORT2_RX_DMA_FLAG);
		uart_port2.rx_cnt = uart_port2.rx_len - DMA_GetCurrDataCounter(PORT2_RX_DMA_STREAM);
		if(uart_port2.recv_cb)
			uart_port2.recv_cb(uart_port2.rx_addr, uart_port2.rx_cnt);
		DMA_SetCurrDataCounter(PORT2_RX_DMA_STREAM, uart_port2.rx_len);
		DMA_Cmd(PORT2_RX_DMA_STREAM, ENABLE);
	}
}


static void uart_port_setup(uart_port_handle_t *uart_port)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/*Configure UART param*/
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = uart_port->buadrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(uart_port->uart_dev, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = uart_port->irq_num;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearFlag(uart_port->uart_dev, USART_FLAG_TC);

	USART_Cmd(uart_port->uart_dev, ENABLE); 
}

void uart_port_deinit(uart_port_handle_t *uart_port)
{
	DMA_Cmd(uart_port->dma_tx_stream, DISABLE);
	DMA_Cmd(uart_port->dma_rx_stream, DISABLE);
	DMA_ITConfig(uart_port->dma_tx_stream, DMA_IT_TC, DISABLE);
	DMA_ITConfig(uart_port->dma_rx_stream, DMA_IT_TC, DISABLE);
	DMA_ClearFlag(uart_port->dma_tx_stream, uart_port->dma_tx_irq_flag);
	DMA_ClearFlag(uart_port->dma_rx_stream, uart_port->dma_rx_irq_flag);
	USART_ITConfig(uart_port->uart_dev, USART_IT_IDLE, DISABLE);
	USART_Cmd(uart_port->uart_dev, DISABLE);
	USART_DeInit(uart_port->uart_dev);
}

void uart_port_txdma_setup(uart_port_handle_t *uart_port, void *addr, uint16_t size, uart_port_send_callback_t send_cb)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = uart_port->dma_tx_irq_num;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_DeInit(uart_port->dma_tx_stream);
	while(DMA_GetCmdStatus(uart_port->dma_tx_stream) != DISABLE);
		
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

	DMA_InitStructure.DMA_Channel = uart_port->dma_tx_chn;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(uart_port->uart_dev->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)addr;
	DMA_InitStructure.DMA_BufferSize = size;

	DMA_Init(uart_port->dma_tx_stream, &DMA_InitStructure);
	DMA_ITConfig(uart_port->dma_tx_stream, DMA_IT_TC, ENABLE);

	USART_DMACmd(uart_port->uart_dev, USART_DMAReq_Tx, ENABLE);

	uart_port->tx_addr = (uint8_t *)addr;
	uart_port->tx_len = size;
	uart_port->send_cb = send_cb;
}

void uart_port_rxdma_setup(uart_port_handle_t *uart_port, void *addr, uint16_t size, uart_port_receive_callback_t recv_cb)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = uart_port->dma_rx_irq_num;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_DeInit(uart_port->dma_rx_stream);
	while(DMA_GetCmdStatus(uart_port->dma_rx_stream) != DISABLE);
		
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

	DMA_InitStructure.DMA_Channel = uart_port->dma_rx_chn;;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(uart_port->uart_dev->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)addr;
	DMA_InitStructure.DMA_BufferSize = size;

	DMA_Init(uart_port->dma_rx_stream, &DMA_InitStructure);
	DMA_ITConfig(uart_port->dma_rx_stream, DMA_IT_TC, ENABLE);

	USART_ClearITPendingBit(uart_port->uart_dev, USART_IT_RXNE);
	USART_ITConfig(uart_port->uart_dev, USART_IT_RXNE, DISABLE);
	USART_DMACmd(uart_port->uart_dev, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(uart_port->dma_rx_stream, ENABLE);

	USART_ClearITPendingBit(uart_port->uart_dev, USART_IT_IDLE);
	USART_ITConfig(uart_port->uart_dev, USART_IT_IDLE, ENABLE);

	uart_port->rx_addr = (uint8_t *)addr;
	uart_port->rx_len = size;
	uart_port2.rx_cnt = 0;
	uart_port->recv_cb = recv_cb;
}

void uart_port_put(uart_port_handle_t *uart_port, unsigned char c)
{
	while(USART_GetFlagStatus(uart_port->uart_dev, USART_FLAG_TC) == RESET);
	USART_SendData(uart_port->uart_dev, c);
}

void uart_port_send_it(uart_port_handle_t *uart_port, uint8_t *pdata, uint16_t length, uart_port_send_callback_t send_cb)
{
	if(DMA_GetCmdStatus(uart_port->dma_tx_stream) != DISABLE)
		DMA_Cmd(uart_port->dma_tx_stream, DISABLE);

	uart_port->send_cb = send_cb;
	uart_port->tx_addr = (uint8_t *)pdata;
	uart_port->tx_len = length;
	uart_port->tx_cnt = 0; 
	USART_ClearITPendingBit(uart_port->uart_dev, USART_IT_TC);
	USART_ITConfig(uart_port->uart_dev, USART_IT_TC, ENABLE);
	USART_SendData(uart_port->uart_dev, uart_port2.tx_addr[uart_port2.tx_cnt++]);
}

void uart_port_dma_send(uart_port_handle_t *uart_port, void *addr, uint16_t size)
{
	USART_ClearITPendingBit(uart_port->uart_dev, USART_IT_TC);
	USART_ITConfig(uart_port->uart_dev, USART_IT_TC, DISABLE);
	if(DMA_GetCmdStatus(uart_port->dma_tx_stream) != DISABLE)
		return;
	DMA_MemoryTargetConfig(uart_port->dma_tx_stream, (uint32_t)addr, DMA_Memory_0);
	DMA_SetCurrDataCounter(uart_port->dma_tx_stream, size);
	DMA_Cmd(uart_port->dma_tx_stream, ENABLE);

	uart_port->tx_addr = (uint8_t *)addr;
	uart_port->tx_len = size;
}

void uart_port_receive_it(uart_port_handle_t *uart_port, uint8_t *pdata, uint16_t length, uart_port_receive_callback_t recv_cb)
{
	if(DMA_GetCmdStatus(uart_port->dma_rx_stream) != DISABLE)
		DMA_Cmd(uart_port->dma_rx_stream, DISABLE);

	uart_port->recv_cb = recv_cb;
	uart_port->rx_addr = (uint8_t *)pdata;
	uart_port->rx_len = length;
	uart_port->rx_cnt = 0; 
	USART_ClearITPendingBit(uart_port->uart_dev, USART_IT_RXNE);
	USART_ITConfig(uart_port->uart_dev, USART_IT_RXNE, ENABLE);
	USART_ClearITPendingBit(uart_port->uart_dev, USART_IT_IDLE);
	USART_ITConfig(uart_port->uart_dev, USART_IT_IDLE, ENABLE);
}


uart_port_handle_t uart_port1;

void uart_port1_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(PORT1_TX_GPIO_RCC | PORT1_RX_GPIO_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(PORT1_UART_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(PORT1_DMA_RCC, ENABLE);

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

	uart_port1.uart_dev = PORT1_UART_DEV;
	uart_port1.irq_num = PORT1_UART_IRQn;
	uart_port1.buadrate = bound;

	uart_port1.dma_tx_stream = PORT1_TX_DMA_STREAM;
	uart_port1.dma_tx_chn = PORT1_TX_DMA_CHN;
	uart_port1.dma_tx_irq_num = PORT1_TX_DMA_IRQn;
	uart_port1.dma_tx_irq_flag = PORT1_TX_DMA_FLAG;
	
	uart_port1.dma_rx_stream = PORT1_RX_DMA_STREAM;
	uart_port1.dma_rx_chn = PORT1_RX_DMA_CHN;
	uart_port1.dma_rx_irq_num = PORT1_RX_DMA_IRQn;
	uart_port1.dma_tx_irq_flag = PORT1_RX_DMA_FLAG;

	uart_port_setup(&uart_port1);
}


uart_port_handle_t uart_port2;

void uart_port2_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(PORT2_TX_GPIO_RCC | PORT2_RX_GPIO_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(PORT2_UART_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(PORT2_DMA_RCC, ENABLE);

	GPIO_PinAFConfig(PORT2_TX_GPIO, PORT2_TX_PIN_SRC, PORT2_TX_AF);
	GPIO_PinAFConfig(PORT2_RX_GPIO, PORT2_RX_PIN_SRC, PORT2_RX_AF);

  	/* Configure the PORT2 TX pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = PORT2_TX_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(PORT2_TX_GPIO, &GPIO_InitStructure);

  	/* Configure the PORT2 RX pin */
	GPIO_InitStructure.GPIO_Pin = PORT2_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(PORT2_RX_GPIO, &GPIO_InitStructure);

	uart_port2.uart_dev = PORT2_UART_DEV;
	uart_port2.irq_num = PORT2_UART_IRQn;
	uart_port2.buadrate = bound;

	uart_port2.dma_tx_stream = PORT2_TX_DMA_STREAM;
	uart_port2.dma_tx_chn = PORT2_TX_DMA_CHN;
	uart_port2.dma_tx_irq_num = PORT2_TX_DMA_IRQn;
	uart_port2.dma_tx_irq_flag = PORT2_TX_DMA_FLAG;
	
	uart_port2.dma_rx_stream = PORT2_RX_DMA_STREAM;
	uart_port2.dma_rx_chn = PORT2_RX_DMA_CHN;
	uart_port2.dma_rx_irq_num = PORT2_RX_DMA_IRQn;
	uart_port2.dma_tx_irq_flag = PORT2_RX_DMA_FLAG;

	uart_port_setup(&uart_port2);
}

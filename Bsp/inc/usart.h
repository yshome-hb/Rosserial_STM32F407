#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h" 

typedef void (*uart_port_send_callback_t)(void *handle);
typedef void (*uart_port_receive_callback_t)(void *handle, uint16_t size);

typedef struct {
    UART_HandleTypeDef uart_handle;
	IRQn_Type uart_irq_num;

	DMA_HandleTypeDef dma_tx_handle;
	IRQn_Type dma_tx_irq_num;
    
    DMA_HandleTypeDef dma_rx_handle;
	IRQn_Type dma_rx_irq_num;

} uart_port_handle_t;

void uart_port_deinit(uart_port_handle_t *uart_port);
void uart_port_txdma_setup(uart_port_handle_t *uart_port);
void uart_port_rxdma_setup(uart_port_handle_t *uart_port);
void uart_port_put(uart_port_handle_t *uart_port, unsigned char c);
void uart_port_send_it(uart_port_handle_t *uart_port, uint8_t *pdata, uint16_t length, uart_port_send_callback_t send_cb);
void uart_port_send_dma(uart_port_handle_t *uart_port, uint8_t *pdata, uint16_t length, uart_port_send_callback_t send_cb);
void uart_port_receive_it(uart_port_handle_t *uart_port, uint8_t *pdata, uint16_t length, uart_port_receive_callback_t recv_cb);
void uart_port_receive_dma(uart_port_handle_t *uart_port, uint8_t *pdata, uint16_t length, uart_port_receive_callback_t recv_cb);

uart_port_handle_t *uart_port1_init(uint32_t baudrate);
uart_port_handle_t *uart_port2_init(uint32_t baudrate);

#ifdef __cplusplus
}
#endif

#endif

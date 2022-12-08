#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h" 

typedef void (*uart_port_send_callback_t)(void);
typedef void (*uart_port_receive_callback_t)(void *pdata, uint16_t length);

typedef struct {
    USART_TypeDef *uart_dev;
    uint8_t irq_num;
    uint32_t buadrate;

    DMA_Stream_TypeDef *dma_tx_stream;
    uint32_t dma_tx_chn;
    uint8_t dma_tx_irq_num;
    uint32_t dma_tx_irq_flag;
    void *dma_tx_addr;
    uint16_t dma_tx_len;

    DMA_Stream_TypeDef *dma_rx_stream;
    uint32_t dma_rx_chn;
    uint8_t dma_rx_irq_num;
    uint32_t dma_rx_irq_flag;
    void *dma_rx_addr;
    uint16_t dma_rx_len;

    uart_port_send_callback_t send_cb;
    uart_port_receive_callback_t recv_cb;

} uart_port_handle_t;

extern uart_port_handle_t uart_port1;
extern uart_port_handle_t uart_port2;

void uart_port_deinit(uart_port_handle_t *uart_port);
void uart_port_txdma_setup(uart_port_handle_t *uart_port, void *addr, uint16_t size, uart_port_send_callback_t send_cb);
void uart_port_rxdma_setup(uart_port_handle_t *uart_port, void *addr, uint16_t size, uart_port_receive_callback_t recv_cb);
void uart_port_put(uart_port_handle_t *uart_port, unsigned char c);
void uart_port_dma_send(uart_port_handle_t *uart_port, void *addr, uint16_t size);

void uart_port1_init(uint32_t bound);
void uart_port2_init(uint32_t bound);

#ifdef __cplusplus
}
#endif

#endif

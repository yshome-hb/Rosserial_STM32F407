#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*uart_port_send_callback_t)(void);
typedef void (*uart_port_receive_callback_t)(void *pdata, uint16_t length);

typedef struct {
    void *dma_tx_addr;
    uint16_t dma_tx_len;
    void *dma_rx_addr;
    uint16_t dma_rx_len;
    uart_port_send_callback_t send_cb;
    uart_port_receive_callback_t recv_cb;

} uart_port_handle_t;

void uart_port1_init(unsigned int bound);
void uart_port1_deinit(void);
void uart_port1_txdma_setup(unsigned char *addr, unsigned short size, uart_port_send_callback_t send_cb);
void uart_port1_rxdma_setup(unsigned char *addr, unsigned short size, uart_port_receive_callback_t recv_cb);
void uart_port1_put(unsigned char c);
void uart_port1_dma_send(unsigned char *addr, unsigned short size);

#ifdef __cplusplus
}
#endif

#endif

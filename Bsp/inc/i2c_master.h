#ifndef __I2C_MASTER_H__
#define __I2C_MASTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h" 

typedef void (*i2c_master_send_callback_t)(void *handle);
typedef void (*i2c_master_receive_callback_t)(void *handle, uint16_t size);

typedef struct {
    I2C_HandleTypeDef i2c_handle;
	IRQn_Type i2c_irq_num;

} i2c_master_handle_t;

void i2c_master_deinit(i2c_master_handle_t *i2c_master);
void i2c_transmit(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint32_t timeout);
void i2c_receive(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint32_t timeout);
void i2c_writeReg(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint32_t timeout);
void i2c_readReg(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint32_t timeout);

i2c_master_handle_t *i2c_master1_init(uint32_t clock);
i2c_master_handle_t *i2c_master2_init(uint32_t clock);

#ifdef __cplusplus
}
#endif

#endif

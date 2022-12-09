#ifndef __I2C_MASTER_H__
#define __I2C_MASTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h" 

typedef struct {
    I2C_TypeDef *i2c_dev;
    uint32_t clock;

} i2c_master_handle_t;

typedef int16_t i2c_status_t;

void i2c_master_deinit(i2c_master_handle_t *i2c_master);
i2c_status_t i2c_master_transmit(i2c_master_handle_t *i2c_master, uint8_t address, const uint8_t *pdata, uint16_t length, uint16_t timeout);
i2c_status_t i2c_master_receive(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint16_t timeout);
i2c_status_t i2c_master_writeReg(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t reg, const uint8_t *pdata, uint16_t length, uint16_t timeout);
i2c_status_t i2c_master_readReg(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t reg, uint8_t *pdata, uint16_t length, uint16_t timeout);

i2c_master_handle_t *i2c_master1_init(uint32_t clock);
i2c_master_handle_t *i2c_master2_init(uint32_t clock);

#ifdef __cplusplus
}
#endif

#endif

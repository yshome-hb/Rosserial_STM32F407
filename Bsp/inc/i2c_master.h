#ifndef __I2C_MASTER_H__
#define __I2C_MASTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h" 

#define I2C_DEV_TIMEOUT     1000

typedef struct {
    I2C_HandleTypeDef i2c_handle;
	// IRQn_Type i2c_irq_num;

} i2c_master_handle_t;

typedef int32_t i2c_status_t;

#define I2C_STATUS_SUCCESS  (0)
#define I2C_STATUS_ERROR    (1)
#define I2C_STATUS_BUSY     (2)
#define I2C_STATUS_TIMEOUT  (3)

void i2c_master_deinit(i2c_master_handle_t *i2c_master);
i2c_status_t i2c_transmit(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint32_t timeout);
i2c_status_t i2c_receive(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint32_t timeout);
i2c_status_t i2c_writeReg(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint32_t timeout);
i2c_status_t i2c_readReg(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint32_t timeout);

i2c_master_handle_t *i2c_master1_init(uint32_t clock);
i2c_master_handle_t *i2c_master2_init(uint32_t clock);

void I2C_setHandle(i2c_master_handle_t *i2c_master);
i2c_status_t I2C_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
i2c_status_t I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
i2c_status_t I2C_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
i2c_status_t I2C_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
i2c_status_t I2C_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
i2c_status_t I2C_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data);
i2c_status_t I2C_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
i2c_status_t I2C_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif

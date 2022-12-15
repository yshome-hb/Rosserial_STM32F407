#include "i2c_master.h"

/**************** Master1 defines ****************/
#define MASTER1_I2C_DEV                 I2C1
#define MASTER1_I2C_CLK_ENABLE          __I2C1_CLK_ENABLE

/*SCL Pin*/
#define MASTER1_SCL_GPIO_CLK_ENABLE     __GPIOB_CLK_ENABLE
#define MASTER1_SCL_PORT                GPIOB
#define MASTER1_SCL_PIN                 GPIO_PIN_8
#define MASTER1_SCL_AF                  GPIO_AF4_I2C1

/*SDA PIN*/
#define MASTER1_SDA_GPIO_CLK_ENABLE     __GPIOB_CLK_ENABLE
#define MASTER1_SDA_PORT                GPIOB
#define MASTER1_SDA_PIN                 GPIO_PIN_9
#define MASTER1_SDA_AF                  GPIO_AF4_I2C1

static i2c_master_handle_t i2c_master1;

/**************** Master2 defines ****************/
#define MASTER2_I2C_DEV                 I2C2
#define MASTER2_I2C_CLK_ENABLE          __I2C2_CLK_ENABLE

/*SCL Pin*/
#define MASTER2_SCL_GPIO_CLK_ENABLE     __GPIOB_CLK_ENABLE
#define MASTER2_SCL_PORT                GPIOB
#define MASTER2_SCL_PIN                 GPIO_PIN_10
#define MASTER2_SCL_AF                  GPIO_AF4_I2C2

/*SDA PIN*/
#define MASTER2_SDA_GPIO_CLK_ENABLE     __GPIOB_CLK_ENABLE
#define MASTER2_SDA_PORT                GPIOB
#define MASTER2_SDA_PIN                 GPIO_PIN_11
#define MASTER2_SDA_AF                  GPIO_AF4_I2C2

static i2c_master_handle_t i2c_master2;

static void i2c_master_setup(i2c_master_handle_t *i2c_master)
{
	i2c_master->i2c_handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
	i2c_master->i2c_handle.Init.OwnAddress1 = 0;
	i2c_master->i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2c_master->i2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c_master->i2c_handle.Init.OwnAddress2 = 0;
	i2c_master->i2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c_master->i2c_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	HAL_I2C_Init(&i2c_master->i2c_handle);	
}

void i2c_master_deinit(i2c_master_handle_t *i2c_master)
{
	HAL_I2C_DeInit(&i2c_master->i2c_handle);
}

i2c_status_t i2c_master_transmit(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint32_t timeout)
{
	return HAL_I2C_Master_Transmit(&i2c_master->i2c_handle, address, pdata, length, timeout);
}

i2c_status_t i2c_master_receive(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint32_t timeout)
{
	return HAL_I2C_Master_Receive(&i2c_master->i2c_handle, address, pdata, length, timeout);
}

i2c_status_t i2c_master_writeReg(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t reg, uint8_t *pdata, uint16_t length, uint32_t timeout)
{
	return HAL_I2C_Mem_Write(&i2c_master->i2c_handle, address, reg, I2C_MEMADD_SIZE_8BIT, pdata, length, timeout);
}

i2c_status_t i2c_master_readReg(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t reg, uint8_t *pdata, uint16_t length, uint32_t timeout)
{
	return HAL_I2C_Mem_Read(&i2c_master->i2c_handle, address, reg, I2C_MEMADD_SIZE_8BIT, pdata, length, timeout);
}

i2c_master_handle_t *i2c_master1_init(uint32_t clock)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	MASTER1_I2C_CLK_ENABLE();
	MASTER1_SCL_GPIO_CLK_ENABLE();
	MASTER1_SDA_GPIO_CLK_ENABLE();

  	/* Configure the Master1 SCL pin */
	GPIO_InitStruct.Pin = MASTER1_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = MASTER1_SCL_AF;
	HAL_GPIO_Init(MASTER1_SCL_PORT, &GPIO_InitStruct);

  	/* Configure the Master1 SDA pin */
	GPIO_InitStruct.Pin = MASTER1_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = MASTER1_SDA_AF;
	HAL_GPIO_Init(MASTER1_SDA_PORT, &GPIO_InitStruct);

	i2c_master1.i2c_handle.Instance = MASTER1_I2C_DEV;
	i2c_master1.i2c_handle.Init.ClockSpeed = clock;

	i2c_master_setup(&i2c_master1);
	return &i2c_master1;
}

i2c_master_handle_t *i2c_master2_init(uint32_t clock)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	MASTER2_I2C_CLK_ENABLE();
	MASTER2_SCL_GPIO_CLK_ENABLE();
	MASTER2_SDA_GPIO_CLK_ENABLE();

  	/* Configure the Master2 SCL pin */
	GPIO_InitStruct.Pin = MASTER2_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = MASTER2_SCL_AF;
	HAL_GPIO_Init(MASTER2_SCL_PORT, &GPIO_InitStruct);

  	/* Configure the Master2 SDA pin */
	GPIO_InitStruct.Pin = MASTER2_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = MASTER2_SDA_AF;
	HAL_GPIO_Init(MASTER2_SDA_PORT, &GPIO_InitStruct);

	i2c_master2.i2c_handle.Instance = MASTER2_I2C_DEV;
	i2c_master2.i2c_handle.Init.ClockSpeed = clock;

	i2c_master_setup(&i2c_master2);
	return &i2c_master2;
}


static I2C_HandleTypeDef *i2c_phandle = NULL;

void I2C_setHandle(i2c_master_handle_t *i2c_master) {
	i2c_phandle = &i2c_master->i2c_handle;
}

/** Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (I2C_STATUS_SUCCESS = success)
 */
i2c_status_t I2C_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data) {
	return HAL_I2C_Mem_Read(i2c_phandle, devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, data, 1, I2C_DEV_TIMEOUT);
}

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (I2C_STATUS_SUCCESS = success)
 */
i2c_status_t I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {
	return HAL_I2C_Mem_Read(i2c_phandle, devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, data, length, I2C_DEV_TIMEOUT);
}

/** Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (I2C_STATUS_SUCCESS = success)
 */
i2c_status_t I2C_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
    uint8_t b;
    i2c_status_t ret = I2C_readByte(devAddr, regAddr, &b);
	if(ret == I2C_STATUS_SUCCESS)
    	*data = b & (1 << bitNum);
    return ret;
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (I2C_STATUS_SUCCESS = success)
 */
i2c_status_t I2C_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t b;
    i2c_status_t ret = I2C_readByte(devAddr, regAddr, &b);
    if (ret == I2C_STATUS_SUCCESS) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
    }
    return ret;
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (I2C_STATUS_SUCCESS = success)
 */
i2c_status_t I2C_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
	return HAL_I2C_Mem_Write(i2c_phandle, devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, &data, 1, I2C_DEV_TIMEOUT);
}

/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (I2C_STATUS_SUCCESS = success)
 */
i2c_status_t I2C_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
	return HAL_I2C_Mem_Write(i2c_phandle, devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, data, length, I2C_DEV_TIMEOUT);
}

/** write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (I2C_STATUS_SUCCESS = success)
 */
i2c_status_t I2C_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    i2c_status_t ret = I2C_readByte(devAddr, regAddr, &b);
	if(ret != I2C_STATUS_SUCCESS) return ret;
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return I2C_writeByte(devAddr, regAddr, b);
}

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (I2C_STATUS_SUCCESS = success)
 */
i2c_status_t I2C_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    i2c_status_t ret = I2C_readByte(devAddr, regAddr, &b);
	if(ret != I2C_STATUS_SUCCESS) return ret;
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    b &= ~(mask); // zero all important bits in existing byte
    b |= data; // combine data with existing byte
    return I2C_writeByte(devAddr, regAddr, b);
}



#include "delay.h"
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
#define MASTER2_SCL_GPIO                GPIOB
#define MASTER2_SCL_PIN                 GPIO_PIN_10
#define MASTER2_SCL_AF                  GPIO_AF4_I2C2

/*SDA PIN*/
#define MASTER2_SDA_GPIO_CLK_ENABLE            RCC_AHB1Periph_GPIOB
#define MASTER2_SDA_GPIO                GPIOB
#define MASTER2_SDA_PIN                 GPIO_Pin_11
#define MASTER2_SDA_PIN_SRC             GPIO_PinSource11
#define MASTER2_SDA_AF                  GPIO_AF_I2C2

static i2c_master_handle_t i2c_master2;

#define i2c_wait_event_until_timeout(dev, event, timeout) {	\
	uint32_t start = sys_time_ms();					\
	while(!I2C_CheckEvent(dev, event)){				\
		if(sys_time_elapsed(start) > timeout)		\
			return I2C_STATUS_TIMEOUT;				\
		delay_ms(10);								\
	}												\
}

static void i2c_master_setup(i2c_master_handle_t *i2c_master)
{
	I2C_InitTypeDef  I2C_InitStructure;  

	/*Configure I2C param*/
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = i2c_master->clock;
	I2C_Init(i2c_master->i2c_dev, &I2C_InitStructure);	

	I2C_Cmd(i2c_master->i2c_dev, ENABLE);
	I2C_AcknowledgeConfig(i2c_master->i2c_dev, ENABLE);
}

void i2c_master_deinit(i2c_master_handle_t *i2c_master)
{
	I2C_Cmd(i2c_master->i2c_dev, DISABLE);
	I2C_DeInit(i2c_master->i2c_dev);
}

i2c_status_t i2c_master_transmit(i2c_master_handle_t *i2c_master, uint8_t address, const uint8_t *pdata, uint16_t length, uint32_t timeout)
{
	I2C_GenerateSTART(i2c_master->i2c_dev, ENABLE);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_MODE_SELECT, timeout);

	I2C_Send7bitAddress(i2c_master->i2c_dev, address, I2C_Direction_Transmitter);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, timeout);

	while(length--){
		I2C_SendData(i2c_master->i2c_dev, *pdata);
		pdata++;
		i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_BYTE_TRANSMITTED, timeout);
	}

	I2C_GenerateSTOP(i2c_master->i2c_dev, ENABLE);
	return I2C_STATUS_SUCCESS;
}

i2c_status_t i2c_master_receive(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t *pdata, uint16_t length, uint32_t timeout)
{
	I2C_GenerateSTART(i2c_master->i2c_dev, ENABLE);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_MODE_SELECT, timeout);

	I2C_Send7bitAddress(i2c_master->i2c_dev, address, I2C_Direction_Receiver);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, timeout);

	while(length--){
		if(length == 0){
			I2C_AcknowledgeConfig(i2c_master->i2c_dev, DISABLE);
			I2C_GenerateSTOP(i2c_master->i2c_dev, ENABLE);
    	}

		i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_BYTE_RECEIVED, timeout);
      	*pdata = I2C_ReceiveData(i2c_master->i2c_dev);
      	pdata++;
	}

	I2C_AcknowledgeConfig(i2c_master->i2c_dev, ENABLE);
	return I2C_STATUS_SUCCESS;
}

i2c_status_t i2c_master_writeReg(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t reg, const uint8_t *pdata, uint16_t length, uint32_t timeout)
{
	I2C_GenerateSTART(i2c_master->i2c_dev, ENABLE);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_MODE_SELECT, timeout);

	I2C_Send7bitAddress(i2c_master->i2c_dev, address, I2C_Direction_Transmitter);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, timeout);

	I2C_SendData(i2c_master->i2c_dev, reg);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_BYTE_TRANSMITTED, timeout);

	while(length--){
		I2C_SendData(i2c_master->i2c_dev, *pdata);
		pdata++;
		i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_BYTE_TRANSMITTED, timeout);
	}

	I2C_GenerateSTOP(i2c_master->i2c_dev, ENABLE);
	return I2C_STATUS_SUCCESS;
}

i2c_status_t i2c_master_readReg(i2c_master_handle_t *i2c_master, uint8_t address, uint8_t reg, uint8_t *pdata, uint16_t length, uint32_t timeout)
{
	I2C_GenerateSTART(i2c_master->i2c_dev, ENABLE);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_MODE_SELECT, timeout);

	I2C_Send7bitAddress(i2c_master->i2c_dev, address, I2C_Direction_Transmitter);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, timeout);

	I2C_SendData(i2c_master->i2c_dev, reg);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_BYTE_TRANSMITTED, timeout);

	I2C_GenerateSTART(i2c_master->i2c_dev, ENABLE);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_MODE_SELECT, timeout);

	I2C_Send7bitAddress(i2c_master->i2c_dev, address, I2C_Direction_Receiver);
	i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, timeout);

	while(length--){
		if(length == 0){
			I2C_AcknowledgeConfig(i2c_master->i2c_dev, DISABLE);
			I2C_GenerateSTOP(i2c_master->i2c_dev, ENABLE);
    	}

		i2c_wait_event_until_timeout(i2c_master->i2c_dev, I2C_EVENT_MASTER_BYTE_RECEIVED, timeout);
      	*pdata = I2C_ReceiveData(i2c_master->i2c_dev);
      	pdata++;
	}

	I2C_AcknowledgeConfig(i2c_master->i2c_dev, ENABLE);
	return I2C_STATUS_SUCCESS;
}

i2c_master_handle_t *i2c_master1_init(uint32_t clock)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(MASTER1_SCL_GPIO_RCC | MASTER1_SDA_GPIO_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(MASTER1_I2C_RCC, ENABLE);

	GPIO_PinAFConfig(MASTER1_SCL_GPIO, MASTER1_SCL_PIN_SRC, MASTER1_SCL_AF);
	GPIO_PinAFConfig(MASTER1_SDA_GPIO, MASTER1_SDA_PIN_SRC, MASTER1_SDA_AF);

  	/* Configure the Master1 SCL pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MASTER1_SCL_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(MASTER1_SCL_GPIO, &GPIO_InitStructure);

  	/* Configure the Master1 SDA pin */
	GPIO_InitStructure.GPIO_Pin = MASTER1_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(MASTER1_SDA_GPIO, &GPIO_InitStructure);

	i2c_master1.i2c_dev = MASTER1_I2C_DEV;
	i2c_master1.clock = clock;

	i2c_master_setup(&i2c_master1);
	return &i2c_master1;
}

i2c_master_handle_t *i2c_master2_init(uint32_t clock)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(MASTER2_SCL_GPIO_RCC | MASTER2_SDA_GPIO_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(MASTER2_I2C_RCC, ENABLE);

	GPIO_PinAFConfig(MASTER2_SCL_GPIO, MASTER2_SCL_PIN_SRC, MASTER2_SCL_AF);
	GPIO_PinAFConfig(MASTER2_SDA_GPIO, MASTER2_SDA_PIN_SRC, MASTER2_SDA_AF);

  	/* Configure the Master2 SCL pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MASTER2_SCL_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(MASTER2_SCL_GPIO, &GPIO_InitStructure);

  	/* Configure the Master2 SDA pin */
	GPIO_InitStructure.GPIO_Pin = MASTER2_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(MASTER2_SDA_GPIO, &GPIO_InitStructure);

	i2c_master2.i2c_dev = MASTER2_I2C_DEV;
	i2c_master2.clock = clock;

	i2c_master_setup(&i2c_master2);
	return &i2c_master2;
}


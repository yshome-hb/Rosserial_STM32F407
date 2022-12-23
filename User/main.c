/**
  ******************************************************************************
  * File   : main.c 
  * Version: V1.0.0
  * Date   : 2020-11-27
  * Brief  : Main program body
  ******************************************************************************
  */ 

#include "FreeRTOS.h"
#include "task.h"
#include "arch_init.h"
#include "arch_define.h"
#include "io.h"
#include "usart.h"
#include "i2c_master.h"
#include "mpu6050.h"

#define START_TASK_PRIO		1	//Task priority
#define START_STK_SIZE 		256 //Task stack size
TaskHandle_t StartTask_Handler;	//Task handle 

#define TASK1_PRIO          2   //Task priority
#define TASK1_STK_SIZE      128 //Task stack size
TaskHandle_t Task1Task_Handler; //Task handle 

uint8_t recv_data[32];
uint16_t recv_len = 0;

void uart_port1_send_complete(void)
{
	io_output_set(OUTPUT_LED_1, 1);
}

void uart_port1_recv_data(uint8_t *pdata, uint16_t length)
{
	memcpy(recv_data, pdata, length);
	recv_len = length;
	if(length < 5)
		io_output_set(OUTPUT_LED_1, 0);
	else
		io_output_set(OUTPUT_LED_1, 1);
}


//Start task task function
void task1(void *pvParameters)
{
	char send_str[10] = {'a', 'b', 'c', 'd'};
	char recv_str[10];
	int16_t acc_x;
	int16_t acc_y;
	int16_t acc_z;

	servo_init();
	io_output_init();
	uart_port_handle_t *uart_port = uart_port2_init(115200);
	i2c_master_handle_t *i2c_master = i2c_master2_init(400000);
	I2C_setHandle(i2c_master);
	// uart_port_txdma_setup(uart_port, send_str, 10, uart_port1_send_complete);
	// uart_port_rxdma_setup(uart_port, recv_str, 5, uart_port1_recv_data);
	uart_port_receive_it(uart_port, recv_str, 5, uart_port1_recv_data);
	servo_attach(0, 500);
	servo_attach(1, 2000);

	// MPU6050_initialize();

	ssd1306_init();

	while(1)
	{

		// I2C_readByte(0xD0, 0x75, i2c_regs);
		// acc_x = MPU6050_getAccelerationX();
		// acc_y = MPU6050_getAccelerationY();
		// acc_z = MPU6050_getAccelerationZ();

		LOG_INFO("hello world %d %d %d", acc_x, acc_y, acc_z);

		// uart_port_send_it(uart_port, send_str, 4, uart_port1_send_complete);
		// uart_port_dma_send(&uart_port2, send_str, 4);
		//LEDa��500ms,��500ms
		io_output_set(OUTPUT_LED_1, 0);
		arch_msleep(500);
		io_output_set(OUTPUT_LED_1, 1);
		arch_msleep(500);
	}
}

/**
 * @brief       start_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* �����ٽ��� */
    /* ��������1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);

    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}

int main(void)
{
	arch_mcu_init();

	//Create the start task
	xTaskCreate((TaskFunction_t )start_task,            //Task function
				(const char*    )"start_task",          //Task name
				(uint16_t       )START_STK_SIZE,        //Task stack size
				(void*          )NULL,                  //Arguments passed to the task function
				(UBaseType_t    )START_TASK_PRIO,       //Task priority
				(TaskHandle_t*  )&StartTask_Handler);   //Task handle

	vTaskStartScheduler();  //Enables task scheduling
}

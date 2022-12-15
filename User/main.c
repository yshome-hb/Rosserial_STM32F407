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

	servo_init();
	io_output_init();
	uart_port_handle_t *uart_port = uart_port2_init(115200);
	// i2c_master_handle_t *i2c_master = i2c_master2_init(400000);
	// uart_port_txdma_setup(uart_port, send_str, 10, uart_port1_send_complete);
	// uart_port_rxdma_setup(uart_port, recv_str, 5, uart_port1_recv_data);
	uart_port_receive_it(uart_port, recv_str, 5, uart_port1_recv_data);
	servo_attach(0, 500);
	servo_attach(1, 2000);

	while(1)
	{
		LOG_INFO("hello world");
		// i2c_master_readReg(i2c_master, 0xD0, 0x75, i2c_regs, 1, 1000);
		uart_port_send_it(uart_port, send_str, 4, uart_port1_send_complete);
		// uart_port_dma_send(&uart_port2, send_str, 4);
		//LEDa亮500ms,灭500ms
		io_output_set(OUTPUT_LED_1, 0);
		arch_msleep(500);
		io_output_set(OUTPUT_LED_1, 1);
		arch_msleep(500);
	}
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);

    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
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

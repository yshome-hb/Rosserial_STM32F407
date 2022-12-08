/**
  ******************************************************************************
  * File   : main.c 
  * Version: V1.0.0
  * Date   : 2020-11-27
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "arch_init.h"
#include "arch_define.h"
#include "io.h"
#include "usart.h"

/* Private define ------------------------------------------------------------*/
#define START_TASK_PRIO		1	//Task priority

#define START_STK_SIZE 		256 //Task stack size 

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
TaskHandle_t StartTask_Handler;	//Task handle
uint8_t recv_data[32];
uint16_t recv_len = 0;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

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
void start_task(void *pvParameters)
{
	char send_str[10] = {'a', 'b', 'c', 'd'};
	char recv_str[10];

	servo_init();
	io_output_init();
	uart_port2_init(115200);
	// uart_port_txdma_setup(&uart_port2, send_str, 10, uart_port1_send_complete);
	// uart_port_rxdma_setup(&uart_port2, recv_str, 5, uart_port1_recv_data);
	uart_port_receive_it(&uart_port2, recv_str, 5, uart_port1_recv_data);

	while(1)
	{
		LOG_INFO("hello world");
		uart_port_send_it(&uart_port2, send_str, 4, uart_port1_send_complete);
		// uart_port_dma_send(&uart_port2, send_str, 4);
		//LEDa亮500ms,灭500ms
		io_output_set(OUTPUT_LED_1, 0);
		arch_msleep(500);
		io_output_set(OUTPUT_LED_1, 1);
		arch_msleep(500);
	}
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


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

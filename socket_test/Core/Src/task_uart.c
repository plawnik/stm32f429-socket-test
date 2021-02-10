/*
 * uart_task.c
 *
 *  Created on: Jan 18, 2021
 *      Author: pawel
 */

// Simple uart debugger
// to use call UartTaskInit() at program start
// and send data by UartDbg() like printf()
// you can define UART_TASK_PRIORITY in other file
// WARNING! UartDbg() uses sprintf function(high memory usage)
// care about stack size in tasks using this function

#include <task_uart.h>
#include "cmsis_os.h"
#include "queue.h"



// set default priority if not defined
#ifndef UART_TASK_PRIORITY
#define UART_TASK_PRIORITY (osPriorityNormal + 1)
#endif


// freertos definitions
QueueHandle_t uart_messages;
UART_HandleTypeDef *dbg_uart;
// fix to use with freertoss V2 api
osThreadId_t uartTaskHandle;
const osThreadAttr_t uartTask_attributes = {
  .name = "uartTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size =  1024*2
};
//osThreadId uartTaskHandle;



// init uart send task
void UartTaskInit(UART_HandleTypeDef *uart_handle) {
  dbg_uart = uart_handle;
  uart_messages = xQueueCreate(32, sizeof(uint8_t*));
  // fix to use with freertoss V2 api
  uartTaskHandle = osThreadNew(StartUartTask, NULL, &uartTask_attributes);
  //osThreadDef(uartTask, StartUartTask, UART_TASK_PRIORITY, 0, 1024);
  //uartTaskHandle = osThreadCreate(osThread(uartTask), NULL);
}

// sprintf implementation to easy use
void vprint(const char *fmt, va_list argp) {
  char *txt;
  txt = pvPortMalloc(100);
  if (0 < vsprintf(txt, fmt, argp)) // build string
  {
    xQueueSend(uart_messages,&txt,0);
  }
}

// caller to send data funtion
void UartDbg(const char *fmt, ...) // custom printf() function
{
  vTaskSuspendAll();
  va_list argp;
  va_start(argp, fmt);
  vprint(fmt, argp);
  va_end(argp);
  vTaskResume(uartTaskHandle);// resume main sending task
  xTaskResumeAll();

}

// main sending task
void StartUartTask(void *argument) {
  // set priority to lower value, when task init finished
  vTaskPrioritySet(NULL,UART_TASK_PRIORITY);
  uint8_t *mes=NULL;
  while (1) {
    while (xQueueReceive(uart_messages, &mes, 0) == pdTRUE) {
      // WTF??
      // DMA version not working when LWIP init func is called,
      // probably problem with static allocation for lwip task
      // or disabling DMA module for ETH interface init
      //while(!(dbg_uart->gState == HAL_UART_STATE_READY)) { }
      //HAL_UART_Transmit_DMA(dbg_uart, (uint8_t*)mes, strlen((char*)mes));
      HAL_UART_Transmit(dbg_uart, (uint8_t*)mes, strlen((char*)mes), 10);
      vPortFree(mes);
    }
    vTaskSuspend(NULL); // sleep this task
  }
}


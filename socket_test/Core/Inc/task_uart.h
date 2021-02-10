/*
 * uart_task.h
 *
 *  Created on: Jan 18, 2021
 *      Author: pawel
 */

#ifndef INC_TASK_UART_H_
#define INC_TASK_UART_H_

#include "main.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void StartUartTask(void * argument);
void UartTaskInit(UART_HandleTypeDef *uart_handle);
void UartDbg(const char *fmt, ...); // custom printf() function

#endif /* INC_TASK_UART_H_ */

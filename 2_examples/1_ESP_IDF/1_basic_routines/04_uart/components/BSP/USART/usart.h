/**
 ****************************************************************************************************
 * @file        uart.h
 * @author      ALIENTEK
 * @brief       UART code
 * @license     Copyright (C) 2020-2030, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK DNESP32S3 board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  :
 * version      data         notes
 * V1.0         20240430     the first version
 *
 ****************************************************************************************************
 */

#ifndef _USART_H
#define _USART_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/uart_select.h"
#include "driver/gpio.h"


/* pin definition */
/* default GPIO43 as U0TXD, GPIO44 as U0RXD */
#define USART_UX            UART_NUM_0
#define USART_TX_GPIO_PIN   GPIO_NUM_43
#define USART_RX_GPIO_PIN   GPIO_NUM_44

/* receive buf size */
#define RX_BUF_SIZE         1024

/* function declaration */
void usart_init(uint32_t baudrate); /* 初始化串口 */

#endif

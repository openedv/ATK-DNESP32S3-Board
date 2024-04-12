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
 * change logs  ：
 * version      data         notes
 * V1.0         20240309     the first version
 *
 ****************************************************************************************************
 */

#ifndef __UART_H
#define __UART_H

#include "Arduino.h"

/* pin definition */
/* default GPIO43 as U0TXD, GPIO44 as U0RXD 
 * TXD_PIN and RXD_PIN as UART1 or UART2(didn't used)
 */
#define TXD_PIN      19
#define RXD_PIN      20

/* function declaration */
void uart_init(uint8_t uartx, uint32_t baud);

#endif

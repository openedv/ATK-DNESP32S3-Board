/**
 ****************************************************************************************************
 * @file        uart.cpp
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

#include "uart.h"

/**
* @brief       configure uart
* @param       uartx：serial number(0~2)
* @param       baud：in bits per second
* @retval      None
*/
void uart_init(uint8_t uartx, uint32_t baud) 
{
    if (uartx == 0)
    {
        Serial.begin(baud);                                   /* uart0 initialization */
    }
    else if (uartx == 1)
    {
        Serial1.begin(baud, SERIAL_8N1, RXD_PIN, TXD_PIN);    /* uart1 initialization */
    }
    else if (uartx == 2)
    {
        Serial2.begin(baud, SERIAL_8N1, RXD_PIN, TXD_PIN);    /* uart2 initialization */
    }
}



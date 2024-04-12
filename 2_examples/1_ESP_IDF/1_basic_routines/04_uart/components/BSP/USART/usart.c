/**
 ****************************************************************************************************
 * @file        uart.c
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

#include "usart.h"


/**
* @brief       configure uart
* @param       baudrate: baud rate
* @retval      None
*/
void usart_init(uint32_t baudrate)
{
    /* zero-initialize the config structure */
    uart_config_t uart_config = {0};

    /* configure UART */
    uart_config.baud_rate = baudrate;                   /* UART baud rate */
    uart_config.data_bits = UART_DATA_8_BITS;           /* UART byte size */
    uart_config.parity = UART_PARITY_DISABLE;           /* UART parity mode */
    uart_config.stop_bits = UART_STOP_BITS_1;           /* UART stop bits */
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;   /* UART HW flow control mode (cts/rts) */
    uart_config.source_clk = UART_SCLK_APB;             /* UART HW RTS threshold */
    uart_config.rx_flow_ctrl_thresh = 122;              /* UART source clock selection */
    uart_param_config(USART_UX, &uart_config);          /* Configure UART parameters */

    /* set UART pins */
    uart_set_pin(USART_UX, USART_TX_GPIO_PIN, USART_RX_GPIO_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    /* install UART driver (we don't need an event queue here) */
    uart_driver_install(USART_UX, RX_BUF_SIZE * 2, RX_BUF_SIZE * 2, 20, NULL, 0);
}

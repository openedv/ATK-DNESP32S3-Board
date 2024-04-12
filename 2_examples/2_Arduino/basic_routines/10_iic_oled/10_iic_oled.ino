/**
 ****************************************************************************************************
 * @file        10_iic_oled.ino
 * @author      ALIENTEK
 * @brief       OLED example
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

#include "oled.h"
#include "uart.h"
#include "xl9555.h"


/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    uart_init(0, 115200);   /* uart0 initialization */
    xl9555_init();          /* xl9555 initialization */
    oled_init();            /* oled initialization */
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    oled_show_demo();
}

/**
 ****************************************************************************************************
 * @file        05_timer_it.ino
 * @author      ALIENTEK
 * @brief       TIMER_IT example
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

#include "tim.h"
#include "led.h"


/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    led_init();                 /* led initialization */
    timx_int_init(5000, 8000);  /* timer initialization */
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    /* nothing to do, waitting for interrupt */
    delay(1000);
}
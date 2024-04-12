/**
 ****************************************************************************************************
 * @file        03_exti.ino
 * @author      ALIENTEK
 * @brief       EXTI example
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

#include "led.h"
#include "exti.h"


/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    led_init();         /* led initialization */
    exti_init();        /* interrupt pin initialization */
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    LED(led_state);     /* led state is determined by led_state, led_state change is implemented in key_isr function */
}

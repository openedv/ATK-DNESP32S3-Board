/**
 ****************************************************************************************************
 * @file        led.cpp
 * @author      ALIENTEK
 * @brief       LED code
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

/**
 * @brief       configure led pin
 * @param       None
 * @retval      None
 */
void led_init(void) 
{
    pinMode(LED_PIN, OUTPUT);     /* set LED_PIN as output */
    digitalWrite(LED_PIN, HIGH);  /* combined with Board_SCH, set LED_PIN HIGH, LED is off */
}



/**
 ****************************************************************************************************
 * @file        07_led_pwm.ino
 * @author      ALIENTEK
 * @brief       LED_PWM example
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

#include "pwm.h"


uint16_t g_ledpwmval = 0;     /* duty */
uint8_t g_dir = 1;            /* change_direction */

/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    led_pwm_init(1000, 10);   /* led pwm initialization */
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    if (g_dir)
    {
        g_ledpwmval += 5;
    }
    else
    {
        g_ledpwmval -= 5;
    }

    if (g_ledpwmval > 1005)
    {
        g_dir = 0;
    }

    if (g_ledpwmval < 5)
    {
        g_dir = 1;
    }

    pwm_set_duty(g_ledpwmval);  /* write duty to led pwm */
    delay(10);
}
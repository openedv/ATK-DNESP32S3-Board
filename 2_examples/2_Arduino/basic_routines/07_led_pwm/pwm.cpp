/**
 ****************************************************************************************************
 * @file        pwm.cpp
 * @author      ALIENTEK
 * @brief       PWM code
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

/**
 * @brief       configure led pwm
 * @param       frequency: freq limits depend on resolution
 * @param       resolution: 1-16 bits
 * @retval      None
 */
void led_pwm_init(uint16_t frequency, uint8_t resolution)
{
    ledcSetup(LED_PWM_CHANNEL, frequency, resolution);    /* initialize channels  */
    ledcAttachPin(LED_PWM_PIN, LED_PWM_CHANNEL);          /* assign LED_PWM_PIN to channel */
}

/**
 * @brief       pwm duty set
 * @param       duty: pwm_duty
 * @retval      None
 */
void pwm_set_duty(uint16_t duty)
{
    ledcWrite(LED_PWM_CHANNEL, duty);   /* change the duty cycle of pwm */
}



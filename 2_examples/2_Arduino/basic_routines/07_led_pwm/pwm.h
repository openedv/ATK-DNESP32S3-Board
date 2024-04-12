/**
 ****************************************************************************************************
 * @file        pwm.h
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

#ifndef __PWM_H
#define __PWM_H

#include "Arduino.h"

/* led pwm definition */
#define LED_PWM_PIN       1
#define LED_PWM_CHANNEL   0

/* function declaration */
void led_pwm_init(uint16_t frequency, uint8_t resolution);  
void pwm_set_duty(uint16_t duty);                           

#endif

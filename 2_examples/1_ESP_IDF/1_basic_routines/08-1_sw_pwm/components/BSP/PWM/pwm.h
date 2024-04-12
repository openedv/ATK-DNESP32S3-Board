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

#ifndef __PWM_H_
#define __PWM_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "driver/gpio.h"


/* led pwm definition */
#define LEDC_PWM_TIMER          LEDC_TIMER_1
#define LEDC_PWM_CH0_GPIO       GPIO_NUM_1
#define LEDC_PWM_CH0_CHANNEL    LEDC_CHANNEL_1

/* function declaration */
void pwm_init(uint8_t resolution, uint16_t freq);
void pwm_set_duty(uint16_t duty);

#endif

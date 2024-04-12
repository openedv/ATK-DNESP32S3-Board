/**
 ****************************************************************************************************
 * @file        esptim.h
 * @author      ALIENTEK
 * @brief       ESPTIM code
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

#ifndef __ESPTIM_H_
#define __ESPTIM_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "soc/timer_group_struct.h"
#include "driver/timer.h"
#include "esp_timer.h"
#include "led.h"


extern esp_timer_handle_t esp_tim_handle;
/* function declaration */
void esptim_int_init(uint16_t arr, uint64_t tp);
void TIM_PeriodElapsedCallback(void *arg);

#endif

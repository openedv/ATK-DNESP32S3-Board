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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "led.h"


/* function declaration */
void esptim_int_init(uint64_t tps);
void esptim_callback(void *arg);

#endif
/**
 ****************************************************************************************************
 * @file        exti.h
 * @author      ALIENTEK
 * @brief       EXTI code
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

#ifndef __EXIT_H_
#define __EXIT_H_

#include "esp_err.h"
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_system.h" 
#include "esp_log.h"
#include "sdkconfig.h"
#include "led.h"


/* pin definition */
#define BOOT_INT_GPIO_PIN   GPIO_NUM_0

#define BOOT                gpio_get_level(BOOT_INT_GPIO_PIN)

/* function declaration */
void exit_init(void);

#endif

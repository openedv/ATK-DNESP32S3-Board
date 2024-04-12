/**
 ****************************************************************************************************
 * @file        key.h
 * @author      ALIENTEK
 * @brief       KEY code
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

#ifndef __KEY_H_
#define __KEY_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


/* pin definition */
#define BOOT_GPIO_PIN   GPIO_NUM_0

/* key operate definition */
#define BOOT            gpio_get_level(BOOT_GPIO_PIN)

#define BOOT_PRES       1

/* function declaration */
void key_init(void);
uint8_t key_scan(uint8_t mode);

#endif

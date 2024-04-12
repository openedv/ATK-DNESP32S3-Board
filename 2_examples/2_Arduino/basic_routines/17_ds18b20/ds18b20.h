/**
 ****************************************************************************************************
 * @file        ds18b20.h
 * @author      ALIENTEK
 * @brief       DS18B20 code
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

#ifndef __DS18B20_H
#define __DS18B20_H

#include "Arduino.h"
#include "esp_system.h"

/* pin definition */
#define DS18B20_DQ_PIN      GPIO_NUM_0

/* ds18b20 operate definition */
#define DS18B20_DQ_OUT(x)   gpio_set_level(DS18B20_DQ_PIN, x)
#define DS18B20_DQ_IN       gpio_get_level(DS18B20_DQ_PIN)

#define DS18B20_MODE_IN     gpio_set_direction(DS18B20_DQ_PIN, GPIO_MODE_INPUT)
#define DS18B20_MODE_OUT    gpio_set_direction(DS18B20_DQ_PIN, GPIO_MODE_OUTPUT)

/* function declaration */
uint8_t ds18b20_init(void);             
uint8_t ds18b20_check(void);            
short ds18b20_get_temperature(void);   

#endif

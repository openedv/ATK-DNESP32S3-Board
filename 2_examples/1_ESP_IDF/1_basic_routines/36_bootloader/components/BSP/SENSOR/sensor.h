/**
 ****************************************************************************************************
 * @file        sensor.h
 * @author      ALIENTEK
 * @brief       SENSOR code
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


#ifndef __SENSOR_H_
#define __SENSOR_H_

#include "esp_err.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/temperature_sensor.h"


#define SENSOR_RANGE_MIN    20
#define SENSOR_RANGE_MAX    50

/* function declaration */
void temperature_sensor_init(void);
float sensor_get_temperature(void);

#endif
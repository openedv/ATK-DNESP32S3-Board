/**
 ****************************************************************************************************
 * @file        camera.h
 * @author      ALIENTEK
 * @brief       CAMERA code
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

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_camera.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "xl9555.h"
#include "lcd.h"


/* WROVER-KIT PIN Map */
#define CAM_PIN_PWDN     GPIO_NUM_NC    /* power down is not used */
#define CAM_PIN_RESET    GPIO_NUM_NC    /* software reset will be performed */
#define CAM_PIN_XCLK     GPIO_NUM_NC
#define CAM_PIN_SIOD     GPIO_NUM_39
#define CAM_PIN_SIOC     GPIO_NUM_38
#define CAM_PIN_D7       GPIO_NUM_18
#define CAM_PIN_D6       GPIO_NUM_17
#define CAM_PIN_D5       GPIO_NUM_16
#define CAM_PIN_D4       GPIO_NUM_15
#define CAM_PIN_D3       GPIO_NUM_7
#define CAM_PIN_D2       GPIO_NUM_6
#define CAM_PIN_D1       GPIO_NUM_5
#define CAM_PIN_D0       GPIO_NUM_4
#define CAM_PIN_VSYNC    GPIO_NUM_47
#define CAM_PIN_HREF     GPIO_NUM_48
#define CAM_PIN_PCLK     GPIO_NUM_45

#define CAM_PWDN(x)         do{ x ? \
                                (xl9555_pin_write(OV_PWDN_IO, 1)):       \
                                (xl9555_pin_write(OV_PWDN_IO, 0));       \
                            }while(0)

#define CAM_RST(x)          do{ x ? \
                                (xl9555_pin_write(OV_RESET_IO, 1)):       \
                                (xl9555_pin_write(OV_RESET_IO, 0));       \
                            }while(0)

/* function declaration */
uint8_t camera_init(void);
void camera_show(uint16_t x, uint16_t y);
#endif

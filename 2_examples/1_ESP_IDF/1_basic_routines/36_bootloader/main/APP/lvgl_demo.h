/**
 ****************************************************************************************************
 * @file        lvgl_demo.h
 * @author      ALIENTEK
 * @brief       LVGL DEMO code
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
 
#ifndef __LVGL_DEMO_H
#define __LVGL_DEMO_H

#include "lcd.h"
#include "spi_sdcard.h"
#include "key.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "demos/lv_demos.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t xGuiSemaphore;
extern uint32_t back_act_key;

/* function declaration */
void lvgl_demo(void);

#endif

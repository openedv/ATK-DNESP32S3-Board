/**
 ****************************************************************************************************
 * @file        lv_pic_demo.h
 * @author      ALIENTEK
 * @brief       LVGL PIC Demo code
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

#ifndef __LV_PIC_DEMO_H
#define __LV_PIC_DEMO_H

#include "lvgl.h"
#include "ff.h"
#include "lcd.h"
#include "app_ui.h"
#include "exfuns.h"
#include "ff.h"
#include "driver/i2s.h"
#include "driver/i2s_std.h"
#include "i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "lvgl_demo.h"
#include "bmp.h"
#include "jpeg.h"
#include "png.h"
#include "sdmmc_cmd.h"


enum PIC_STATE
{
    PIC_NULL,
    PIC_PAUSE,
    PIC_PLAY,
    PIC_NEXT,
    PIC_PREV
};

/* function declaration */
void lv_pic_demo(void);

#endif

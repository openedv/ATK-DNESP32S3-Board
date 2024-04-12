/**
 ****************************************************************************************************
 * @file        lv_camera_demo.h
 * @author      ALIENTEK
 * @brief       LVGL CAMERA Demo code
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

#ifndef __LV_CAMERA_DEMO_H
#define __LV_CAMERA_DEMO_H

#include "lvgl.h"
#include "ff.h"
#include "lcd.h"
#include "app_ui.h"
#include "camera.h"
#include "esp_camera.h"
#include "mjpeg.h"
#include "lv_file_demo.h"


typedef struct
{
    lv_obj_t *lv_camera_cont;

    struct
    {
        lv_obj_t* camera_header;
    }camera_buf;

}lv_camera_struct;

/* function declaration */
void lv_camera_demo(void);

#endif

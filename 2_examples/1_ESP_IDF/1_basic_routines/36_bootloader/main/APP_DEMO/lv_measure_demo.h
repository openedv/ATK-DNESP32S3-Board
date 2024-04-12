/**
 ****************************************************************************************************
 * @file        lv_measure_demo.h
 * @author      ALIENTEK
 * @brief       LVGL MEASURE Demo code
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

#ifndef __LV_MEASURE_DEMO_H
#define __LV_MEASURE_DEMO_H

#include "lvgl.h"
#include "ff.h"
#include "lcd.h"
#include "app_ui.h"
#include "sensor.h"
#include "driver/temperature_sensor.h"


typedef struct
{
    lv_obj_t *lv_main_cont;
    
   struct
   {
        lv_obj_t *lv_small_cont;
        struct
        {
            lv_timer_t * lv_ap3216c_timer;
            lv_obj_t * ap_ir;
            lv_obj_t * ap_ps;
            lv_obj_t * ap_als;
            lv_obj_t * ap_ir_label;
            lv_obj_t * ap_ir_unit;
            lv_obj_t * ap_ps_label;
            lv_obj_t * ap_ps_unit;
            lv_obj_t * ap_als_label;
            lv_obj_t * ap_als_unit;
        }ap3216c;
            
        struct
        {
            lv_timer_t * lv_temp_timer;
            lv_obj_t * temp_obj;
            lv_obj_t * temp_label;
            lv_obj_t * temp_unit;
            lv_obj_t * temp_slider;
            lv_obj_t * temp_line1;
            lv_obj_t * temp_line1_1;
            lv_obj_t * temp_line2;
            lv_obj_t * temp_line2_1;
        }temp;
   }small_cont;
}measure_obj_t;

/* function declaration */
void lv_measure_demo(void);

#endif

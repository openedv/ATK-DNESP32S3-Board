/**
 ****************************************************************************************************
 * @file        touch.h
 * @author      ALIENTEK
 * @brief       TOUCH code
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

#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/mphal.h"
#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ltdc.h"
#include "gt9xxx.h"


#define TP_PRES_DOWN    0x8000  /* Touch screen pressed flag */
#define TP_CATH_PRES    0x4000
#define CT_MAX_TOUCH    10

typedef struct
{
    uint8_t (*init)(void);      /* Initialize touch screen */
    uint8_t (*scan)(uint8_t);   /* Scan touch screen. 0, screen scanning; 1. Physical coordinates */
    uint16_t x[CT_MAX_TOUCH];   /* Current coordinates */
    uint16_t y[CT_MAX_TOUCH];   /* The capacitive screen has a maximum of 10 sets of coordinates */

    uint16_t sta;               /* state
                                 * b15:Press 1/Release 0
                                 * b14:0:no button pressed.1:A button is pressed
                                 * b13~b10:reserve
                                 * b9~b0:The number of points pressed on the capacitive touch screen (0 indicates not pressed, 1 indicates pressed)
                                 */

    uint8_t touchtype;
} _m_tp_dev;

extern _m_tp_dev tp_dev;

uint8_t tp_init(void);
uint8_t tp_scan(uint8_t mode);
#endif
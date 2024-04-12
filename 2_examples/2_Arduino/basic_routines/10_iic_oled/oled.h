/**
 ****************************************************************************************************
 * @file        oled.h
 * @author      ALIENTEK
 * @brief       OLED code
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

#ifndef __OLED_H
#define __OLED_H

#include "Arduino.h"

/* pin definition */
#define OLED_SCL_PIN      4
#define OLED_SDA_PIN      5
#define OLED_D2_PIN       6
#define OLED_DC_PIN       38

#define OLED_ADDR         0X3C  /* 7bits device address */

#define OLED_RST(x)       xl9555_pin_set(OV_RESET, x ? IO_SET_HIGH : IO_SET_LOW)

/* function declaration */
void oled_init(void);           
void oled_show_demo(void);      

#endif
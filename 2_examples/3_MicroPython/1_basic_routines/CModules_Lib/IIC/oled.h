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
 
#ifndef __OLED_H__
#define __OLED_H__

#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/mphal.h"
#include "myiic.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"


#define OLED_ADDR       0X3C    /* addr */
#define OLED_CMD        0x00    /* command */
#define OLED_DATA       0x40    /* data */

/* function declaration */
void oled_init(void);
void oled_on(void);
void oled_off(void);
void oled_clear(void);
void oled_draw_point(uint8_t x,uint8_t y,uint8_t t);
void oled_fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void oled_show_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void oled_show_num(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void oled_show_string(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size);
void oled_refresh_gram(void);

#endif  /*  __OLED_H__ */
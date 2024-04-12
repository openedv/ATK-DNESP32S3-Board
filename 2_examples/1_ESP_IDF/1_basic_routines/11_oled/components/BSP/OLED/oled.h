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

#include "iic.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "xl9555.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  OLED_PIN_RESET = 0u,
  OLED_PIN_SET
} OLED_PinState;

#define OLED_ADDR       0X3C    /* addr */
#define OLED_CMD        0x00    /* command */
#define OLED_DATA       0x40    /* data */

/* pin definition */
#define OLED_SCL_PIN    4
#define OLED_SDA_PIN    5
#define OLED_D2_PIN     6
#define OLED_DC_PIN     38

/* oled operate definition */
#define OLED_DC(x)      do{ x ?                                          \
                            gpio_set_level(OLED_DC_PIN, OLED_PIN_SET) :  \
                            gpio_set_level(OLED_DC_PIN, OLED_PIN_RESET); \
                          }while(0) 

#define OLED_RST(x)     do{ x ?                                            \
                            xl9555_pin_write(OV_RESET_IO, OLED_PIN_SET) :  \
                            xl9555_pin_write(OV_RESET_IO, OLED_PIN_RESET); \
                          }while(0)

/* function declaration */
void oled_init(i2c_obj_t self);
void oled_on(void);
void oled_off(void);
void oled_clear(void);
void oled_draw_point(uint8_t x, uint8_t y, uint8_t dot);
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode);
uint32_t oled_pow(uint8_t m, uint8_t n);
void oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
void oled_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size);
void oled_refresh_gram(void);

#endif

#ifdef  __cplusplus
}

#endif /*  __cplusplus */

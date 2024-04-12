/**
 ****************************************************************************************************
 * @file        ltdc.h
 * @author      ALIENTEK
 * @brief       LTDC code
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

#ifndef _LTDC_H
#define _LTDC_H

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "xl9555.h"
#include <math.h>

/* Please update the following configuration according to your LCD spec */
#define LCD_BL(x)       do { x ?                                 \
                             xl9555_pin_write(LCD_BL_IO, 1):     \
                             xl9555_pin_write(LCD_BL_IO, 0);     \
                        } while(0)

#define GPIO_LCD_DE     (GPIO_NUM_4)
#define GPIO_LCD_VSYNC  (GPIO_NUM_NC)
#define GPIO_LCD_HSYNC  (GPIO_NUM_NC)
#define GPIO_LCD_PCLK   (GPIO_NUM_5)

#define GPIO_LCD_R3     (GPIO_NUM_45)
#define GPIO_LCD_R4     (GPIO_NUM_48)
#define GPIO_LCD_R5     (GPIO_NUM_47)
#define GPIO_LCD_R6     (GPIO_NUM_21)
#define GPIO_LCD_R7     (GPIO_NUM_14)

#define GPIO_LCD_G2     (GPIO_NUM_10)
#define GPIO_LCD_G3     (GPIO_NUM_9)
#define GPIO_LCD_G4     (GPIO_NUM_46)
#define GPIO_LCD_G5     (GPIO_NUM_3)
#define GPIO_LCD_G6     (GPIO_NUM_8)
#define GPIO_LCD_G7     (GPIO_NUM_18)

#define GPIO_LCD_B3     (GPIO_NUM_17)
#define GPIO_LCD_B4     (GPIO_NUM_16)
#define GPIO_LCD_B5     (GPIO_NUM_15)
#define GPIO_LCD_B6     (GPIO_NUM_7)
#define GPIO_LCD_B7     (GPIO_NUM_6)


/* stroke color */
#define WHITE           0xFFFF
#define BLACK           0x0000
#define RED             0xF800
#define GREEN           0x07E0
#define BLUE            0x001F
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define CYAN            0x07FF


typedef struct  
{
    uint32_t pwidth;        /* RGBLCD width */
    uint32_t pheight;       /* RGBLCD height */
    uint16_t hsw;           /* Vertical sync width, unit: number of lines */
    uint16_t vsw;           /* Horizontal sync width, unit: PCLK period */
    uint16_t hbp;           /* Horizontal back porch, number of PCLK between hsync and start of line active data */
    uint16_t vbp;           /* Vertical back porch, number of invalid lines between vsync and start of frame */
    uint16_t hfp;           /* Horizontal front porch, number of PCLK between the end of active data and the next hsync */
    uint16_t vfp;           /* Vertical front porch, number of invalid lines between the end of frame and the next vsync  */
    uint8_t activelayer;
    uint8_t dir;            /* LCD display direction */
    uint16_t id;            /* LTDC ID */
    uint32_t pclk_hz;       /* Frequency of pixel clock */
    uint16_t width;         /* RGBLCD width */
    uint16_t height;        /* RGBLCD height */
} _ltdc_dev; 

extern _ltdc_dev ltdcdev;

extern esp_lcd_panel_handle_t panel_handle;

/* function declaration */
void ltdc_init(void);
void ltdc_clear(uint16_t color);
uint16_t ltdc_rgb888_to_565(uint8_t r, uint8_t g, uint8_t b);
void ltdc_display_dir(uint8_t dir);
void ltdc_draw_point(uint16_t x, uint16_t y, uint16_t color);
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void ltdc_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ltdc_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void ltdc_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);
void ltdc_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);
void ltdc_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);
void ltdc_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);
void ltdc_draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color);
void ltdc_app_show_mono_icos(uint16_t x,uint16_t y,uint8_t width,uint8_t height,uint8_t *icosbase,uint16_t color,uint16_t bkcolor);
#endif

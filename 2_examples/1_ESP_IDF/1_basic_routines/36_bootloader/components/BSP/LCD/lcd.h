/**
 ****************************************************************************************************
 * @file        lcd.h
 * @author      ALIENTEK
 * @brief       LCD code
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

#ifndef __LCD_H__
#define __LCD_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "xl9555.h"
#include "spi.h"


/* pin definition */
#define LCD_NUM_WR      GPIO_NUM_40
#define LCD_NUM_CS      GPIO_NUM_21

/* LCD operate definition */
#define LCD_WR(x)       do{ x ? \
                            (gpio_set_level(LCD_NUM_WR, 1)):    \
                            (gpio_set_level(LCD_NUM_WR, 0));    \
                        }while(0)

#define LCD_CS(x)       do{ x ? \
                            (gpio_set_level(LCD_NUM_CS, 1)):    \
                            (gpio_set_level(LCD_NUM_CS, 0));    \
                        }while(0)

#define LCD_PWR(x)       do{ x ? \
                            (xl9555_pin_write(SLCD_PWR_IO, 1)): \
                            (xl9555_pin_write(SLCD_PWR_IO, 0)); \
                        }while(0)

#define LCD_RST(x)       do{ x ? \
                            (xl9555_pin_write(SLCD_RST_IO, 1)): \
                            (xl9555_pin_write(SLCD_RST_IO, 0)); \
                        }while(0)

/* stroke color */
#define WHITE           0xFFFF
#define BLACK           0x0000
#define RED             0xF800
#define GREEN           0x07E0
#define BLUE            0x001F
#define MAGENTA         0XF81F
#define YELLOW          0XFFE0
#define CYAN            0X07FF
#define BROWN           0XBC40
#define BRRED           0XFC07
#define GRAY            0X8430
#define DARKBLUE        0X01CF
#define LIGHTBLUE       0X7D7C
#define GRAYBLUE        0X5458
#define LIGHTGREEN      0X841F
#define LGRAY           0XC618 
#define LGRAYBLUE       0XA651
#define LBBLUE          0X2B12

/* Scanning direction definition */
#define L2R_U2D         0           /* From left to right, from top to bottom */
#define L2R_D2U         1           /* From left to right, from bottom to top */
#define R2L_U2D         2           /* From right to left, from top to bottom */
#define R2L_D2U         3           /* From right to left, from bottom to top */
#define U2D_L2R         4           /* From top to bottom, from left to right */
#define U2D_R2L         5           /* From top to bottom, from right to left */
#define D2U_L2R         6           /* From bottom to top, from left to right */
#define D2U_R2L         7           /* From bottom to top, from right to left */

#define DFT_SCAN_DIR    L2R_U2D     /* Default scanning direction */

#define LCD_320X240     0
#define LCD_240X240     1


typedef struct _lcd_obj_t
{
    uint16_t        width;
    uint16_t        height;
    uint8_t         dir;
    uint16_t        wramcmd;
    uint16_t        setxcmd;
    uint16_t        setycmd;
    uint16_t        wr;
    uint16_t        cs;
} lcd_obj_t;

#define LCD_TOTAL_BUF_SIZE      (320 * 240 * 2)
#define LCD_BUF_SIZE            15360

extern lcd_obj_t lcd_self;
extern uint8_t lcd_buf[LCD_TOTAL_BUF_SIZE];

/* function declaration */
void lcd_init(void);
void lcd_clear(uint16_t color);
void lcd_scan_dir(uint8_t dir);
void lcd_write_data(const uint8_t *data, int len);
void lcd_write_data16(uint16_t data);
void lcd_set_cursor(uint16_t xpos, uint16_t ypos);
void lcd_set_window(uint16_t xstar, uint16_t ystar,uint16_t xend,uint16_t yend);
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);
void lcd_draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color);
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void lcd_show_char(uint16_t x, uint16_t y, uint8_t chr, uint8_t size, uint8_t mode, uint16_t color);
void lcd_color_fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);
void lcd_app_show_mono_icos(uint16_t x,uint16_t y,uint8_t width,uint8_t height,uint8_t *icosbase,uint16_t color,uint16_t bkcolor);
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void lcd_display_dir(uint8_t dir);

#endif

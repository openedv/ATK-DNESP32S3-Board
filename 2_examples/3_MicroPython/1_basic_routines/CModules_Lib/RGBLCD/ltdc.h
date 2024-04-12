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
 
#ifndef __LTDC_H__
#define __LTDC_H__

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/mphal.h"
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"


#define GPIO_LCD_DE         (GPIO_NUM_4)
#define GPIO_LCD_VSYNC      (GPIO_NUM_NC)
#define GPIO_LCD_HSYNC      (GPIO_NUM_NC)
#define GPIO_LCD_PCLK       (GPIO_NUM_5)

#define GPIO_LCD_R3         (GPIO_NUM_45)
#define GPIO_LCD_R4         (GPIO_NUM_48)
#define GPIO_LCD_R5         (GPIO_NUM_47)
#define GPIO_LCD_R6         (GPIO_NUM_21)
#define GPIO_LCD_R7         (GPIO_NUM_14)

#define GPIO_LCD_G2         (GPIO_NUM_10)
#define GPIO_LCD_G3         (GPIO_NUM_9)
#define GPIO_LCD_G4         (GPIO_NUM_46)
#define GPIO_LCD_G5         (GPIO_NUM_3)
#define GPIO_LCD_G6         (GPIO_NUM_8)
#define GPIO_LCD_G7         (GPIO_NUM_18)

#define GPIO_LCD_B3         (GPIO_NUM_17)
#define GPIO_LCD_B4         (GPIO_NUM_16)
#define GPIO_LCD_B5         (GPIO_NUM_15)
#define GPIO_LCD_B6         (GPIO_NUM_7)
#define GPIO_LCD_B7         (GPIO_NUM_6)

/* color */
#define WHITE               0xFFFF      /* white */
#define BLACK               0x0000      /* black */
#define RED                 0xF800      /* red */
#define GREEN               0x07E0      /* green */
#define BLUE                0x001F      /* blue */ 
#define MAGENTA             0xF81F      /* BLUE + RED */
#define YELLOW              0xFFE0      /* GREEN + RED */
#define CYAN                0x07FF      /* GREEN + BLUE */  

#define BROWN               0xBC40      /* brown */
#define BRRED               0xFC07      /* Brown red */
#define GRAY                0x8430      /* grey */ 
#define DARKBLUE            0x01CF      /* dark blue */
#define LIGHTBLUE           0x7D7C      /* light blue */ 
#define GRAYBLUE            0x5458      /* Grey blue */ 
#define LIGHTGREEN          0x841F      /* light green */  
#define LGRAY               0xC618      /* PANNEL */ 
#define LGRAYBLUE           0xA651      /* light gray blue */ 
#define LBBLUE              0x2B12      /* light brown blue */ 


typedef struct  
{
	mp_obj_base_t base;     /* Basic address */
    uint32_t pwidth;        /* height */
    uint32_t pheight;       /* width */
    uint16_t hsw;           /* horizontal synchronization width */
    uint16_t vsw;           /* vertical synchronization width */
    uint16_t hbp;           /* horizontal rear corridor */
    uint16_t vbp;           /* vertical rear corridor */
    uint16_t hfp;           /* horizontal front porch */
    uint16_t vfp;           /* vertical front porch */
    uint8_t activelayer;
    uint8_t dir;            /* 0,portrait; 1,landscape */
    uint16_t id;            /* LTDC ID */
    uint32_t pclk_hz;       /* pixel clock */
    uint16_t width;         /* LTDC width */
    uint16_t height;        /* LTDC height */
}_ltdc_dev;


extern _ltdc_dev *ltdc_self;

#endif  /*  __LCD_H__ */

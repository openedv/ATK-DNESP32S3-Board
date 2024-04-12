/**
 ****************************************************************************************************
 * @file        piclib.h
 * @author      ALIENTEK
 * @brief       PICLIB code
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

#ifndef __PICLIB_H
#define __PICLIB_H

#include "lcd.h"
#include <unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "ff.h"
#include "exfuns.h"
#include "gif.h"
#include "bmp.h"
#include "jpeg.h"
#include "png.h"


#define PIC_FORMAT_ERR      0x27    /* format error */
#define PIC_SIZE_ERR        0x28    /* image size error */
#define PIC_WINDOW_ERR      0x29    /* window setting error */
#define PIC_MEM_ERR         0x11    /* memory error */

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#define rgb565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

/* image display physical layer interface */
typedef struct
{
    /* void draw_point(uint16_t x,uint16_t y,uint32_t color) Draw point function */
    void(*draw_point)(uint16_t, uint16_t, uint16_t);
    
    /* void fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color) Monochromatic fill function */
    void(*fill)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
    
    /* void draw_hline(uint16_t x0,uint16_t y0,uint16_t len,uint16_t color) Draw horizontal line function */
    void(*draw_hline)(uint16_t, uint16_t, uint16_t, uint16_t);
    
    /* void piclib_fill_color(uint16_t x,uint16_t y,uint16_t size,uint16_t *color) Multi point filling Multi point filling */
    void(*multicolor)(uint16_t, uint16_t, uint16_t, uint16_t *);
} _pic_phy;

extern _pic_phy pic_phy;


typedef struct
{
    uint16_t lcdwidth;      /* LCD width */
    uint16_t lcdheight;     /* LCD height */
} _pic_info;

extern _pic_info picinfo;

/* function declaration */
void piclib_mem_free (void *paddr);
void *piclib_mem_malloc (uint32_t size);
void piclib_init(void);
uint8_t piclib_ai_load_picfile(char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

#endif

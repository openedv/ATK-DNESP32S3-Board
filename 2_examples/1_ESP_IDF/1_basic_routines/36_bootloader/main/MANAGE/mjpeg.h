/**
 ****************************************************************************************************
 * @file        mjpeg.h
 * @author      ALIENTEK
 * @brief       MJPEG code
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
 
#ifndef __MJPEG_H
#define __MJPEG_H 

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "jpeglib.h"
#include "lcd.h"


struct my_error_mgr
{
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

extern int Windows_Width;       /* image Width */
extern int Windows_Height;      /* image Height */
#define rgb565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

typedef void (*lcd_write_cb)(uint32_t w,uint32_t h,uint8_t *video_buf);

/* function declaration */
char mjpegdec_init(uint16_t offx, uint16_t offy);
void mjpegdec_free(void);
uint8_t mjpegdec_decode(uint8_t* buf, uint32_t bsize,lcd_write_cb lcd_cb);
void mjpegdec_malloc(void);
void mjpegdec_video_free(void);
#endif

/**
 ****************************************************************************************************
 * @file        font.h
 * @author      ALIENTEK
 * @brief       FONT code
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

#ifndef __FONTS_H
#define __FONTS_H

#include "esp_partition.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"
#include "ff.h"
#include "lcd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


/* font library storage first address */
extern uint32_t FONTINFOADDR;

typedef struct
{
    uint8_t fontok;             /* The font library has a flag, 0XAA, and the font library is normal; Other, the font library does not exist */
    uint32_t ugbkaddr;          /* unigbk file address */
    uint32_t ugbksize;          /* unigbk file size */
    uint32_t f12addr;           /* gbk12 file address */
    uint32_t gbk12size;         /* gbk12 file size */
    uint32_t f16addr;           /* gbk16 file address */
    uint32_t gbk16size;         /* gbk16 file size */
    uint32_t f24addr;           /* gbk24 file address */
    uint32_t gbk24size;         /* gbk24 file size */
} _font_info;

extern _font_info ftinfo;

/* function declaration */
esp_err_t fonts_partition_read(void * buffer, uint32_t offset, uint32_t length);
esp_err_t fonts_partition_write(void * buffer, uint32_t offset, uint32_t length);
esp_err_t fonts_partition_erase_sector(uint32_t offset);
uint8_t fonts_update_font(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color);
void fonts_progress_show(uint16_t x, uint16_t y, uint8_t size, uint32_t totsize, uint32_t pos, uint16_t color);
uint8_t fonts_init(void);

#endif

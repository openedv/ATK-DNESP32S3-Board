/**
 ****************************************************************************************************
 * @file        text.h
 * @author      ALIENTEK
 * @brief       TEXT code
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

#ifndef __TEXT_H
#define __TEXT_H

#include "fonts.h"
#include "esp_partition.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"
#include "ff.h"
#include "lcd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "convert.h"


/* function declaration */
void text_show_font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint8_t mode, uint16_t color);
void text_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, uint8_t size, uint8_t mode, uint16_t color);
void text_show_string_middle(uint16_t x, uint16_t y, char *str, uint8_t size, uint16_t width, uint16_t color);

#endif
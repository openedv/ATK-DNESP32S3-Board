/**
 ****************************************************************************************************
 * @file        png.h
 * @author      ALIENTEK
 * @brief       PNG code
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

#ifndef __PNG_H
#define __PNG_H

#include <stdint.h>
#include <stdbool.h>
#include "pngle.h"
#include "piclib.h"
#include "ff.h"
#include "esp_log.h"


/* function declaration */
void png_init(pngle_t *pngle, uint32_t w, uint32_t h);
void png_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]);
void png_finish(pngle_t *pngle);
TickType_t png_decode(const char *filename, int width, int height);

#endif

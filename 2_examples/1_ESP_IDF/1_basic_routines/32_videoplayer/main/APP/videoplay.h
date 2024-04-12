/**
 ****************************************************************************************************
 * @file        videoplay.h
 * @author      ALIENTEK
 * @brief       VIDERPLAY code
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

#ifndef __VIDEOPLAYER_H
#define __VIDEOPLAYER_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "avi.h"
#include "xl9555.h"
#include "lcd.h"
#include "ff.h"
#include "esptim.h"
#include "mjpeg.h"
#include "es8388.h"
#include "driver/i2s.h"
#include "i2s.h"
#include "text.h"


#define AVI_VIDEO_BUF_SIZE      (60 * 1024)

/* function declaration */
void video_play(void);
uint8_t video_seek(FIL *favi, AVI_INFO *aviinfo, uint8_t *mbuf);
#endif
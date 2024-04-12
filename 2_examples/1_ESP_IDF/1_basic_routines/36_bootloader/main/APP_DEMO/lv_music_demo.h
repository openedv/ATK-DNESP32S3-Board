/**
 ****************************************************************************************************
 * @file        lv_music_demo.h
 * @author      ALIENTEK
 * @brief       LVGL MUSIC Demo code
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


#ifndef __LV_MUSIC_H
#define __LV_MUSIC_H

#include "lvgl.h"
#include "ff.h"
#include "lcd.h"
#include "app_ui.h"
#include "exfuns.h"
#include "ff.h"
#include "driver/i2s.h"
#include "driver/i2s_std.h"
#include "i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "lvgl_demo.h"
#include "sdmmc_cmd.h"


#define WAV_TX_BUFSIZE    8192

typedef struct
{
    uint32_t ChunkID;
    uint32_t ChunkSize;
    uint32_t Format;
}ChunkRIFF;     /* RIFF块 */

typedef struct
{
    uint32_t ChunkID;
    uint32_t ChunkSize;
    uint16_t AudioFormat;
    uint16_t NumOfChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
//    uint16_t ByteExtraData;
}ChunkFMT;      /* fmt */

typedef struct 
{
    uint32_t ChunkID;
    uint32_t ChunkSize;
    uint32_t NumOfSamples;
}ChunkFACT;     /* fact */

typedef struct 
{
    uint32_t ChunkID;
    uint32_t ChunkSize;
}ChunkLIST;     /* LIST */

typedef struct
{
    uint32_t ChunkID;
    uint32_t ChunkSize;
}ChunkDATA;     /* data */

typedef struct
{ 
    ChunkRIFF riff;
    ChunkFMT fmt;
//    ChunkFACT fact;
    ChunkDATA data;
}__WaveHeader;  /* wav */

typedef struct
{ 
    uint16_t audioformat;
    uint16_t nchannels;
    uint16_t blockalign;
    uint32_t datasize;

    uint32_t totsec;
    uint32_t cursec;

    uint32_t bitrate;
    uint32_t samplerate;
    uint16_t bps;

    uint32_t datastart;
}__wavctrl;

typedef struct
{
    uint8_t *tbuf;
    FIL *file;

    uint8_t status;
}__audiodev;

enum MUSIC_STATE
{
    MUSIC_NULL,
    MUSIC_PAUSE,
    MUSIC_PLAY,
    MUSIC_NEXT,
    MUSIC_PREV
};

/* function declaration */
void lv_music_demo(void);

#endif
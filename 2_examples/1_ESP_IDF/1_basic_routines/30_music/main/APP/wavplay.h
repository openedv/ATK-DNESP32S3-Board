/**
 ****************************************************************************************************
 * @file        wavplay.h
 * @author      ALIENTEK
 * @brief       WAVPLAY code
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

#ifndef __WAVPLAY_H
#define __WAVPLAY_H

#include "audioplay.h"
#include "ff.h"
#include "es8388.h"
#include "xl9555.h"
#include "driver/i2s.h"
#include "driver/i2s_std.h"
#include "led.h"
#include "i2s.h"


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


#define I2S_NUM                 (I2S_NUM_0)                         /* I2S port */
#define I2S_BCK_IO              (GPIO_NUM_46)                       /* ES8388_SCLK */
#define I2S_WS_IO               (GPIO_NUM_9)                        /* ES8388_LRCK */
#define I2S_DO_IO               (GPIO_NUM_10)                       /* ES8388_SDOUT */
#define I2S_DI_IO               (GPIO_NUM_14)                       /* ES8388_SDIN */
#define IS2_MCLK_IO             (GPIO_NUM_3)                        /* ES8388_MCLK */
#define SAMPLE_RATE             (44100)                             /* sample rate */

/* function declaration */
uint8_t wav_decode_init(uint8_t *fname, __wavctrl *wavx);
uint8_t wav_play_song(uint8_t *fname);
void wavplay_i2s_init(int samplerate,int bits_sample);
size_t i2s_tx_write(uint8_t *buffer, uint32_t frame_size);

#endif

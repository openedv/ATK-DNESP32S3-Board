/**
 ****************************************************************************************************
 * @file        audioplay.h
 * @author      ALIENTEK
 * @brief       AUDIOPLAY code
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

#ifndef __AUDIOPLAY_H
#define __AUDIOPLAY_H

#include "ff.h"
#include "wavplay.h"
#include "exfuns.h"
#include "i2s.h"
#include "lcd.h"
#include "text.h"


typedef struct
{
    uint8_t *tbuf;
    FIL *file;                              /* Audio file pointer */

    uint8_t status;
}__audiodev;

extern __audiodev g_audiodev;

/* function declaration */
void wav_sai_dma_callback(void);
void audio_start(void);
void audio_stop(void);
uint16_t audio_get_tnum(uint8_t *path);
void audio_index_show(uint16_t index, uint16_t total);
void audio_msg_show(uint32_t totsec, uint32_t cursec, uint32_t bitrate);
void audio_play(void);
uint8_t audio_play_song(uint8_t *fname);

#endif

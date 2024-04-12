/**
 ****************************************************************************************************
 * @file        recorder.h
 * @author      ALIENTEK
 * @brief       RECORDER code
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

#ifndef __RECORDER_H
#define __RECORDER_H

#include "audioplay.h"
#include "ff.h"
#include "es8388.h"
#include "xl9555.h"
#include "driver/i2s.h"
#include "led.h"
#include "wavplay.h"
#include "text.h"
#include "i2s.h"


/* function declaration */
void recoder_enter_rec_mode(void);
void recoder_wav_init(__WaveHeader *wavhead);
void recoder_msg_show(uint32_t tsec, uint32_t kbps);
void recoder_remindmsg_show(uint8_t mode);
void recoder_new_pathname(uint8_t *pname);
void wav_recorder(void);
#endif

/**
 ****************************************************************************************************
 * @file        recorder.c
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

#include "recorder.h"



uint32_t g_wav_size;    /* wav file data size */

uint8_t g_rec_sta = 0;  /**
                         * Recording status
                         * [7]:0: recording not enabled; 1: Recording has been enabled
                         * [6:1]:reserve
                         * [0]:0: recording in progress; 1: Pause recording
                         */

/**
 * @brief       Enter PCM recording mode
 * @param       None
 * @retval      None
 */
void recoder_enter_rec_mode(void)
{
    es8388_adda_cfg(0, 1);
    es8388_input_cfg(0);
    es8388_mic_gain(8);
    es8388_alc_ctrl(3, 4, 4);
    es8388_output_cfg(0, 0);
    es8388_spkvol_set(0);
    es8388_sai_cfg(0, 3);
    i2s_set_samplerate_bits_sample(SAMPLE_RATE,I2S_BITS_PER_SAMPLE_16BIT);
    i2s_trx_start();
    recoder_remindmsg_show(0);
}

/**
 * @brief       Enter PCM playback mode
 * @param       None
 * @retval      None
 */
void recoder_enter_play_mode(void)
{
    es8388_adda_cfg(1, 0);
    es8388_output_cfg(1, 1);
    es8388_spkvol_set(28);
    i2s_trx_stop();
    recoder_remindmsg_show(1);
}

/**
 * @brief       recoder wav init
 * @param       wavhead : Wav file header pointer
 * @retval      None
 */
void recoder_wav_init(__WaveHeader *wavhead)
{
    wavhead->riff.ChunkID = 0x46464952;
    wavhead->riff.ChunkSize = 0;
    wavhead->riff.Format = 0x45564157;
    wavhead->fmt.ChunkID = 0x20746D66;
    wavhead->fmt.ChunkSize = 16;
    wavhead->fmt.AudioFormat = 0x01;
    wavhead->fmt.NumOfChannels = 2;
    wavhead->fmt.SampleRate = SAMPLE_RATE;
    wavhead->fmt.ByteRate = wavhead->fmt.SampleRate * 4;
    wavhead->fmt.BlockAlign = 4;
    wavhead->fmt.BitsPerSample = 16;
    wavhead->data.ChunkID = 0x61746164;
    wavhead->data.ChunkSize = 0;
}

/**
 * @brief       Display recording time and bit rate
 * @param       tsec : time
 * @param       kbps : Code rate
 * @retval      None
 */
void recoder_msg_show(uint32_t tsec, uint32_t kbps)
{
    lcd_show_string(30, 210, 200, 16, 16, "TIME:", RED);
    lcd_show_num(30 + 40, 210, tsec / 60, 2, 16, RED);
    lcd_show_char(30 + 56, 210, ':', 16, 0, RED);
    lcd_show_num(30 + 64, 210, tsec % 60, 2, 16, RED);

    lcd_show_string(140, 210, 200, 16, 16, "KPBS:", RED);
    lcd_show_num(140 + 40, 210, kbps / 1000, 4, 16, RED);
}

/**
 * @brief       Prompt information
 * @param       mode : Working mode
 *   @arg       0: Recording mode
 *   @arg       1: Playback mode
 * @retval      None
 */
void recoder_remindmsg_show(uint8_t mode)
{
    lcd_fill(30, 120, lcd_self.width, 180, WHITE);

    if (mode == 0)  /* recording mode */
    {
        lcd_show_string(30, 120, 200, 16, 16, "KEY0:REC/PAUSE", BLUE);
        lcd_show_string(30, 140, 200, 16, 16, "KEY2:STOP&SAVE", BLUE);
        lcd_show_string(30, 160, 200, 16, 16, "KEY3:PLAY", BLUE);
    }
    else            /* playback mode */
    {
        lcd_show_string(30, 120, 200, 16, 16, "KEY0:STOP Play", BLUE);
        lcd_show_string(30, 140, 200, 16, 16, "KEY3:PLAY/PAUSE", BLUE);
    }
}

/**
 * @brief       Obtain file name through time
 * @param       pname : File path
 * @retval      None
 */
void recoder_new_pathname(uint8_t *pname)
{
    uint8_t res;
    uint16_t index = 0;
    FIL *ftemp;
    ftemp = (FIL *)malloc(sizeof(FIL));

    if (ftemp == NULL) 
    {
        return;
    }

    while (index < 0xFFFF)
    {
        sprintf((char *)pname, "0:RECORDER/REC%05d.wav", index);
        res = f_open(ftemp, (const TCHAR *)pname, FA_READ);

        if (res == FR_NO_FILE)
        {
            break;
        }

        index++;
    }

    free(ftemp);
}

/**
 * @brief       Recording
 * @param       None
 * @retval      None
 */
void wav_recorder(void)
{
    uint8_t res;
    uint8_t key;
    uint8_t rval = 0;
    uint32_t bw;
    
    __WaveHeader *wavhead = 0;
    FF_DIR recdir;
    FIL *f_rec;
    
    uint8_t *pdatabuf;
    uint8_t *pname = 0;
    uint32_t recsec = 0;
    uint8_t timecnt = 0;
    uint16_t bytes_read = 0;

    while (f_opendir(&recdir, "0:/RECORDER"))
    {
        lcd_show_string(30, 230, 240, 16, 16, "RECORDER folder error!", RED);
        vTaskDelay(200);
        lcd_fill(30, 230, 240, 246, WHITE);
        vTaskDelay(200);
        f_mkdir("0:/RECORDER");
    }

    pdatabuf = malloc(1024 * 10);
    f_rec = (FIL*)malloc(sizeof(FIL));
    wavhead = (__WaveHeader *)malloc(sizeof(__WaveHeader));
    pname = malloc(30);

    if (!f_rec || !wavhead || !pname || !pdatabuf)
    {
        rval = 1;
    }

    if (rval == 0)
    {
        recoder_enter_rec_mode();
        pname[0] = 0;

        while (rval == 0)
        {
            key = xl9555_key_scan(0);

            switch (key)
            {
                case KEY2_PRES:                                     /* STOP&SAVE */
                    if (g_rec_sta & 0x80)                           /* There are audio files available */
                    {
                        g_rec_sta = 0;                              /* turn off recording */
                        wavhead->riff.ChunkSize = g_wav_size + 36;
                        wavhead->data.ChunkSize = g_wav_size;
                        f_lseek(f_rec, 0);
                        f_write(f_rec, (const void *)wavhead, sizeof(__WaveHeader), &bw);
                        f_close(f_rec);
                        g_wav_size = 0;
                    }

                    g_rec_sta = 0;
                    recsec = 0;
                    LED(1);
                    lcd_fill(30, 190, lcd_self.width, lcd_self.height, WHITE);
                    break;
 
                case KEY0_PRES:     /* REC/PAUSE */
                    if (g_rec_sta & 0x01)
                    {
                        g_rec_sta &= 0xFE;
                    }
                    else if (g_rec_sta & 0x80)
                    {
                        g_rec_sta |= 0x01;
                    }
                    else
                    {
                        recsec = 0;
                        recoder_new_pathname(pname);
                        text_show_string(30, 190, lcd_self.width, 16, "录制:", 16, 0, RED);
                        text_show_string(30 + 40, 190, lcd_self.width, 16, (char *)pname + 11, 16, 0, RED);
                        recoder_wav_init(wavhead);
                        res = f_open(f_rec, (const TCHAR*)pname, FA_CREATE_ALWAYS | FA_WRITE);

                        if (res)
                        {
                            g_rec_sta = 0;
                            rval = 0xFE;
                        }
                        else
                        {
                            res = f_write(f_rec, (const void *)wavhead, sizeof(__WaveHeader), (UINT*)&bw);
                            recoder_msg_show(0, 0);
                            g_rec_sta |= 0x80;
                        }
                    }

                    if (g_rec_sta & 0x01)
                    {
                        LED(0);
                    }
                    else 
                    {
                        LED(1);
                    }
                    break;

                case KEY3_PRES:                         /* Play the latest recording */
                    if (g_rec_sta != 0x80)
                    {
                        if (pname[0])
                        {
                            text_show_string(30, 190, lcd_self.width, 16, "播放:", 16, 0, RED);
                            text_show_string(30 + 40, 190, lcd_self.width, 16, (char *)pname + 11, 16, 0, RED);
                            recoder_enter_play_mode();
                            audio_play_song(pname);
                            lcd_fill(30, 190, lcd_self.width, lcd_self.height, WHITE);
                            recoder_enter_rec_mode();
                        }
                    }
                    break;
            }

            if ((g_rec_sta & 0x80) == 0x80)
            {
                if ((g_rec_sta & 0x01) == 0x00)
                {
                    bytes_read = i2s_rx_read((uint8_t *)pdatabuf, 1024 * 10);
                    res = f_write(f_rec, pdatabuf, bytes_read, (UINT*)&bw);

                    if (res)
                    {
                        printf("write error:%d\r\n", res);
                    }

                    g_wav_size += bytes_read;
                }
            }
            else 
            {
                vTaskDelay(1);
            }

            timecnt++;

            if ((timecnt % 20) == 0)
            {
                LED_TOGGLE();
            }


            if (recsec != (g_wav_size / wavhead->fmt.ByteRate))
            {
                recsec = g_wav_size / wavhead->fmt.ByteRate;
                recoder_msg_show(recsec, wavhead->fmt.SampleRate * wavhead->fmt.NumOfChannels * wavhead->fmt.BitsPerSample);
            }
        }
    }

    free(pdatabuf);
    free(f_rec);
    free(wavhead);
    free(pname);
}

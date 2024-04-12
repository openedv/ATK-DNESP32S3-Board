/**
 ****************************************************************************************************
 * @file        wavplay.c
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

#include "wavplay.h"
/*FreeRTOS*********************************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/******************************************************************************************************/
/*FreeRTOS Configuration*/

/* MUSIC Task Configuration
 * Including: task handle, task priority, stack size, creation task
 */
#define MUSIC_PRIO      4                   /* task priority */
#define MUSIC_STK_SIZE  5*1024              /* task stack size */
TaskHandle_t            MUSICTask_Handler;  /* task handle */
void music(void *pvParameters);             /* Task function */

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;

/******************************************************************************************************/

__wavctrl wavctrl;


esp_err_t i2s_play_end = ESP_FAIL;
esp_err_t i2s_play_next_prev = ESP_FAIL;

/**
 * @brief       WAV file parsing
 * @param       fname : File path
 * @param       wavx  : Structure pointer
 * @retval      0:Open file successfully
 *              1:fail to open file
 *              2:Non WAV files
 *              3:DATA region not found
 */
uint8_t wav_decode_init(uint8_t *fname, __wavctrl *wavx)
{
    FIL *ftemp;
    uint8_t *buf; 
    uint32_t br = 0;
    uint8_t res = 0;

    ChunkRIFF *riff;
    ChunkFMT *fmt;
    ChunkFACT *fact;
    ChunkDATA *data;
    
    ftemp = (FIL*)malloc(sizeof(FIL));
    buf = malloc(512);
    
    if (ftemp && buf)
    {
        res = f_open(ftemp, (TCHAR*)fname, FA_READ);            /* open file */
        
        if (res == FR_OK)
        {
            f_read(ftemp, buf, 512, (UINT *)&br);               /* read 512 byte */
            riff = (ChunkRIFF *)buf;
            
            if (riff->Format == 0x45564157)                     /* if it is a WAV file */
            {
                fmt = (ChunkFMT *)(buf + 12);
                fact = (ChunkFACT *)(buf + 12 + 8 + fmt->ChunkSize);
                
                if (fact->ChunkID == 0x74636166 || fact->ChunkID == 0x5453494C)
                {
                    wavx->datastart = 12 + 8 + fmt->ChunkSize + 8 + fact->ChunkSize;
                }
                else
                {
                    wavx->datastart = 12 + 8 + fmt->ChunkSize;
                }
                
                data = (ChunkDATA *)(buf + wavx->datastart);
                
                if (data->ChunkID == 0x61746164)                /* parsed successfully */
                {
                    wavx->audioformat = fmt->AudioFormat;       /* audio Format */
                    wavx->nchannels = fmt->NumOfChannels;       /* number of channels */
                    wavx->samplerate = fmt->SampleRate;         /* sampling rate */
                    wavx->bitrate = fmt->ByteRate * 8;
                    wavx->blockalign = fmt->BlockAlign;
                    wavx->bps = fmt->BitsPerSample;
                    
                    wavx->datasize = data->ChunkSize;
                    wavx->datastart = wavx->datastart + 8;
                     
                    printf("wavx->audioformat:%d\r\n", wavx->audioformat);
                    printf("wavx->nchannels:%d\r\n", wavx->nchannels);
                    printf("wavx->samplerate:%ld\r\n", wavx->samplerate);
                    printf("wavx->bitrate:%ld\r\n", wavx->bitrate);
                    printf("wavx->blockalign:%d\r\n", wavx->blockalign);
                    printf("wavx->bps:%d\r\n", wavx->bps);
                    printf("wavx->datasize:%ld\r\n", wavx->datasize);
                    printf("wavx->datastart:%ld\r\n", wavx->datastart);  
                }
                else
                {
                    res = 3;
                }
            }
            else
            {
                res = 2;
            }
        }
        else
        {
            res = 1;
        }
    }
    
    f_close(ftemp);
    free(ftemp);
    free(buf); 
    
    return 0;
}

/**
 * @brief       Get the current playback time
 * @param       fx    : field name pointer
 * @param       wavx  : Structure pointer
 * @retval      None
 */
void wav_get_curtime(FIL *fx, __wavctrl *wavx)
{
    long long fpos;

    wavx->totsec = wavx->datasize / (wavx->bitrate / 8);
    fpos = fx->fptr-wavx->datastart;
    wavx->cursec = fpos * wavx->totsec / wavx->datasize;
}

/**
 * @brief       music task
 * @param       pvParameters : None
 * @retval      None
 */
void music(void *pvParameters)
{
    pvParameters = pvParameters;
    uint32_t nr;
    FSIZE_t res = 0;
    uint8_t n = 0;
    volatile long long int i2s_table_size = 0;

    while(1)
    {
        if ((g_audiodev.status & 0x0F) == 0x03)
        {
            f_lseek(g_audiodev.file, n ? res : wavctrl.datastart);                          /* Skip file header */

            for (uint16_t readTimes = 0; readTimes < (wavctrl.datasize / WAV_TX_BUFSIZE); readTimes++)
            {
                if ((g_audiodev.status & 0x0F) == 0x00)                                     /* Pause playback */
                {
                    res = f_tell(g_audiodev.file);                                          /* Record pause position */
                    n = 1;
                    break;
                }

                if (i2s_table_size >= wavctrl.datasize || i2s_play_next_prev == ESP_OK)
                {
                    n = 0;
                    i2s_table_size = 0;
                    i2s_play_end = ESP_OK;
                    audio_stop();
                    vTaskDelay(10);
                    break;
                }

                f_read(g_audiodev.file,g_audiodev.tbuf, WAV_TX_BUFSIZE, (UINT*)&nr);
                i2s_table_size = i2s_table_size + i2s_tx_write(g_audiodev.tbuf, WAV_TX_BUFSIZE);
                vTaskDelay(1);
            }

        }
        else
        {
            vTaskDelay(10);
        }
    }
}

/**
 * @brief       Play WAV files
 * @param       fname : File path
 * @retval      KEY0_PRES   : error
 *              KEY1_PRES   : fail to open file
 *              Other       : Non WAV files
 */
uint8_t wav_play_song(uint8_t *fname)
{
    uint8_t key = 0;
    uint8_t t = 0;
    uint8_t res;
    i2s_play_end = ESP_FAIL;
    i2s_play_next_prev = ESP_FAIL;
    g_audiodev.file = (FIL*)malloc(sizeof(FIL));
    g_audiodev.tbuf = malloc(WAV_TX_BUFSIZE);
    
    if (g_audiodev.file || g_audiodev.tbuf)
    {
        res = wav_decode_init(fname, &wavctrl);

        if (res == 0)                                                           /* Parsed successfully */
        {
            if (wavctrl.bps == 16)
            {
                es8388_sai_cfg(0, 3);
                i2s_set_samplerate_bits_sample(wavctrl.samplerate,I2S_BITS_PER_SAMPLE_16BIT);
            }
            else if (wavctrl.bps == 24)
            {
                es8388_sai_cfg(0, 0);
                i2s_set_samplerate_bits_sample(wavctrl.samplerate,I2S_BITS_PER_SAMPLE_24BIT);
             }

            audio_stop();

            if (MUSICTask_Handler == NULL)
            {
                taskENTER_CRITICAL(&my_spinlock);

                xTaskCreatePinnedToCore((TaskFunction_t )music,
                                        (const char*    )"music",
                                        (uint16_t       )MUSIC_STK_SIZE,
                                        (void*          )NULL,
                                        (UBaseType_t    )MUSIC_PRIO,
                                        (TaskHandle_t*  )&MUSICTask_Handler,
                                        (BaseType_t     ) 0);
                taskEXIT_CRITICAL(&my_spinlock);
            }

            res = f_open(g_audiodev.file, (TCHAR*)fname, FA_READ);              /* open file */

            if (res == 0)
            {
                audio_start();
                vTaskDelay(100);
                audio_start();
                vTaskDelay(100);

                while (res == 0)
                { 
                    while (1)
                    {
                        if (i2s_play_end == ESP_OK)
                        {
                            res = KEY0_PRES;
                            break;
                        }

                        key = xl9555_key_scan(0);
                        
                        if (key == KEY3_PRES)
                        {
                            if ((g_audiodev.status & 0x0F) == 0x03)
                            {
                                audio_stop();
                                vTaskDelay(100);
                            }
                            else if ((g_audiodev.status & 0x0F) == 0x00)
                            {
                                audio_start();
                                vTaskDelay(100);
                            }
                        }
                        
                        if (key == KEY2_PRES || key == KEY0_PRES)
                        {
                            i2s_play_next_prev = ESP_OK;
                            vTaskDelay(100);
                            res = KEY0_PRES;
                            break;
                        }
                        
                        if ((g_audiodev.status & 0x0F) == 0x03)
                        {
                            wav_get_curtime(g_audiodev.file, &wavctrl);
                            audio_msg_show(wavctrl.totsec, wavctrl.cursec, wavctrl.bitrate);
                        }
                        
                        t++;
                        if (t == 20)
                        {
                            t = 0 ;
                            LED_TOGGLE();
                        }
                        
                        if ((g_audiodev.status & 0x01) == 0)
                        {
                            vTaskDelay(10);
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (key == KEY2_PRES || key == KEY0_PRES)
                    {
                        res = key;
                        break;
                    }
                }
                audio_stop();
            }
            else
            {
                res = 0xFF;
            }
        }
        else
        {
            res = 0xFF;
        }
    }
    else
    {
        res = 0xFF;
    }
    
    free(g_audiodev.tbuf);
    free(g_audiodev.file);
    
    return res;
}

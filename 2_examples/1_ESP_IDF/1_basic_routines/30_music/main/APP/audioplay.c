/**
 ****************************************************************************************************
 * @file        audioplay.c
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

#include "audioplay.h"


__audiodev g_audiodev;          /* audio Handle */

/**
 * @brief       Start audio playback
 * @param       None
 * @retval      None
 */
void audio_start(void)
{
    g_audiodev.status = 3 << 0;
    i2s_trx_start();
}

/**
 * @brief       Stop audio playback
 * @param       None
 * @retval      None
 */
void audio_stop(void)
{
    g_audiodev.status = 0;
    i2s_trx_stop();
}

/**
 * @brief       Obtain the total number of target files in the path path
 * @param       path : path
 * @retval      Total number of valid files
 */
uint16_t audio_get_tnum(uint8_t *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;
    FILINFO *tfileinfo;
    
    tfileinfo = (FILINFO*)malloc(sizeof(FILINFO));
    
    res = f_opendir(&tdir, (const TCHAR*)path);
    
    if ((res == FR_OK) && tfileinfo)
    {
        while (1)
        {
            res = f_readdir(&tdir, tfileinfo);
            
            if ((res != FR_OK) || (tfileinfo->fname[0] == 0))
            {
                break;
            }

            res = exfuns_file_type(tfileinfo->fname);
            
            if ((res & 0xF0) == 0x40)
            {
                rval++; /* accumulate */
            }
        }
    }
    
    free(tfileinfo);
    
    return rval;
}

/**
 * @brief       Display track index
 * @param       index : Current index
 * @param       total : Total number of files
 * @retval      None
 */
void audio_index_show(uint16_t index, uint16_t total)
{
    lcd_show_num(30 + 0, 230, index, 3, 16, RED);
    lcd_show_char(30 + 24, 230, '/', 16, 0, RED);
    lcd_show_num(30 + 32, 230, total, 3, 16, RED);
}

/**
 * @brief       Display playback time and bit rate information
 * @param       totsec : Total time length of audio files
 * @param       cursec : Current playback time
 * @param       bitrate: Bit rate
 * @retval      None
 */
void audio_msg_show(uint32_t totsec, uint32_t cursec, uint32_t bitrate)
{
    static uint16_t playtime = 0xFFFF;
    
    if (playtime != cursec)
    {
        playtime = cursec;
        
        lcd_show_xnum(30, 210, playtime / 60, 2, 16, 0X80, RED);
        lcd_show_char(30 + 16, 210, ':', 16, 0, RED);
        lcd_show_xnum(30 + 24, 210, playtime % 60, 2, 16, 0X80, RED);
        lcd_show_char(30 + 40, 210, '/', 16, 0, RED);
        
        lcd_show_xnum(30 + 48, 210, totsec / 60, 2, 16, 0X80, RED);
        lcd_show_char(30 + 64, 210, ':', 16, 0, RED);
        lcd_show_xnum(30 + 72, 210, totsec % 60, 2, 16, 0X80, RED);
        
        lcd_show_num(30 + 110, 210, bitrate / 1000, 4, 16, RED);
        lcd_show_string(30 + 110 + 32 , 210, 200, 16, 16, "Kbps", RED);
    }
}

/**
 * @brief       Audio play
 * @param       None
 * @retval      None
 */
void audio_play(void)
{
    uint8_t res;
    FF_DIR wavdir;
    FILINFO *wavfileinfo;
    uint8_t *pname;
    uint16_t totwavnum;
    uint16_t curindex;
    uint8_t key;
    uint32_t temp;
    uint32_t *wavoffsettbl;

    es8388_adda_cfg(1, 0);
    es8388_output_cfg(1, 1);

    while (f_opendir(&wavdir, "0:/MUSIC"))
    {
        text_show_string(30, 190, 240, 16, "MUSIC文件夹错误!", 16, 0, BLUE);
        vTaskDelay(200);
        lcd_fill(30, 190, 240, 206, WHITE);
        vTaskDelay(200);
    }

    totwavnum = audio_get_tnum((uint8_t *)"0:/MUSIC");          /* total number of valid files */
    
    while (totwavnum == NULL)
    {
        text_show_string(30, 190, 240, 16, "没有音乐文件!", 16, 0, BLUE);
        vTaskDelay(200);
        lcd_fill(30, 190, 240, 146, WHITE);
        vTaskDelay(200);
    }
    
    wavfileinfo = (FILINFO*)malloc(sizeof(FILINFO));
    pname = malloc(255 * 2 + 1);
    wavoffsettbl = malloc(4 * totwavnum);
    
    while (!wavfileinfo || !pname || !wavoffsettbl)
    {
        text_show_string(30, 190, 240, 16, "内存分配失败!", 16, 0, BLUE);
        vTaskDelay(200);
        lcd_fill(30, 190, 240, 146, WHITE);
        vTaskDelay(200);
    }
    
    /* record index */
    res = f_opendir(&wavdir, "0:/MUSIC");
    
    if (res == FR_OK)
    {
        curindex = 0;                                           /* the current index is 0 */
        
        while (1)
        {
            temp = wavdir.dptr;                                 /* record current index */

            res = f_readdir(&wavdir, wavfileinfo);              /* read a file from the directory */
            
            if ((res != FR_OK) || (wavfileinfo->fname[0] == 0))
            {
                break;                                          /* error/at the end, exit */
            }

            res = exfuns_file_type(wavfileinfo->fname);
            
            if ((res & 0xF0) == 0x40)
            {
                wavoffsettbl[curindex] = temp;                   /* record index */
                curindex++;
            }
        }
    }
    
    curindex = 0;                                               /* display from 0 */
    res = f_opendir(&wavdir, (const TCHAR*)"0:/MUSIC");
    
    while (res == FR_OK)                                        /* open successfully */
    {
        dir_sdi(&wavdir, wavoffsettbl[curindex]);               /* change the current directory index */
        res = f_readdir(&wavdir, wavfileinfo);                  /* read a file from the directory */
        
        if ((res != FR_OK) || (wavfileinfo->fname[0] == 0))
        {
            break;                                              /* error/at the end, exit */
        }
        
        strcpy((char *)pname, "0:/MUSIC/");
        strcat((char *)pname, (const char *)wavfileinfo->fname);
        lcd_fill(30, 190, lcd_self.width - 1, 190 + 16, WHITE);
        audio_index_show(curindex + 1, totwavnum);
        text_show_string(30, 190, lcd_self.width - 60, 16, (char *)wavfileinfo->fname, 16, 0, BLUE);
        key = audio_play_song(pname);
        
        if (key == KEY2_PRES)                                   /* previous song */
        {
            if (curindex)
            {
                curindex--;
            }
            else
            {
                curindex = totwavnum - 1;
            }
        }
        else if (key == KEY0_PRES)                              /* next song */
        {
            curindex++;

            if (curindex >= totwavnum)
            {
                curindex = 0;
            }
        }
        else
        {
            break;
        }
    }

    free(wavfileinfo);
    free(pname);
    free(wavoffsettbl);
}

/**
 * @brief       Play audio files
 * @param       fname : File path
 * @retval      Key values
 *   @arg       KEY0_PRES : next song.
 *   @arg       KEY2_PRES : previous song.
 *   @arg       Other     : error
 */
uint8_t audio_play_song(uint8_t *fname)
{
    uint8_t res;  
    
    res = exfuns_file_type((char *)fname); 

    switch (res)
    {
        case T_WAV:
            res = wav_play_song(fname);
            break;
        case T_MP3:
            /* Self implementation */
            break;

        default:            /* Other files, automatically jump to the next track */
            printf("can't play:%s\r\n", fname);
            res = KEY0_PRES;
            break;
    }
    return res;
}

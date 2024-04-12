/**
 ****************************************************************************************************
 * @file        videoplay.c
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

#include "videoplay.h"


extern uint8_t frameup;

/**
 * @brief       Obtain the number of valid video files in the specified path
 * @param       path: File Path
 * @retval      Number of valid video files
 */
static uint16_t video_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;
    FILINFO *tfileinfo;
    
    tfileinfo = (FILINFO *)malloc(sizeof(FILINFO));
    res = (uint8_t)f_opendir(&tdir, (const TCHAR *)path);
    
    if ((res == 0) && tfileinfo)
    {
        while (1)
        {
            res = (uint8_t)f_readdir(&tdir, tfileinfo);
            
            if ((res != 0) || (tfileinfo->fname[0] == 0))
            {
                break;
            }
            
            res = exfuns_file_type(tfileinfo->fname);
            
            if ((res & 0xF0) == 0x60)
            {
                rval++;
            }
        }
    }
    
    free(tfileinfo);
    
    return rval;
}

/**
 * @brief       Display basic video information
 * @param       name : Video Name
 * @param       index: Current index
 * @param       total: Total number of files
 * @retval      None
 */
static void video_bmsg_show(uint8_t *name, uint16_t index, uint16_t total)
{
    uint8_t *buf;
    
    buf = malloc(100);
    
    sprintf((char *)buf, "文件名:%s", name);
    
    sprintf((char *)buf, "索引:%d/%d", index, total);
    
    free(buf);
}

/**
 * @brief       Display relevant information about the current video file
 * @param       aviinfo: AVI control structure
 * @retval      None
 */
static void video_info_show(AVI_INFO *aviinfo)
{
    uint8_t *buf;
    
    buf = malloc(100);
    
    sprintf((char *)buf, "声道数:%d,采样率:%ld", aviinfo->Channels, aviinfo->SampleRate * 10);
    text_show_string(10, 10, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);
    
    sprintf((char *)buf, "帧率:%ld帧", 1000 / (aviinfo->SecPerFrame / 1000));
    text_show_string(10, 30, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);
    
    free(buf);
}

/**
 * @brief       Display current playback time
 * @param       favi   : The currently playing video file
 * @param       aviinfo: AVI control structure
 * @retval      None
 */
void video_time_show(FIL *favi, AVI_INFO *aviinfo)
{
    static uint32_t oldsec;
    
    uint8_t *buf;
    
    uint32_t totsec = 0;
    uint32_t cursec;
    
    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame; 
    totsec /= 1000; 
    cursec = ((double)favi->fptr / favi->obj.objsize) * totsec;
    
    if (oldsec != cursec)
    {
        buf = malloc(100);
        oldsec = cursec;
        
        sprintf((char *)buf, "Play time:%02ld:%02ld:%02ld/%02ld:%02ld:%02ld", cursec / 3600, (cursec % 3600) / 60, cursec % 60, totsec / 3600, (totsec % 3600) / 60, totsec % 60);
        text_show_string(10, 50, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);
        
        free(buf);
    }
}

/**
 * @brief       Play MJPEG videos
 * @param       pname: Video file name
 * @retval      Key values
 *              KEY0_PRES: Previous video
 *              WKUP_PRES: Next video
 *              Other    : error code
 */
static uint8_t video_play_mjpeg(uint8_t *pname)
{
    uint8_t *framebuf;
    uint8_t *pbuf;
    uint8_t res = 0;
    uint16_t offset;
    uint32_t nr;
    uint8_t key;
    FIL *favi;

    framebuf = (uint8_t *)malloc(AVI_VIDEO_BUF_SIZE);
    favi = (FIL *)malloc(sizeof(FIL));

    if ((framebuf == NULL) || (favi == NULL))
    {
        printf("memory error!\r\n");
        res = 0xFF;
    }

    memset(framebuf, 0, AVI_VIDEO_BUF_SIZE);
    
    while (res == 0)
    {
        res = (uint8_t)f_open(favi, (const TCHAR *)pname, FA_READ);
        
        if (res == 0)
        {
            pbuf = framebuf;
            res = (uint8_t)f_read(favi, pbuf, AVI_VIDEO_BUF_SIZE, (UINT*)&nr);
            
            if (res != 0)
            {
                printf("fread error:%d\r\n", res);
                break;
            }
            
            res = avi_init(pbuf, AVI_VIDEO_BUF_SIZE);   /* AVI data analysis */
            
            if (res != 0)
            {
                printf("avi error:%d\r\n", res);
                break;
            }
            
            video_info_show(&g_avix);
            esptim_int_init(g_avix.SecPerFrame / 1000, 1000);
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");                                                   /* movi ID */
            avi_get_streaminfo(pbuf + offset + 4);
            f_lseek(favi, offset + 12);
            res = mjpegdec_init((lcd_self.width - g_avix.Width) / 2, 110 + (lcd_self.height - 110 - g_avix.Height) / 2);
            Windows_Width = g_avix.Width;
            Windows_Height = g_avix.Height;

            if (g_avix.SampleRate)
            {
                printf("g_avix.SampleRate:%ld\r\n",g_avix.SampleRate);
                es8388_sai_cfg(0, 3);
                i2s_set_samplerate_bits_sample(g_avix.SampleRate,I2S_BITS_PER_SAMPLE_16BIT);
                i2s_start(I2S_NUM);
            }

            while (1)
            {
                if (g_avix.StreamID == AVI_VIDS_FLAG)                       /* Video streaming */
                {
                    pbuf = framebuf;
                    f_read(favi, pbuf, g_avix.StreamSize + 8, (UINT*)&nr);
                    res = mjpegdec_decode(pbuf, g_avix.StreamSize);
                    
                    if (res != 0)
                    {
                        printf("decode error!\r\n");
                    }

                    while (frameup == 0);
                    frameup = 0;
                }
                else
                {
                    video_time_show(favi, &g_avix);
                    f_read(favi, framebuf, g_avix.StreamSize + 8, &nr);
                    pbuf = framebuf;
                    i2s_tx_write(framebuf, g_avix.StreamSize);
                }
                
                key = xl9555_key_scan(0);

                if (key == KEY0_PRES || key == KEY2_PRES)                   /* Play the next/previous video */
                {
                    res = key;
                    break;
                }
                else if (key == KEY1_PRES || key == KEY3_PRES)
                {
                    i2s_stop(I2S_NUM);                                      /* stop audio */
                    video_seek(favi, &g_avix, framebuf);
                    pbuf = framebuf;
                    i2s_start(I2S_NUM);
                }

                if (avi_get_streaminfo(pbuf + g_avix.StreamSize) != 0)
                {
                    printf("g_frame error\r\n");
                    res = KEY0_PRES;
                    break;
                }
            }

            i2s_stop(I2S_NUM);
            esp_timer_stop(esp_tim_handle);
            lcd_set_window(0, 0, lcd_self.width, lcd_self.height);
            mjpegdec_free();
            f_close(favi);
        }
    }

    i2s_zero_dma_buffer(I2S_NUM);
    free(framebuf);
    free(favi);
    
    return res;
}

/**
 * @brief       video play
 * @param       None
 * @retval      None
 */
void video_play(void)
{
    uint8_t res;
    FF_DIR vdir;
    FILINFO *vfileinfo;
    uint8_t *pname;
    uint16_t totavinum;
    uint16_t curindex;
    uint32_t *voffsettbl;
    uint8_t key;
    uint32_t temp;
    
    while (f_opendir(&vdir, "0:/VIDEO") != FR_OK)
    {
        text_show_string(60, 190, 240, 16, "VIDEO文件夹错误!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(60, 190, 240, 206, WHITE);
        vTaskDelay(200);
    }
    
    totavinum = video_get_tnum("0:/VIDEO");
    
    while (totavinum == 0)
    {
        text_show_string(60, 190, 240, 16, "没有视频文件!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(60, 190, 240, 146, WHITE);
        vTaskDelay(200);
    }
    
    vfileinfo = (FILINFO *)malloc(sizeof(FILINFO));
    pname = (uint8_t *)malloc(2 * 255 + 1);
    voffsettbl = (uint32_t *)malloc(totavinum  * 4);
    
    while ((vfileinfo == NULL) || (pname == NULL) || (voffsettbl == NULL))
    {
        text_show_string(60, 190, 240, 16, "内存分配失败!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(60, 190, 240, 146, WHITE);
        vTaskDelay(200);
    }
    
    res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");
    
    if (res == 0)
    {
        curindex = 0;
        
        while (1)
        {
            temp = vdir.dptr;
            res = (uint8_t)f_readdir(&vdir, vfileinfo);
            
            if ((res != 0) || (vfileinfo->fname[0] == 0))
            {
                break;
            }
            
            res = exfuns_file_type(vfileinfo->fname);
            
            if ((res & 0xF0) == 0x60)                                           /* It's a video file */
            {
                voffsettbl[curindex] = temp;                                    /* Record index */
                curindex++;
            }
        }
    }
    
    curindex = 0;
    res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");

    while (res == 0)
    {
        dir_sdi(&vdir, voffsettbl[curindex]);
        res = (uint8_t)f_readdir(&vdir, vfileinfo);
        
        if ((res != 0) || (vfileinfo->fname[0] == 0))
        {
            break;
        }
        
        strcpy((char *)pname, "0:/VIDEO/");
        strcat((char *)pname, (const char *)vfileinfo->fname);
        lcd_clear(WHITE);
        video_bmsg_show((uint8_t *)vfileinfo->fname, curindex + 1, totavinum);
        
        key = video_play_mjpeg(pname);

        if (key == KEY2_PRES)   /* Previous video */
        {
            if (curindex != 0)
            {
                curindex--;
            }
            else
            {
                curindex = totavinum - 1;
            }
        }
        else if (key == KEY0_PRES)  /* Next video */
        {
            curindex++;
            if (curindex >= totavinum)
            {
                curindex = 0;
            }
        }
        else
        {
            break;
        }
    }
    
    free(vfileinfo);
    free(pname);
    free(voffsettbl);
}

/**
 * @brief       AVI file search
 * @param       favi    : AVI file
 * @param       aviinfo : AVI control structure
 * @param       mbuf    : Data buffer
 * @retval      results of enforcement
 *   @arg       0       : success
 *   @arg       Other   : error
 */
uint8_t video_seek(FIL *favi, AVI_INFO *aviinfo, uint8_t *mbuf)
{
    uint32_t fpos = favi->fptr;
    uint8_t *pbuf;
    uint16_t offset;
    uint32_t br;
    uint32_t delta;
    uint32_t totsec;
    uint8_t key;

    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;
    totsec /= 1000;
    delta = (favi->obj.objsize / totsec) * 5;

    while (1)
    {
        key = xl9555_key_scan(1);

        if (key == KEY3_PRES)                   /* Fast forward */
        {
            if (fpos < favi->obj.objsize)
            {
                fpos += delta;
            }

            if (fpos > (favi->obj.objsize - AVI_VIDEO_BUF_SIZE))
            {
                fpos = favi->obj.objsize - AVI_VIDEO_BUF_SIZE;
            }
        }
        else if (key == KEY1_PRES)              /* Fast rewind */
        {
            if (fpos > delta)
            {
                fpos -= delta;
            }
            else
            {
                fpos = 0;
            }
        }
        else 
        {
            break;
        }

        f_lseek(favi, fpos);
        f_read(favi, mbuf, AVI_VIDEO_BUF_SIZE, &br);
        pbuf = mbuf;
        
        if (fpos == 0)
        {
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");
        }
        else
        {
            offset = 0;
        }

        offset += avi_srarch_id(pbuf + offset, AVI_VIDEO_BUF_SIZE, g_avix.VideoFLAG);
        avi_get_streaminfo(pbuf + offset);
        f_lseek(favi, fpos + offset + 8);
        
        if (g_avix.StreamID == AVI_VIDS_FLAG)
        {
            f_read(favi, mbuf, g_avix.StreamSize + 8, &br);
            mjpegdec_decode(mbuf, g_avix.StreamSize);
        }
        else
        {
            printf("error flag");
        }

        video_time_show(favi, &g_avix);
    }

    return 0;
}

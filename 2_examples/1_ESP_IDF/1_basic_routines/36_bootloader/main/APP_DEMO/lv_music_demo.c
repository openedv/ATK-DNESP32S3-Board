/**
 ****************************************************************************************************
 * @file        lv_music_demo.c
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

#include "lv_music_demo.h"


LV_FONT_DECLARE(myFont12)
LV_IMG_DECLARE(ui_img_album_png);

lv_obj_t *lv_music_cont;
lv_obj_t *lv_music_client_cont;
lv_obj_t *lv_music_win_header;

static lv_obj_t *song_play_label;
static lv_obj_t *song_last_label;
static lv_obj_t *song_next_label;

static lv_obj_t *obj_cont_4;
static lv_obj_t *obj_cont_5;
static lv_obj_t *obj_cont_6;

static lv_obj_t *now_time_label;
static lv_obj_t *total_time_label;
static lv_obj_t *time_slider;
static lv_obj_t * song_name;

__wavctrl wavctrl;
__audiodev g_audiodev;
uint8_t song_play_state = 0;
uint16_t curindex;
FILINFO *wavfileinfo;
uint8_t *pname;
uint32_t *wavoffsettbl;
FF_DIR wavdir;
uint16_t totwavnum;
uint8_t music_key = 0;
esp_err_t i2s_play_end = ESP_FAIL;
esp_err_t i2s_play_next_prev = ESP_FAIL;
uint32_t nr = 0;
FSIZE_t lv_res = 0;
uint8_t n = 0;
volatile long long int i2s_table_size = 0;
extern uint8_t sd_check_en;

/* MUSIC Task Configuration
 * Including: task handle, task priority, stack size, creation task
 */
#define MUSIC_PRIO      2                   /* task priority */
#define MUSIC_STK_SIZE  5*1024              /* task stack size */
TaskHandle_t            MUSICTask_Handler;  /* task handle */
void music(void *pvParameters);             /* Task function */


/* PLAY Task Configuration
 * Including: task handle, task priority, stack size, creation task
 */
#define PLAY_PRIO      10                  /* task priority */
#define PLAY_STK_SIZE  5*1024              /* task stack size */
TaskHandle_t           PLAYTask_Handler;   /* task handle */
void plsy(void *pvParameters);             /* Task function */


/**
 * @brief       Start audio playback
 * @param       None
 * @retval      None
 */
void lv_audio_start(void)
{
    g_audiodev.status = 3 << 0;
    i2s_trx_start();
}

/**
 * @brief       Stop audio playback
 * @param       None
 * @retval      None
 */
void lv_audio_stop(void)
{
    g_audiodev.status = 0;
    i2s_trx_stop();
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
        lv_slider_set_range(time_slider,0,totsec);
        lv_slider_set_value(time_slider,playtime,LV_ANIM_ON);
        lv_label_set_text_fmt(now_time_label, "%d:%d", playtime / 60,playtime % 60);
        lv_label_set_text_fmt(total_time_label, "%ld:%ld", totsec / 60,totsec % 60);
    }
}

/**
 * @brief  Music playback event callback
 * @param  *e ：event
 * @return None
 */
static void song_play_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    
    if (target == obj_cont_4)        /* Start/Stop */
    {
        if (code == LV_EVENT_CLICKED)
        {
            if (song_play_state ==0)
            {
                music_key = MUSIC_PLAY;
                lv_audio_stop();
                lv_label_set_text(song_play_label, LV_SYMBOL_PLAY);
                song_play_state = 1;
            }
            else
            {
                music_key = MUSIC_PAUSE;
                lv_audio_start();
                lv_label_set_text(song_play_label, LV_SYMBOL_PAUSE);
                song_play_state = 0;
            }
        }
    }
    else if (target == obj_cont_5)       /* Next Song */
    {
        if (code == LV_EVENT_PRESSED)
        {
            lv_obj_set_style_text_color(song_last_label, lv_color_hex(0x272727), 0);
        }
        else if (code == LV_EVENT_RELEASED)
        {
            lv_audio_stop();
            music_key = MUSIC_NEXT;
            lv_label_set_text(song_play_label, LV_SYMBOL_PAUSE);
            lv_obj_set_style_text_color(song_last_label, lv_color_hex(0xffffff), 0);
            nr = 0;
            lv_res = 0;
            n = 0;
            i2s_table_size = 0;
            song_play_state = 0;
        }
    }
    else if (target == obj_cont_6)       /* Previous Song */
    {
        if (code == LV_EVENT_PRESSED)
        {
            lv_obj_set_style_text_color(song_next_label, lv_color_hex(0x272727), 0);
        }
        else if (code == LV_EVENT_RELEASED)
        {
            lv_audio_stop();
            music_key = MUSIC_PREV;
            lv_label_set_text(song_play_label, LV_SYMBOL_PAUSE);
            lv_obj_set_style_text_color(song_next_label, lv_color_hex(0xffffff), 0);
            nr = 0;
            lv_res = 0;
            n = 0;
            i2s_table_size = 0;
            song_play_state = 0;
        }
    }
}

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
            riff = (ChunkRIFF *)buf;                            /* if it is a WAV file */
            
            if (riff->Format == 0x45564157)
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
  * @brief  del music
  * @param  None
  * @retval None
  */
void lv_music_del(void)
{
    lv_audio_stop();
    f_close(g_audiodev.file);

    if (PLAYTask_Handler != NULL)
    {
        vTaskDelete(PLAYTask_Handler);
        taskYIELD();
    }

    if (wavfileinfo || pname || wavoffsettbl)
    {
        free(wavfileinfo);
        free(pname);
        free(wavoffsettbl);
    }

    if (g_audiodev.file || g_audiodev.tbuf)
    {
        free(g_audiodev.file);
        free(g_audiodev.tbuf);
    }

    if (MUSICTask_Handler != NULL)
    {
        vTaskDelete(MUSICTask_Handler);
        taskYIELD();
    }

    PLAYTask_Handler = NULL;
    MUSICTask_Handler = NULL;

    es8388_adda_cfg(0,0);
    es8388_input_cfg(0);
    es8388_output_cfg(0,0);
    i2s_deinit();
    xl9555_pin_write(SPK_EN_IO,1);
    lv_obj_clean(app_obj_general.current_parent);
    lv_obj_del(app_obj_general.current_parent);
    app_obj_general.current_parent = NULL;
    lv_app_show();
    xSemaphoreGive(xGuiSemaphore);
}

/**
 * @brief       play task
 * @param       pvParameters : parameters (not used)
 * @retval      None
 */
void play(void *pvParameters)
{
    pvParameters = pvParameters;

    while(1)
    {
        if ((g_audiodev.status & 0x0F) == 0x03)
        {
            f_lseek(g_audiodev.file, n ? lv_res : wavctrl.datastart);

            for (uint16_t readTimes = 0; readTimes < (wavctrl.datasize / WAV_TX_BUFSIZE); readTimes++)
            {
                if ((g_audiodev.status & 0x0F) == 0x00)
                {
                    if (music_key == MUSIC_PLAY || music_key == MUSIC_PAUSE)
                    {
                        lv_res = f_tell(g_audiodev.file);
                        n = 1;
                    }

                    i2s_zero_dma_buffer(I2S_NUM);

                    if (music_key == 0xFF)
                    {
                        nr = 0;
                        lv_res = 0;
                        n = 0;
                        i2s_table_size = 0;
                        song_play_state = 0;
                        break;
                    }

                    break;
                }

                if (i2s_table_size >= wavctrl.datasize || i2s_play_next_prev == ESP_OK)
                {
                    n = 0;
                    i2s_table_size = 0;
                    i2s_play_end = ESP_OK;
                    i2s_zero_dma_buffer(I2S_NUM);
                    taskYIELD();
                    break;
                }

                if (sdmmc_get_status(card) != ESP_OK)
                {
                    i2s_zero_dma_buffer(I2S_NUM);
                    g_audiodev.status = 0;

                    while (1)
                    {
                        back_act_key = KEY1_PRES;
                        taskYIELD();
                    }
                }

                f_read(g_audiodev.file,g_audiodev.tbuf, WAV_TX_BUFSIZE, (UINT*)&nr);
                i2s_table_size = i2s_table_size + i2s_tx_write(g_audiodev.tbuf, WAV_TX_BUFSIZE);
                vTaskDelay(10);
            }
        }
        else
        {
            if (sdmmc_get_status(card) != ESP_OK)
            {
                while (1)
                {
                    back_act_key = KEY1_PRES;
                    vTaskDelay(10);
                }
            }

            vTaskDelay(10);
        }
    }
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
                rval++;
            }
        }
    }
    
    free(tfileinfo);
    
    return rval;
}

/**
 * @brief       music
 * @param       pvParameters : parameters (not used)
 * @retval      None
 */
void music(void *pvParameters)
{
    pvParameters = pvParameters;
    uint8_t res;
    uint32_t temp;
    g_audiodev.file = (FIL*)malloc(sizeof(FIL));
    g_audiodev.tbuf = malloc(WAV_TX_BUFSIZE);

    es8388_adda_cfg(1, 0);
    es8388_input_cfg(0);
    es8388_output_cfg(1, 1);
    es8388_hpvol_set(20);
    es8388_spkvol_set(20);
    xl9555_pin_write(SPK_EN_IO,0);

    while(1)
    {
        /* record index */
        res = f_opendir(&wavdir, "0:/MUSIC");

        if (res == FR_OK)
        {
            curindex = 0;                                               /* the current index is 0 */
            
            while (1)
            {
                temp = wavdir.dptr;                                     /* record current index */

                res = f_readdir(&wavdir, wavfileinfo);                  /* read a file from the directory */
                
                if ((res != FR_OK) || (wavfileinfo->fname[0] == 0))
                {
                    break;                                              /* error/at the end, exit */
                }

                res = exfuns_file_type(wavfileinfo->fname);
                
                if ((res & 0xF0) == 0x40)
                {
                    wavoffsettbl[curindex] = temp;                      /* record index */
                    curindex++;
                }
            }
        }

        curindex = 0;       
        res = f_opendir(&wavdir, (const TCHAR*)"0:/MUSIC");         /* display from 0 */
        
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
            lv_label_set_text_fmt(song_name,"%s",wavfileinfo->fname);
            i2s_play_end = ESP_FAIL;
            i2s_play_next_prev = ESP_FAIL;
            music_key = 0;
            
            if (g_audiodev.file || g_audiodev.tbuf)
            {
                res = wav_decode_init(pname, &wavctrl);     /* previous song */
            }

            if (res == 0)
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
            }

            res = f_open(g_audiodev.file, (TCHAR*)pname, FA_READ);

            lv_audio_stop();
            vTaskDelay(100);
            lv_audio_start();

            if (PLAYTask_Handler == NULL)
            {
                xTaskCreatePinnedToCore((TaskFunction_t )play,
                                        (const char*    )"play",
                                        (uint16_t       )PLAY_STK_SIZE,
                                        (void*          )NULL,
                                        (UBaseType_t    )PLAY_PRIO,
                                        (TaskHandle_t*  )&PLAYTask_Handler,
                                        (BaseType_t     ) 1);
            }

            while (1)
            {
                xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

                if (music_key == MUSIC_PREV)                                   /* previous song */
                {
                    if (curindex)
                    {
                        curindex--;
                    }
                    else
                    {
                        curindex = totwavnum - 1;
                    }

                    i2s_play_next_prev = ESP_OK;
                    break;
                }
                else if (music_key == MUSIC_NEXT || i2s_play_end == ESP_OK) /* next song */
                {
                    curindex++;

                    if (curindex >= totwavnum)
                    {
                        curindex = 0;
                    }

                    i2s_play_next_prev = ESP_OK;
                    break;
                }

                if ((g_audiodev.status & 0x0F) == 0x03)
                {
                    wav_get_curtime(g_audiodev.file, &wavctrl);
                    audio_msg_show(wavctrl.totsec, wavctrl.cursec, wavctrl.bitrate);
                }

                xSemaphoreGive(xGuiSemaphore);

                vTaskDelay(100);
            }

            i2s_zero_dma_buffer(I2S_NUM);
            g_audiodev.status = 0;
            nr = 0;
            lv_res = 0;
            n = 0;
            i2s_table_size = 0;
            song_play_state = 0;
            music_key = 0xff;
            xSemaphoreGive(xGuiSemaphore);
        }

        free(wavfileinfo);
        free(pname);
        free(wavoffsettbl);
    }
}

/**
 * @brief  music demo
 * @param  None
 * @return None
 */
void lv_music_demo(void)
{
    nr = 0;
    lv_res = 0;
    n = 0;
    i2s_table_size = 0;
    song_play_state = 0;

    if (app_obj_general.current_parent != NULL)
    {
        lv_obj_del(app_obj_general.current_parent);
        app_obj_general.current_parent = NULL;
    }

    if (sd_check_en == 0)
    {
        lv_msgbox("SD device not detected");
    }
    else
    {
        sd_check_en = 1;
        i2s_init();
        vTaskDelay(100);
        if (f_opendir(&wavdir, "0:/MUSIC"))
        {
            lv_msgbox("MUSIC folder error");
            return ;
        }
        
        totwavnum = audio_get_tnum((uint8_t *)"0:/MUSIC");

        if (totwavnum == 0)
        {
            lv_msgbox("No music files");
            return ;
        }

        wavfileinfo = (FILINFO*)malloc(sizeof(FILINFO));
        pname = malloc(255 * 2 + 1);
        wavoffsettbl = malloc(4 * totwavnum);

        if (!wavfileinfo || !pname || !wavoffsettbl)
        {
            lv_msgbox("memory allocation failed");
            return ;
        }

        lv_app_del();

        lv_obj_t *music_obj = lv_obj_create(lv_scr_act());
        lv_obj_set_style_radius(music_obj, 0, LV_STATE_DEFAULT);
        lv_obj_set_size(music_obj,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
        lv_obj_set_style_border_opa(music_obj,LV_OPA_0,LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(music_obj,lv_color_make(0,0,0),LV_STATE_DEFAULT);
        lv_obj_set_pos(music_obj,0,0);
        lv_obj_clear_flag(music_obj, LV_OBJ_FLAG_SCROLLABLE);

        app_obj_general.current_parent = music_obj;
        app_obj_general.Function = lv_music_del;

        
        lv_obj_t *obj_cont_1 = lv_obj_create(music_obj);
        lv_obj_set_size(obj_cont_1, lv_obj_get_width(lv_scr_act()) - 90, lv_obj_get_width(lv_scr_act()) - 90);
        lv_obj_set_style_bg_color(obj_cont_1, lv_color_make(50,52,67), LV_STATE_DEFAULT);
        lv_obj_align(obj_cont_1, LV_ALIGN_TOP_MID, 0, 50);
        lv_obj_set_style_border_opa(obj_cont_1, 0, 0);
        lv_obj_set_style_radius(obj_cont_1, 200, 0);

        lv_obj_t * img_obj_cont_1 = lv_img_create(obj_cont_1);
        lv_obj_set_size(img_obj_cont_1,ui_img_album_png.header.w, ui_img_album_png.header.h);
        lv_img_set_src(img_obj_cont_1,&ui_img_album_png);
        lv_obj_center(img_obj_cont_1);
        
        song_name = lv_label_create(music_obj);
        lv_obj_align(song_name,LV_ALIGN_TOP_MID,0,25);
        lv_obj_set_style_text_font(song_name,&myFont12,LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(song_name,lv_color_hex(0xffffff),LV_STATE_DEFAULT);
        lv_label_set_text(song_name," ");

        time_slider = lv_slider_create(music_obj);
        lv_obj_set_size(time_slider, lv_obj_get_width(lv_scr_act())/2 + 50, 5);
        lv_obj_align_to(time_slider, obj_cont_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
        lv_slider_set_range(time_slider,0,100);
        lv_obj_set_style_bg_color(time_slider, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(time_slider, 255, 0);
        lv_obj_set_style_bg_color(time_slider, lv_color_make(192,192,192), 0);
        lv_obj_remove_style(time_slider, NULL, LV_PART_KNOB);

        now_time_label = lv_label_create(music_obj);
        lv_obj_set_style_text_font(now_time_label, &lv_font_montserrat_14, 0);
        lv_label_set_text(now_time_label, "0:00");
        lv_obj_set_style_text_color(now_time_label, lv_color_hex(0xffffff), 0);
        lv_obj_align_to(now_time_label, time_slider, LV_ALIGN_OUT_LEFT_MID, 0, 0);
        
        total_time_label = lv_label_create(music_obj);
        lv_obj_set_style_text_font(total_time_label, &lv_font_montserrat_14, 0);
        lv_label_set_text(total_time_label, "0:00");
        lv_obj_set_style_text_color(total_time_label, lv_color_hex(0xffffff), 0);
        lv_obj_align_to(total_time_label, time_slider, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        
        obj_cont_4 = lv_obj_create(music_obj);
        lv_obj_set_size(obj_cont_4, 70, 70);
        lv_obj_set_style_bg_color(obj_cont_4, lv_color_make(20,20,20), LV_STATE_DEFAULT); 
        lv_obj_align(obj_cont_4, LV_ALIGN_BOTTOM_MID, 0, -10);
        lv_obj_set_style_border_color(obj_cont_4,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(obj_cont_4,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_translate_y(obj_cont_4, 5, LV_STATE_PRESSED);
        lv_obj_set_style_radius(obj_cont_4, 70, 0);
        lv_obj_set_style_border_width(obj_cont_4,2,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(obj_cont_4,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_shadow_width(obj_cont_4,10,LV_STATE_FOCUS_KEY);
        lv_obj_add_event_cb(obj_cont_4, song_play_event_cb, LV_EVENT_CLICKED, NULL);
        
        song_play_label = lv_label_create(obj_cont_4);
        lv_obj_set_style_text_font(song_play_label, &lv_font_montserrat_14, 0);
        lv_label_set_text(song_play_label, LV_SYMBOL_PAUSE);
        lv_obj_set_style_text_color(song_play_label, lv_color_hex(0xffffff), 0);
        lv_obj_align(song_play_label, LV_ALIGN_CENTER, 0, 0);

        obj_cont_5 = lv_obj_create(music_obj);
        lv_obj_set_size(obj_cont_5, 50, 50);
        lv_obj_align_to(obj_cont_5, obj_cont_4, LV_ALIGN_OUT_LEFT_MID, -20, 0);
        lv_obj_set_style_bg_color(obj_cont_5,lv_color_make(20,20,20),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(obj_cont_5,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(obj_cont_5,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_radius(obj_cont_5, 50, 0);
        lv_obj_set_style_border_width(obj_cont_5,2,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(obj_cont_5,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_shadow_width(obj_cont_5,10,LV_STATE_FOCUS_KEY);
        lv_obj_add_event_cb(obj_cont_5, song_play_event_cb, LV_EVENT_ALL, NULL);
        
        song_last_label = lv_label_create(obj_cont_5);
        lv_obj_set_style_text_font(song_last_label, &lv_font_montserrat_14, 0);
        lv_label_set_text(song_last_label, LV_SYMBOL_PREV);
        lv_obj_set_style_text_color(song_last_label, lv_color_hex(0xffffff), 0);
        lv_obj_align(song_last_label, LV_ALIGN_CENTER, 0, 0);

        obj_cont_6 = lv_obj_create(music_obj);
        lv_obj_set_size(obj_cont_6, 50, 50);
        lv_obj_align_to(obj_cont_6, obj_cont_4, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
        lv_obj_set_style_bg_color(obj_cont_6,lv_color_make(20,20,20),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(obj_cont_6,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(obj_cont_6,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_radius(obj_cont_6, 50, 0);
        lv_obj_set_style_border_width(obj_cont_6,2,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(obj_cont_6,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_shadow_width(obj_cont_6,10,LV_STATE_FOCUS_KEY);
        lv_obj_add_event_cb(obj_cont_6, song_play_event_cb, LV_EVENT_ALL, NULL);
        
        song_next_label = lv_label_create(obj_cont_6);
        lv_obj_set_style_text_font(song_next_label, &lv_font_montserrat_14, 0);
        lv_label_set_text(song_next_label, LV_SYMBOL_NEXT);
        lv_obj_set_style_text_color(song_next_label, lv_color_hex(0xffffff), 0);
        lv_obj_align(song_next_label, LV_ALIGN_CENTER, 0, 0);
    
        lv_group_add_obj(ctrl_g, obj_cont_5);
        lv_group_add_obj(ctrl_g, obj_cont_4);
        lv_group_add_obj(ctrl_g, obj_cont_6);
        lv_group_remove_obj(time_slider);
        lv_group_focus_obj(obj_cont_5);

        if (MUSICTask_Handler == NULL)
        {
            xTaskCreatePinnedToCore((TaskFunction_t )music,
                                    (const char*    )"music",
                                    (uint16_t       )MUSIC_STK_SIZE,
                                    (void*          )NULL,
                                    (UBaseType_t    )MUSIC_PRIO,
                                    (TaskHandle_t*  )&MUSICTask_Handler,
                                    (BaseType_t     ) 0);
        }
    }
}

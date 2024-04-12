/**
 ****************************************************************************************************
 * @file        lv_pic_demo.c
 * @author      ALIENTEK
 * @brief       LVGL PIC Demo code
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

#include "lv_pic_demo.h"


LV_FONT_DECLARE(myFont12)
FF_DIR picdir;
FILINFO *pic_picfileinfo;
char *pic_pname;
uint16_t pic_totpicnum;
uint16_t pic_curindex = 0;
uint32_t *pic_picoffsettbl;
uint8_t pic_key = PIC_NULL;
static lv_obj_t *pic_frame;
static lv_obj_t *pic_last_label;
static lv_obj_t *pic_next_label;
static lv_obj_t *pic_name;
extern uint8_t sd_check_en;
extern uint8_t decode_en;

/* PIC Task Configuration
 * Including: task handle, task priority, stack size, creation task
 */
#define PIC_PRIO      10                                /* task priority */
#define PIC_STK_SIZE  5 * 1024                          /* task stack size */
TaskHandle_t          PICTask_Handler;                  /* task handle */
void pic(void *pvParameters);                           /* Task function */

/**
 * @brief       Obtain the total number of target files in the path path
 * @param       path : path
 * @retval      Total number of valid files
 */
uint16_t pic_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;
    FILINFO *tfileinfo;
    tfileinfo = (FILINFO *)malloc(sizeof(FILINFO));
    res = f_opendir(&tdir, (const TCHAR *)path);

    if (res == FR_OK && tfileinfo)
    {
        while (1)
        {
            res = f_readdir(&tdir, tfileinfo);

            if (res != FR_OK || tfileinfo->fname[0] == 0)break;
            res = exfuns_file_type(tfileinfo->fname);

            if ((res & 0X0F) != 0X00)
            {
                rval++;
            }
        }
    }

    free(tfileinfo);
    return rval;
}

lv_img_dsc_t img_pic_dsc = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = NULL,
};

/**
 * @brief       PNG/BMPJPEG/JPG decoding
 * @param       filename : file name
 * @param       width    : image width
 * @param       height   : image height
 * @retval      None
 */
void lv_pic_png_bmp_jpeg_decode(uint16_t w,uint16_t h,uint8_t * pic_buf)
{
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    img_pic_dsc.header.w = w;
    img_pic_dsc.header.h = h;
    img_pic_dsc.data_size = w * h * 2;
    img_pic_dsc.data = (const uint8_t *)pic_buf;
    lv_img_set_src(pic_frame,&img_pic_dsc);
    xSemaphoreGive(xGuiSemaphore);
}

/**
 * @brief       pic task
 * @param       pvParameters : parameters (not used)
 * @retval      None
 */
void pic(void *pvParameters)
{
    pvParameters = pvParameters;
    uint8_t res = 0;
    uint16_t temp = 0;

    while(1)
    {
        res = f_opendir(&picdir, "0:/PICTURE");

        if (res == FR_OK)
        {
            pic_curindex = 0;

            while (1)
            {
                temp = picdir.dptr;
                res = f_readdir(&picdir, pic_picfileinfo);
                if (res != FR_OK || pic_picfileinfo->fname[0] == 0)break;

                res = exfuns_file_type(pic_picfileinfo->fname);

                if ((res & 0X0F) != 0X00)
                {
                    pic_picoffsettbl[pic_curindex] = temp;
                    pic_curindex++;
                }
            }
        }

        pic_curindex = 0;
        res = f_opendir(&picdir, (const TCHAR *)"0:/PICTURE");

        while (res == FR_OK)
        {
            dir_sdi(&picdir, pic_picoffsettbl[pic_curindex]);
            res = f_readdir(&picdir, pic_picfileinfo);

            if (res != FR_OK || pic_picfileinfo->fname[0] == 0)break;

            strcpy((char *)pic_pname, "0:/PICTURE/");
            strcat((char *)pic_pname, (const char *)pic_picfileinfo->fname);

            temp = exfuns_file_type(pic_pname);

            decode_en = ESP_FAIL;

            switch (temp)
            {
                case T_BMP:
                    bmp_decode(pic_pname,lcd_self.width,lcd_self.height,lv_pic_png_bmp_jpeg_decode);    /* BMP decode */
                    break;
                case T_JPG:
                case T_JPEG:
                    jpeg_decode(pic_pname,lcd_self.width,lcd_self.height,lv_pic_png_bmp_jpeg_decode);   /* JPG/JPEG decode */
                    break;
                case T_PNG:
                    png_decode(pic_pname,lcd_self.width,lcd_self.height,lv_pic_png_bmp_jpeg_decode);    /* PNG decode */
                    break;
                default:
                    pic_key = PIC_NEXT;                                                                 /* Non image format */
                    break;
            }

            decode_en = ESP_OK;

            lv_label_set_text_fmt(pic_name,"%s",pic_pname);

            while (1)
            {
                xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

                if (sd_check_en == 0)
                {
                    back_act_key = KEY1_PRES;
                    xSemaphoreGive(xGuiSemaphore);
                }

                if (pic_key == PIC_PREV)
                {
                    if (pic_curindex)
                    {
                        pic_curindex--;
                    }
                    else
                    {
                        pic_curindex = pic_totpicnum - 1;
                    }

                    pic_key = PIC_NULL;
                    break;
                }
                else if (pic_key == PIC_NEXT)
                {
                    pic_curindex++;

                    if (pic_curindex >= pic_totpicnum)
                    {
                        pic_curindex = 0;
                    }

                    pic_key = PIC_NULL;
                    break;
                }

                xSemaphoreGive(xGuiSemaphore);

                vTaskDelay(10);
            }

            xSemaphoreGive(xGuiSemaphore);
        }
    }
}

/**
  * @brief  del pic
  * @param  None
  * @retval None
  */
void lv_pic_del(void)
{
    if (PICTask_Handler != NULL)
    {
        vTaskDelete(PICTask_Handler);
        vTaskDelay(10);
    }

    if (pic_picfileinfo || pic_pname || pic_picoffsettbl)
    {
        free(pic_picfileinfo);
        free(pic_pname);
        free(pic_picoffsettbl);
    }
    decode_en = ESP_OK;
    
    lv_obj_clean(app_obj_general.current_parent);
    lv_obj_del(app_obj_general.current_parent);
    app_obj_general.current_parent = NULL;
    lv_app_show();
    xSemaphoreGive(xGuiSemaphore);
    PICTask_Handler = NULL;
}

/**
 * @brief  Album playback event callback
 * @param  *e ：event
 * @return None
 */
static void pic_play_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    
    if (target == pic_last_label)
    {
        if (code == LV_EVENT_PRESSED)
        {
            lv_obj_set_style_text_color(pic_last_label, lv_color_hex(0x272727), 0);
        }
        else if (code == LV_EVENT_RELEASED)
        {
            pic_key = PIC_PREV;
            lv_obj_set_style_text_color(pic_last_label, lv_color_make(103,116,190), 0);
        }
    }
    else if (target == pic_next_label)
    {
        if (code == LV_EVENT_PRESSED)
        {
            lv_obj_set_style_text_color(pic_next_label, lv_color_hex(0x272727), 0);
        }
        else if (code == LV_EVENT_RELEASED)
        {
            pic_key = PIC_NEXT;
            lv_obj_set_style_text_color(pic_next_label, lv_color_make(103,116,190), 0);
        }
    }
}

/**
 * @brief  pic demo
 * @param  None
 * @return None
 */
void lv_pic_demo(void)
{
    pic_key = PIC_NULL;
    pic_frame = NULL;

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

        if (f_opendir(&picdir, "0:/PICTURE"))
        {
            lv_msgbox("PICTURE folder error");
            return ;
        }
        
        pic_totpicnum = pic_get_tnum("0:/PICTURE");

        if (pic_totpicnum == 0)
        {
            lv_msgbox("No pic files");
            return ;
        }

        pic_picfileinfo = (FILINFO *)malloc(sizeof(FILINFO));
        pic_pname = malloc(255 * 2 + 1);
        pic_picoffsettbl = malloc(4 * pic_totpicnum);

        if (!pic_picfileinfo || !pic_pname || !pic_picoffsettbl)
        {
            lv_msgbox("memory allocation failed");
            return ;
        }

        lv_app_del();

        lv_obj_t *pic_obj = lv_obj_create(lv_scr_act());
        lv_obj_set_size(pic_obj,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
        lv_obj_set_style_bg_color(pic_obj, lv_color_make(0,0,0), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(pic_obj,LV_OPA_100,LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(pic_obj,LV_OPA_0,LV_STATE_DEFAULT);
        lv_obj_set_pos(pic_obj,0,0);
        lv_obj_clear_flag(pic_obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_page_tile(pic_obj,"Image Browser");
        
        pic_name = lv_label_create(pic_obj);
        lv_obj_align(pic_name,LV_ALIGN_OUT_TOP_LEFT,0,42);
        lv_obj_set_width(pic_name,lv_obj_get_width(lv_scr_act()));
        lv_obj_set_style_text_font(pic_name,&myFont12,LV_STATE_DEFAULT);
        lv_label_set_long_mode(pic_name,LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_style_text_color(pic_name,lv_color_hex(0xff0000),LV_STATE_DEFAULT);
        lv_label_set_text(pic_name," ");

        pic_frame = lv_img_create(pic_obj);
        lv_obj_set_style_bg_color(pic_frame, lv_color_make(50,52,67), LV_STATE_DEFAULT);
        lv_obj_align(pic_frame,LV_ALIGN_CENTER,0,20);
        lv_obj_move_background(pic_frame);

        app_obj_general.current_parent = pic_obj;
        app_obj_general.Function = lv_pic_del;

        pic_last_label = lv_label_create(pic_obj);
        lv_obj_set_style_text_font(pic_last_label, &lv_font_montserrat_28, 0);
        lv_label_set_text(pic_last_label, LV_SYMBOL_PREV);
        lv_obj_set_style_text_color(pic_last_label,lv_palette_main(LV_PALETTE_RED),LV_STATE_FOCUSED);
        lv_obj_align(pic_last_label,LV_ALIGN_LEFT_MID,10,0);
        lv_obj_set_style_text_color(pic_last_label, lv_color_make(103,116,190), 0);
        lv_obj_add_event_cb(pic_last_label, pic_play_event_cb, LV_EVENT_ALL, NULL);

        pic_next_label = lv_label_create(pic_obj);
        lv_obj_set_style_text_font(pic_next_label, &lv_font_montserrat_28, 0);
        lv_obj_set_style_text_color(pic_next_label,lv_palette_main(LV_PALETTE_RED),LV_STATE_FOCUSED);
        lv_label_set_text(pic_next_label, LV_SYMBOL_NEXT);
        lv_obj_align(pic_next_label,LV_ALIGN_RIGHT_MID,-10,0);
        lv_obj_set_style_text_color(pic_next_label, lv_color_make(103,116,190), 0);
        lv_obj_add_event_cb(pic_next_label, pic_play_event_cb, LV_EVENT_ALL, NULL);

        lv_group_add_obj(ctrl_g, pic_last_label);
        lv_group_add_obj(ctrl_g, pic_next_label);
        lv_group_focus_obj(pic_last_label);

        if (PICTask_Handler == NULL)
        {
            xTaskCreatePinnedToCore((TaskFunction_t )pic,
                                    (const char*    )"pic",
                                    (uint16_t       )PIC_STK_SIZE,
                                    (void*          )NULL,
                                    (UBaseType_t    )PIC_PRIO,
                                    (TaskHandle_t*  )&PICTask_Handler,
                                    (BaseType_t     ) 1);
        }
    }
}
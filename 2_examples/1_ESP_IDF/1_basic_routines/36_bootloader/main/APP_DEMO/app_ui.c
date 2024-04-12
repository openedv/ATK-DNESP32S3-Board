/**
 ****************************************************************************************************
 * @file        app_ui.c
 * @author      ALIENTEK
 * @brief       APP UI code
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

#include "app_ui.h"
#include "sdmmc_cmd.h"

extern lv_indev_t *indev_keypad;
app_obj_t app_obj_btn;
lv_m_general app_obj_general;
lv_group_t *ctrl_g;
extern uint8_t sd_check_en;
LV_FONT_DECLARE(myFont12)

const app_image_info app_image[] =
{
    {"camera","摄像头",&app_obj_btn.image_bin[0]},
    {"file","文件",&app_obj_btn.image_bin[1]},
    {"video","视频",&app_obj_btn.image_bin[2]},
    {"setting","设置",&app_obj_btn.image_bin[3]},
    {"weather","天气",&app_obj_btn.image_bin[4]},
    {"measure","测量",&app_obj_btn.image_bin[5]},
    {"photo","相册",&app_obj_btn.image_bin[6]},
    {"music","音乐",&app_obj_btn.image_bin[7]},
    {"calendar","日历",&app_obj_btn.image_bin[8]},
};

#define image_mun (int)(sizeof(app_image)/sizeof(app_image[0]))

unsigned int  app_readly_list[32];

int lv_trigger_bit = 0;
uint8_t *image_buffer[APP_NUM];
uint32_t at_cpu;
static uint8_t load_index = 0;

/**
 * @brief       Obtain images from SPI FLASH (BMP, JEG, PNG)
 * @param       image: Pointer to store image information
 * @param       addr : image addr
 * @param       size : image size
 * @retval      0 : Load successful, !0 : Load failed
 */
uint8_t lv_load_img(lv_img_dsc_t *image,uint32_t addr,uint32_t size)
{
    
    uint8_t err = 1;
    uint32_t image_header = 0;
    uint8_t *image_jpeg = NULL;

    load_index ++;

    image_buffer[load_index - 1] = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    
    if (image_buffer[load_index - 1] != NULL)
    {
        err = 0;
    }
    
    err = images_partition_read(image_buffer[load_index - 1],addr,size);
  
    /* Get header */
    image_header = (uint32_t)image_buffer[load_index - 1][3] << 24;
    image_header |= (uint32_t)image_buffer[load_index - 1][2] << 16;
    image_header |= (uint32_t)image_buffer[load_index - 1][1] << 8;
    image_header |= (uint32_t)image_buffer[load_index - 1][0];
    
    /* Get image data stream */
    image_jpeg = (uint8_t *)image_buffer[load_index - 1] + 4;
    
    image->header.cf = image_buffer[load_index - 1][0];
    image->header.always_zero = 0;
    image->header.w = (uint16_t)(image_header >> 10);
    image->header.h = (uint16_t)(image_header >> 21);
    image->data_size = size - 4;
    image->data = image_jpeg;
    
    return err;
}

/**
 * @brief       Message prompt for deletion
 * @param       None
 * @retval      None
 */
void lv_msgbox_del(void)
{
    lv_obj_del(app_obj_general.current_parent);
    app_obj_general.current_parent = NULL;
}

/**
 * @brief       Message prompt
 * @param       name : Message
 * @retval      None
 */
void lv_msgbox(char *name)
{
    /* msgbox create */
    lv_obj_t *msgbox = lv_msgbox_create(lv_scr_act(),LV_SYMBOL_WARNING "Notice",name, NULL,false);
    lv_obj_set_size(msgbox, lcd_self.width - 20, lcd_self.height / 3);   /* set size */
    lv_obj_center(msgbox);
    lv_obj_set_style_border_width(msgbox, 0, 0);
    lv_obj_set_style_shadow_width(msgbox, 20, 0);
    lv_obj_set_style_shadow_color(msgbox, lv_color_hex(0xa9a9a9),LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(msgbox,18,LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(msgbox,20,LV_STATE_DEFAULT);

    lv_obj_t *title = lv_msgbox_get_title(msgbox);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14,LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(title, lv_color_hex(0xff0000),LV_STATE_DEFAULT);

    lv_obj_t *content = lv_msgbox_get_content(msgbox);
    lv_obj_set_style_text_font(content, &lv_font_montserrat_14,LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(content, lv_color_hex(0x6c6c6c),LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(content,15,LV_STATE_DEFAULT);
    app_obj_general.current_parent = msgbox;
    app_obj_general.Function = lv_msgbox_del;
}

/**
 * @brief       Load icons
 * @param       None
 * @retval      None
 */
void lv_load_iamge(void)
{
    for (int i = 0 ; i < APP_NUM ;i++)
    {
        memset(&app_obj_btn.image_bin[i],0,sizeof(lv_img_dsc_t));
        app_obj_btn.image_flag = lv_load_img(&app_obj_btn.image_bin[i],*(uint32_t*)(&g_ftinfo.lvgl_camera_addr + 2 * i),g_ftinfo.lvgl_camera_size);
        
        if (app_obj_btn.image_flag != 0)
        {
            printf("Failed to load icon\r\n");
        }
    }

    load_index = 0;
}

/**
  * @brief  Calculate leading zeros
  * @param  app_readly_list:Ready Table
  * @retval Return to ready position
  */
int lv_clz(unsigned int  app_readly_list[])
{
    int bit = 0;

    for (int i = 0; i < 32; i++)
    {
        if (app_readly_list[i] == 1)
        {
            break;
        }

        bit ++ ;
    }

    return bit;
}

lv_obj_t *status_bar_left;
lv_obj_t *usb_status_left;
lv_obj_t *sd_status_left;
lv_obj_t *cpu_status;

/**
  * @brief  key callback function
  * @param  obj   : Object
  * @param  event : event
  * @retval None
  */
static void lv_imgbtn_control_event_handler(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED)
    {

        for (int i = 0;i < image_mun;i ++)
        {
            if (obj == app_obj_btn.app_btn[i])
            {
                app_readly_list[i] = 1 ;
                
            }
        }

        app_obj_general.fouc_parent = obj;

        lv_trigger_bit = ((unsigned int)lv_clz((app_readly_list)));
        app_readly_list[lv_trigger_bit] = 0;

        switch(lv_trigger_bit)
        {
            case 0: /* camera app */
                lv_camera_demo();
                break;
            
            case 1: /* file system app */
                lv_file_demo();
                break;
            
            case 2: /* video app */
                lv_video_demo();
                break;
                
            case 3:
            case 4:
                if (app_obj_general.current_parent != NULL)
                {
                    lv_obj_del(app_obj_general.current_parent);
                    app_obj_general.current_parent = NULL;
                }
                lv_msgbox("unrealized!!!");
                break;
            
            case 5: /* measure app */
                lv_measure_demo();
                break;
            
            case 6: /* pic app */
                lv_pic_demo();
                break;
            
            case 7: /* music app */
                lv_music_demo();
                break;

            case 8: /* clock app */
                lv_clock_demo();
                break;
            default:
                break;
        }
    }
}

/**
  * @brief  Icon display
  * @param  None
  * @retval None
  */
void lv_app_show(void)
{
    lv_load_iamge();

    for (int i = 0;i < APP_NUM;i++)
    {
        lv_imgbtn_set_src(app_obj_btn.app_btn[i], LV_IMGBTN_STATE_RELEASED, NULL, &app_obj_btn.image_bin[i], NULL);
        lv_timer_handler();
    }

    lv_obj_clear_flag(app_obj_btn.lv_main_cont,LV_OBJ_FLAG_HIDDEN);
    lv_obj_update_layout(app_obj_btn.lv_main_cont);
    lv_group_focus_obj(app_obj_general.fouc_parent);
    app_obj_general.fouc_parent = NULL;
}

/**
  * @brief  Release APP icon memory
  * @param  None
  * @retval None
  */
void lv_app_del(void)
{
    for (int i = 0;i < APP_NUM;i++)
    {
        heap_caps_free(image_buffer[i]);
        lv_imgbtn_set_src(app_obj_btn.app_btn[i], LV_IMGBTN_STATE_RELEASED, NULL, NULL, NULL);
        lv_timer_handler();
        image_buffer[i] = NULL;
    }
    
    lv_obj_add_flag(app_obj_btn.lv_main_cont,LV_OBJ_FLAG_HIDDEN);
    lv_obj_update_layout(app_obj_btn.lv_main_cont);
}

/**
  * @brief  Main interface painting
  * @param  parent:parent
  * @retval None
  */
void lv_mid_cont_add_app(lv_obj_t *parent)
{
    int lv_index = 0;
    int line_feed_num = 0;
    int n = 0;

    ctrl_g = lv_group_create();
    lv_group_set_default(ctrl_g);
    lv_indev_set_group(indev_keypad, ctrl_g);

    app_obj_btn.app_btn[lv_index] = lv_imgbtn_create(parent);
    lv_imgbtn_set_src(app_obj_btn.app_btn[lv_index], LV_IMGBTN_STATE_RELEASED, NULL, &app_obj_btn.image_bin[lv_index], NULL);
    lv_obj_set_size(app_obj_btn.app_btn[lv_index], app_obj_btn.image_bin[lv_index].header.w, app_obj_btn.image_bin[lv_index].header.h);
    lv_obj_set_pos(app_obj_btn.app_btn[lv_index], 15, 25);
    lv_obj_set_style_bg_opa(app_obj_btn.app_btn[lv_index],LV_OPA_50,LV_STATE_FOCUSED);
    unsigned int lv_width_x = app_obj_btn.image_bin[lv_index].header.w + 20;

    lv_group_add_obj(ctrl_g, app_obj_btn.app_btn[lv_index]);
    lv_obj_add_event_cb(app_obj_btn.app_btn[lv_index], lv_imgbtn_control_event_handler, LV_EVENT_ALL, NULL);
    
    app_obj_btn.app_name[lv_index] = lv_label_create(parent);
    lv_obj_set_style_text_color(app_obj_btn.app_name[lv_index],lv_color_hex(0xf0f0f0), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(app_obj_btn.app_name[lv_index], &myFont12, 0);
    lv_label_set_text(app_obj_btn.app_name[lv_index], app_image[lv_index].app_text_English);
    lv_obj_align_to(app_obj_btn.app_name[lv_index],app_obj_btn.app_btn[lv_index],LV_ALIGN_OUT_BOTTOM_MID,0, 0);
    lv_obj_set_style_img_recolor_opa(app_obj_btn.app_btn[lv_index], LV_OPA_30, LV_STATE_PRESSED);
    lv_obj_set_style_img_recolor(app_obj_btn.app_btn[lv_index], lv_color_black(), LV_STATE_PRESSED);
    
    for (lv_index = 1 ; lv_index < APP_NUM; lv_index ++)
    {
        app_obj_btn.app_btn[lv_index] = lv_imgbtn_create(parent);
        lv_imgbtn_set_src(app_obj_btn.app_btn[lv_index], LV_IMGBTN_STATE_RELEASED, NULL, &app_obj_btn.image_bin[lv_index], NULL);
        lv_obj_set_size(app_obj_btn.app_btn[lv_index], app_obj_btn.image_bin[lv_index].header.w, app_obj_btn.image_bin[lv_index].header.h);
        lv_obj_set_style_img_recolor_opa(app_obj_btn.app_btn[lv_index], LV_OPA_30, LV_STATE_PRESSED);
        lv_obj_set_style_img_recolor(app_obj_btn.app_btn[lv_index], lv_color_black(), LV_STATE_PRESSED);
        lv_obj_set_style_size(app_obj_btn.app_btn[lv_index], app_obj_btn.image_bin[lv_index].header.w - 2,LV_STATE_PRESSED);
        lv_width_x = lv_width_x + app_obj_btn.image_bin[lv_index].header.w + 10;
        lv_obj_set_style_bg_opa(app_obj_btn.app_btn[lv_index],LV_OPA_50,LV_STATE_FOCUSED);
        lv_group_add_obj(ctrl_g, app_obj_btn.app_btn[lv_index]);

        if (lv_width_x < lv_obj_get_width(lv_scr_act()))
        {
            lv_obj_align_to(app_obj_btn.app_btn[lv_index], app_obj_btn.app_btn[lv_index - 1], LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        }
        else
        {
            line_feed_num++;

            if (line_feed_num >= 2)
            {
                n = app_obj_btn.image_bin[lv_index].header.h + 5;
            }
            
            lv_obj_set_pos(app_obj_btn.app_btn[lv_index], 15, (app_obj_btn.image_bin[lv_index].header.h * (lv_index)) - n - 80);
            lv_width_x = app_obj_btn.image_bin[lv_index].header.w + 10;
        }
        
        lv_obj_add_event_cb(app_obj_btn.app_btn[lv_index], lv_imgbtn_control_event_handler, LV_EVENT_ALL, NULL);
        
        app_obj_btn.app_name[lv_index] = lv_label_create(parent);
        lv_obj_set_style_text_color(app_obj_btn.app_name[lv_index],lv_color_hex(0xf0f0f0), LV_STATE_DEFAULT);
        
        if(lv_index < APP_NUM - 3)
        {
            lv_obj_set_style_text_font(app_obj_btn.app_name[lv_index], &myFont12, 0);
            lv_label_set_text(app_obj_btn.app_name[lv_index], app_image[lv_index].app_text_English);
        }
        else
        {
            lv_label_set_text(app_obj_btn.app_name[lv_index], " ");
        }
        
        lv_obj_align_to(app_obj_btn.app_name[lv_index],app_obj_btn.app_btn[lv_index],LV_ALIGN_OUT_BOTTOM_MID,0, 0);
    }
}

/**
  * @brief  Obtain RTC time information
  * @param  None
  * @retval None
  */
static void lv_rtc_timer(lv_timer_t* timer)
{
    rtc_get_time();
    app_obj_btn.rtc.year = calendar.year; 
    app_obj_btn.rtc.month = calendar.month; 
    app_obj_btn.rtc.date = calendar.date; 
    app_obj_btn.rtc.hour = calendar.hour; 
    app_obj_btn.rtc.minute = calendar.min; 
    app_obj_btn.rtc.second = calendar.sec; 
    app_obj_btn.rtc.week = calendar.week; 

    lv_label_set_text_fmt(status_bar_left,"%02d : %02d : %02d", app_obj_btn.rtc.hour, app_obj_btn.rtc.minute,app_obj_btn.rtc.second);
    lv_label_set_text_fmt(cpu_status, "%"LV_PRIu32"%% CPU", at_cpu);

    if (sd_check_en == 1)
    {
        lv_obj_set_style_text_color(sd_status_left,lv_color_make(255,255,255), LV_STATE_DEFAULT);

        if (sdmmc_get_status(card) != ESP_OK)
        {
            sd_check_en = 0;
        }
    }
    else
    {
        lv_obj_set_style_text_color(sd_status_left,lv_color_hex(0x292421), LV_STATE_DEFAULT);

        if (sd_spi_init() == ESP_OK)
        {
            if (sdmmc_get_status(card) == ESP_OK)
            {
                if (app_obj_general.current_parent != NULL)
                {
                    lv_obj_del(app_obj_general.current_parent);
                    app_obj_general.current_parent = NULL;
                }
                
                sd_check_en = 1;
            }
        }
    }
}

/**
  * @brief  app icon
  * @param  None
  * @retval None
  */
void lv_app_icon(lv_obj_t *praten)
{
    status_bar_left = lv_label_create(praten);
    lv_label_set_text(status_bar_left, " " );
    lv_obj_set_style_text_font(status_bar_left, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_align(status_bar_left, LV_ALIGN_TOP_RIGHT, -10, 0);
    lv_obj_set_style_text_color(status_bar_left, lv_color_hex(0xf0f0f0), LV_STATE_DEFAULT);

    cpu_status = lv_label_create(praten);
    lv_label_set_text(cpu_status, " ");
    lv_obj_set_style_text_font(cpu_status, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_align(cpu_status, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(cpu_status, lv_color_hex(0xf0f0f0), LV_STATE_DEFAULT);


    usb_status_left = lv_label_create(praten);
    lv_label_set_text(usb_status_left, LV_SYMBOL_USB);
    lv_obj_set_style_text_font(usb_status_left, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_align(usb_status_left, LV_ALIGN_TOP_LEFT, 10, 0);
    lv_obj_set_style_bg_color(usb_status_left,lv_color_hex(0xC0C0C0), LV_STATE_DEFAULT);

    sd_status_left = lv_label_create(praten);
    lv_label_set_text(sd_status_left, LV_SYMBOL_SD_CARD);
    lv_obj_set_style_text_font(sd_status_left, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_align_to(sd_status_left, usb_status_left,LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_set_style_bg_color(sd_status_left,lv_color_hex(0xC0C0C0), LV_STATE_DEFAULT);

    app_obj_btn.rtc.lv_rtc_timer = lv_timer_create(lv_rtc_timer, 500, NULL);
}

/**
  * @brief  Load main interface
  * @param  None
  * @retval None
  */
void lv_load_main_window(void)
{
    app_obj_general.del_parent = NULL;
    app_obj_general.current_parent = NULL;
    app_obj_general.fouc_parent = NULL;
    app_obj_general.Function = NULL;

    memset(&app_obj_btn,0,sizeof(app_obj_btn));
    
    lv_load_iamge();
    lv_obj_update_layout(lv_scr_act());
    
    app_obj_btn.lv_main_cont = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(app_obj_btn.lv_main_cont, lv_color_make(0,0,0), LV_STATE_DEFAULT);
    lv_obj_set_size(app_obj_btn.lv_main_cont, lcd_self.width, lcd_self.height);
    lv_obj_set_style_radius(app_obj_btn.lv_main_cont, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(app_obj_btn.lv_main_cont, LV_OPA_0, LV_STATE_DEFAULT);
    lv_obj_set_pos(app_obj_btn.lv_main_cont, 0, 0);

    lv_obj_clear_flag(app_obj_btn.lv_main_cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *down_con = lv_obj_create(app_obj_btn.lv_main_cont);
    lv_obj_set_size(down_con, lcd_self.width - 10, lcd_self.height / 4);
    lv_obj_align(down_con, LV_ALIGN_BOTTOM_MID, 0, -8);
    lv_obj_set_style_bg_color(down_con, lv_color_hex(0xFAF0E6), 0);
    lv_obj_set_style_bg_opa(down_con, 150, 0);
    lv_obj_set_style_border_opa(down_con, 0, 0);
    lv_obj_set_style_radius(down_con, 20, 0);
    
    lv_obj_t * up_con = lv_obj_create(lv_layer_sys());
    lv_obj_set_style_bg_color(up_con,lv_color_hex(0x000000),LV_STATE_DEFAULT);
    lv_obj_set_pos(up_con, 0, 0);
    lv_obj_move_foreground(up_con);
    lv_obj_set_style_radius(up_con, 0, LV_STATE_DEFAULT);
    lv_obj_set_size(up_con,lv_obj_get_width(lv_scr_act()),lv_font_montserrat_14.line_height + 5);
    lv_obj_set_style_border_opa(up_con,LV_OPA_0,LV_STATE_DEFAULT);

    lv_app_icon(up_con);
    
    lv_mid_cont_add_app(app_obj_btn.lv_main_cont);
}
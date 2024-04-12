/**
 ****************************************************************************************************
 * @file        lv_file_demo.c
 * @author      ALIENTEK
 * @brief       LVGL FILE Demo code
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

#include "lv_file_demo.h"


LV_FONT_DECLARE(myFont12)
LV_FONT_DECLARE(myFont24)
LV_FONT_DECLARE(myFont18)

extern lv_indev_t *indev_keypad;

lv_file_struct lv_flie;

char *lv_suffix [] ={".txt", ".avi", ".png", "jpeg", ".jpg", ".bin", ".gif", ".bmp", ".FON", ".dat", ".sif", ".BIN", ".xbf", ".ttf", ".wav", ".WAV"};
#define LV_SUFFIX(x)    (int)(sizeof(x)/sizeof(x[0]))

uint16_t lv_scan_files (const char *path, lv_obj_t *parent);
void lv_del_list(lv_obj_t *parent);
void lv_create_list(lv_obj_t *parent);
void list_init(lv_obj_t *parent);
lv_obj_t *lv_create_page(lv_obj_t *parent);
char *lv_pash_joint(void);

/**
 * @brief  Location of files
 * @param  fd:field pointer
 * @return Return to position
 */
long lv_tell(lv_fs_file_t *fd)
{
    uint32_t pos = 0;
    lv_fs_tell(fd, &pos);
    printf("\nlv_tcur pos is: %ld\n", pos);
    return pos;
}

/**
 * @brief  Read file content
 * @param  path:File path
 * @return LV_FS_RES_OK:Read successful
 */
lv_fs_res_t lv_file_read(const char *path)
{
    uint32_t rsize = 0;
    lv_fs_file_t fd;
    lv_fs_res_t res;

    res = lv_fs_open(&fd, path, LV_FS_MODE_RD);
    
    if (res != LV_FS_RES_OK)
    {
        printf("open %s ERROR\n",path);
        return LV_FS_RES_UNKNOWN;
    }

    lv_tell(&fd);
    lv_fs_seek(&fd,0,LV_FS_SEEK_SET);
    lv_tell(&fd);
    res = lv_fs_read(&fd, lv_flie.rbuf, FILE_SEZE, &rsize);

    if (res != LV_FS_RES_OK)
    {
        printf("read %s ERROR\n",path);
        return LV_FS_RES_UNKNOWN;
    }

    lv_tell(&fd);
    
    lv_fs_close(&fd);
    
    return LV_FS_RES_OK;
}

/**
  * @brief  Key callback function
  * @param  obj  : object
  * @param  event: event
  * @retval 无
  */
void lv_btn_close_event(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);

    if (code == LV_EVENT_RELEASED)
    {
        if (lv_flie.lv_image_read != NULL)
        {
            lv_obj_del(lv_flie.lv_image_read);
            lv_flie.lv_image_read = NULL;
        }
        
        lv_flie.lv_prev_file_flag -- ;
        
        lv_obj_del(lv_flie.lv_page_cont);
    }
}

/**
  * @brief  create page
  * @param  parent:parent
  * @retval None
  */
lv_obj_t *lv_create_page(lv_obj_t *parent)
{
    lv_flie.lv_page_cont = lv_obj_create(parent);
    lv_obj_set_size(lv_flie.lv_page_cont, lcd_self.width, lcd_self.height);
    lv_obj_set_style_radius(lv_flie.lv_page_cont, 0, LV_STATE_DEFAULT);
    lv_obj_clear_flag(lv_flie.lv_page_cont, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_clear_flag(lv_flie.lv_page_cont, LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_align_to(lv_flie.lv_page_cont, parent, LV_ALIGN_CENTER, 0, 0);
  
    lv_obj_t *lv_page_obj = lv_obj_create(lv_flie.lv_page_cont);
    lv_obj_set_style_bg_color(lv_page_obj, lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
    lv_obj_align(lv_page_obj, LV_ALIGN_BOTTOM_MID, 0, 10);
    lv_obj_set_size(lv_page_obj, lcd_self.width, myFont24.line_height);

    lv_obj_t *lv_page_back_btn = lv_label_create(lv_page_obj);
    lv_obj_set_style_text_font(lv_page_back_btn, &myFont24, LV_STATE_DEFAULT);
    lv_label_set_text(lv_page_back_btn, "Return");
    lv_obj_align_to(lv_page_back_btn, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(lv_page_back_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(lv_page_back_btn, lv_btn_close_event, LV_EVENT_ALL, NULL);

    return lv_flie.lv_page_cont;
}

/**
  * @brief  Display. txt file
  * @param  parent:parent
  * @retval None
  */
void lv_show_filetxt(lv_obj_t *parent)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_set_width(label, lv_obj_get_width(parent));
    lv_label_set_text(label, (char *)lv_flie.rbuf);
    memset(lv_flie.rbuf, 0, sizeof(lv_flie.rbuf));
}

/**
  * @brief  Display. bin image
  * @param  parent:parent
  * @param  path:File path
  * @retval None
  */
void lv_show_imgbin(lv_obj_t *parent, const char *path)
{
    lv_flie.lv_image_read = lv_img_create(parent);
    lv_img_set_src(lv_flie.lv_image_read,path);
    lv_obj_align_to(lv_flie.lv_image_read, parent, LV_ALIGN_CENTER, 0, 0);
}

/**
  * @brief  File path concatenation
  * @param  None
  * @retval None
  */
char *lv_pash_joint(void)
{
    lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag] = (char *)lv_flie.lv_pash;
    lv_flie.lv_prev_file_flag ++;
  
    strcpy((char *)lv_flie.pname, lv_flie.lv_pash);
    strcat((char *)lv_flie.pname, "/");
    strcat((char *)lv_flie.pname, (char *)lv_flie.lv_pname);
    return lv_flie.pname;
}

/**
  * @brief  List button callback function
  * @param  event: event
  * @retval None
  */
static void lv_list_btn_event(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);
  
    if(code == LV_EVENT_CLICKED)
    {
        for (int i = 0; i <= lv_flie.list_flie_nuber ;i++)
        {
            if (obj == lv_flie.list_btn[i])
            {   
                lv_flie.lv_pname = malloc(255);
                lv_flie.pname = malloc(255);

                lv_flie.lv_pname = (char *)lv_list_get_btn_text(lv_flie.list, lv_flie.list_btn[i]);
                
                for (int suffix = 0; suffix < LV_SUFFIX(lv_suffix); suffix ++)
                {
                    if (strstr(lv_flie.lv_pname, lv_suffix[suffix]) != NULL)
                    {
                        lv_flie.lv_suffix_flag = 0;
                        break;
                    }
                }

                if (lv_flie.lv_suffix_flag == 1)
                {   
                    lv_flie.lv_pash = lv_pash_joint();
                    lv_del_list(lv_flie.list);
                    lv_scan_files(lv_flie.pname, lv_scr_act());
                }
                else
                {
                    lv_flie.lv_suffix_flag = 1;
                }
            }
        }
    }
}

/**
  * @brief  Read file name
  * @param  path   : File path
  * @param  parent : parent
  * @retval FR_OK: Success, otherwise failure
  */
uint16_t lv_scan_files (const char *path, lv_obj_t *parent)
{
    lv_flie.fr = f_opendir(&lv_flie.lv_dir, path);
    memset(lv_flie.list_btn, 0, sizeof(lv_flie.list_btn));

    lv_flie.list_flie_nuber = 0;
    lv_create_list(parent);

    if (lv_flie.fr == FR_OK)
    {
        while(1)
        {
            lv_flie.fr = f_readdir(&lv_flie.lv_dir, &lv_flie.SD_fno);

            if ((lv_flie.fr) || lv_flie.SD_fno.fname[0] == 0) break;
            lv_flie.list_flie_nuber++;

            if (lv_flie.SD_fno.fattrib & AM_DIR)
            {
                lv_flie.list_btn[lv_flie.list_flie_nuber] = lv_list_add_btn(lv_flie.list, LV_SYMBOL_DIRECTORY, lv_flie.SD_fno.fname);
            }
            else
            {
                if (  strstr(lv_flie.SD_fno.fname,".png")  != NULL
                    ||strstr(lv_flie.SD_fno.fname,".jpeg") != NULL
                    ||strstr(lv_flie.SD_fno.fname,".jpg")  != NULL
                    ||strstr(lv_flie.SD_fno.fname,".bmp")  != NULL
                    ||strstr(lv_flie.SD_fno.fname,".gif")  != NULL
                    ||strstr(lv_flie.SD_fno.fname,".avi")  != NULL)
                {
                    lv_flie.image_scr = LV_SYMBOL_IMAGE;
                }
                else
                {
                    lv_flie.image_scr = "\xEF\x80\x96";
                }
                
                lv_flie.list_btn[lv_flie.list_flie_nuber] = lv_list_add_btn(lv_flie.list, lv_flie.image_scr, lv_flie.SD_fno.fname);
            }
            
            lv_obj_set_style_pad_left(lv_flie.list_btn[lv_flie.list_flie_nuber], 5, LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(lv_flie.list_btn[lv_flie.list_flie_nuber], 5, LV_STATE_DEFAULT);
            lv_obj_add_event_cb(lv_flie.list_btn[lv_flie.list_flie_nuber], lv_list_btn_event, LV_EVENT_ALL, NULL);
        }
        
        f_closedir(&lv_flie.lv_dir);
    }
    
    return lv_flie.fr;
}

/**
  * @brief  del list
  * @param  parent: parent
  * @retval None
  */
void lv_del_list(lv_obj_t *parent)
{
    lv_obj_del(parent);
    lv_flie.list = NULL;
}

/**
  * @brief  create list
  * @param  parent: parent
  * @retval None
  */
void lv_create_list(lv_obj_t *parent)
{
    lv_flie.list = lv_list_create(parent);
    app_obj_general.del_parent = lv_flie.list;
    lv_obj_set_size(lv_flie.list, lcd_self.width, lcd_self.height - myFont24.line_height * 2 - 28);
    lv_obj_align_to(lv_flie.list, lv_flie.lv_page_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_text_font(lv_flie.list, &myFont12, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lv_flie.list, 0, LV_STATE_DEFAULT);
}

/**
  * @brief  create page tile
  * @param  parent: parent
  * @retval None
  */
void lv_page_tile(lv_obj_t *parent,char * tile_name)
{
    lv_flie.lv_page_obj = lv_obj_create(parent);
    lv_obj_set_size(lv_flie.lv_page_obj, lcd_self.width + 5, myFont12.line_height + 5);
    lv_obj_set_style_bg_color(lv_flie.lv_page_obj,lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lv_flie.lv_page_obj, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(lv_flie.lv_page_obj, -2,18);
    lv_obj_set_style_bg_opa(lv_flie.lv_page_obj,LV_OPA_90,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(lv_flie.lv_page_obj,LV_GRAD_DIR_VER,LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(lv_flie.lv_page_obj,LV_OPA_0,LV_STATE_DEFAULT);

    lv_obj_t *lv_page_label = lv_label_create(lv_flie.lv_page_obj);
    lv_label_set_text(lv_page_label, tile_name);
    lv_obj_set_style_text_color(lv_page_label, lv_palette_main(LV_PALETTE_RED), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_page_label, &myFont12,LV_STATE_DEFAULT);
    lv_obj_align_to(lv_page_label, lv_flie.lv_page_obj, LV_ALIGN_CENTER, 0, -2);
}

/**
  * @brief  Return button callback function
  * @param  event:event
  * @retval None
  */
void lv_back_btn_event_handler(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);
  
    if(code == LV_EVENT_SHORT_CLICKED)
    {
        if (obj == lv_flie.lv_back_btn)
        {
            lv_del_list(lv_flie.list);
            list_init(lv_scr_act());
        }
        if (obj == lv_flie.lv_prev_btn)
        { 
            lv_flie.lv_prev_file_flag--;

            if (lv_flie.lv_prev_file_flag <= 0)
            {
                lv_flie.lv_prev_file_flag = 0;
            }
          
            lv_del_list(lv_flie.list);
            lv_flie.lv_pash = lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag];
            lv_scan_files(lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag], lv_scr_act());
        }
    }
}

/**
  * @brief  win create
  * @param  parent:parent
  * @retval None
  */
void lv_general_win_create(lv_obj_t *parent)
{
    lv_flie.lv_back_btn = lv_label_create(parent);
    lv_obj_set_style_text_font(lv_flie.lv_back_btn, &myFont24, LV_STATE_DEFAULT);

    lv_label_set_text(lv_flie.lv_back_btn, "Menu");
    lv_obj_set_style_text_color(lv_flie.lv_back_btn,lv_color_white(),LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_flie.lv_back_btn,lv_palette_main(LV_PALETTE_RED),LV_STATE_FOCUSED);
    lv_obj_align_to(lv_flie.lv_back_btn, parent, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_flag(lv_flie.lv_back_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(lv_flie.lv_back_btn, lv_back_btn_event_handler, LV_EVENT_ALL, NULL);

    
    lv_flie.lv_prev_btn = lv_label_create(parent);
    lv_obj_set_style_text_font(lv_flie.lv_prev_btn, &myFont24, LV_STATE_DEFAULT);
    lv_label_set_text(lv_flie.lv_prev_btn, "Return");
    lv_obj_set_style_text_color(lv_flie.lv_prev_btn,lv_color_white(),LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_flie.lv_prev_btn,lv_palette_main(LV_PALETTE_RED),LV_STATE_FOCUSED);
    lv_obj_align_to(lv_flie.lv_prev_btn, parent, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_flag(lv_flie.lv_prev_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(lv_flie.lv_prev_btn, lv_back_btn_event_handler, LV_EVENT_ALL, NULL);

    lv_group_add_obj(ctrl_g, lv_flie.lv_prev_btn);
    lv_group_add_obj(ctrl_g, lv_flie.lv_back_btn);
    lv_group_focus_obj(lv_flie.lv_prev_btn);
}

/**
  * @brief  Area for file system return/menu buttons
  * @param  parent: parent
  * @retval None
  */
void lv_page_back(lv_obj_t *parent)
{
    lv_flie.lv_back_obj = lv_obj_create(parent);
    lv_obj_set_size(lv_flie.lv_back_obj, lcd_self.width, myFont24.line_height + 10);
    lv_obj_set_style_bg_color(lv_flie.lv_back_obj,lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lv_flie.lv_back_obj, 0, LV_STATE_DEFAULT);
    lv_obj_align_to(lv_flie.lv_back_obj, parent, LV_ALIGN_BOTTOM_MID, 0, 5);
    lv_obj_set_style_bg_opa(lv_flie.lv_back_obj,LV_OPA_90,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(lv_flie.lv_back_obj,LV_GRAD_DIR_VER,LV_STATE_DEFAULT);
    lv_general_win_create(lv_flie.lv_back_obj);
    lv_obj_set_style_border_opa(lv_flie.lv_back_obj,LV_OPA_0,LV_STATE_DEFAULT);
}

/**
  * @brief  List initialization
  * @param  parent: parent
  * @retval None
  */
void list_init(lv_obj_t *parent)
{
    lv_flie.lv_pash = "0:";
    lv_flie.lv_prev_file_flag = 0;
    lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag] = "0:";
    lv_flie.list_flie_nuber = 0;
    lv_flie.lv_suffix_flag = 1;
    lv_scan_files(lv_flie.lv_pash,parent);
}

/**
  * @brief  del file
  * @param  None
  * @retval None
  */
void lv_file_del(void)
{
    if (app_obj_general.del_parent != NULL)
    {
        lv_del_list(lv_flie.list);
    }

    app_obj_general.del_parent = NULL;

    lv_group_remove_obj(lv_flie.lv_prev_btn);
    lv_group_remove_obj(lv_flie.lv_back_btn);
    lv_obj_clean(app_obj_general.current_parent);
    lv_obj_del(app_obj_general.current_parent);
    app_obj_general.current_parent = NULL;
    lv_app_show();
}

/**
  * @brief  file demo
  * @param  None
  * @retval None
  */
void lv_file_demo(void)
{
    esp_err_t err = ESP_OK;

    err = sd_spi_init();
    vTaskDelay(200);

    if (app_obj_general.current_parent != NULL)
    {
        lv_obj_del(app_obj_general.current_parent);
        app_obj_general.current_parent = NULL;
    }

    if (err != ESP_OK)
    {
        lv_msgbox("SD device not detected");
    }
    else
    {
        lv_app_del();
        lv_obj_t *file_obj = lv_obj_create(lv_scr_act());
        lv_obj_set_style_radius(file_obj, 0, LV_STATE_DEFAULT);
        lv_obj_set_size(file_obj,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
        lv_obj_set_style_border_opa(file_obj,LV_OPA_0,LV_STATE_DEFAULT);
        lv_obj_set_pos(file_obj,0,0);
        lv_obj_clear_flag(file_obj, LV_OBJ_FLAG_SCROLLABLE);

        app_obj_general.current_parent = file_obj;
        app_obj_general.Function = lv_file_del;
        lv_page_tile(file_obj,"TF Card Browser");
        lv_page_back(file_obj);
        list_init(file_obj);
    }
}

/**
 ****************************************************************************************************
 * @file        lv_file_demo.h
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


#ifndef __LV_FILE_DEMO_H
#define __LV_FILE_DEMO_H

#include "lvgl.h"
#include "ff.h"
#include "lcd.h"
#include "app_ui.h"


#define LIST_SIZE    100
#define FILE_SEZE    1992

typedef struct
{
    lv_obj_t * list;
    lv_obj_t *lv_page_obj;
    lv_obj_t *lv_client_page;
    lv_obj_t * list_btn[LIST_SIZE];
    uint8_t list_flie_nuber;
    FRESULT fr;
    FF_DIR lv_dir;
    FILINFO SD_fno;
    char *pname;
    char *lv_pname;
    char *lv_pname_shift;
    const char* lv_pash;
    int lv_suffix_flag;
    int lv_prev_file_flag;
    char *lv_prev_file[LIST_SIZE];
    const void *image_scr;
    lv_obj_t * lv_back_obj;
    lv_obj_t * lv_prev_btn;
    lv_obj_t * lv_back_btn;
    lv_obj_t *lv_page_cont;
    char rbuf[FILE_SEZE];
    lv_obj_t *lv_return_page;
    lv_obj_t *lv_image_read;
}lv_file_struct;

#define FILE_ASSERT(term)                                                                                   \
do                                                                                                          \
{                                                                                                           \
    if (!(term))                                                                                            \
    {                                                                                                       \
        printf("Assert failed. Condition(%s). [%s][%d]\r\n", term, __FUNCTION__, __LINE__);                 \
        while(1)                                                                                            \
        {                                                                                                   \
            ;                                                                                               \
        }                                                                                                   \
    }                                                                                                       \
} while (0)

/* function declaration */
void lv_file_demo(void);
void lv_page_tile(lv_obj_t *parent,char * tile_name);
#endif
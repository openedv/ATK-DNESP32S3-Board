/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       PICTURE example
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

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "led.h"
#include "spi.h"
#include "iic.h"
#include "lcd.h"
#include "xl9555.h"
#include "spi_sdcard.h"
#include "math.h"
#include "exfuns.h"
#include "piclib.h"
#include "ff.h"
#include "fonts.h"
#include "text.h"


i2c_obj_t i2c0_master;

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

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret = 0;
    uint8_t res = 0;
    FF_DIR picdir;
    FILINFO *picfileinfo;
    char *pname;
    uint16_t totpicnum;
    uint16_t curindex = 0;
    uint8_t key = 0;
    uint8_t pause = 0;
    uint8_t t;
    uint16_t temp;
    uint32_t *picoffsettbl;

    ret = nvs_flash_init();                             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                                         /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);                  /* Initialize IIC0 */
    spi2_init();                                        /* Initialize SPI2 */
    xl9555_init(i2c0_master);                           /* Initialize XL9555 */  
    lcd_init();                                         /* Initialize LCD */

    while (sd_spi_init())                               /* Initialize SD */
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);
        vTaskDelay(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        vTaskDelay(500);
    }

    res = exfuns_init();

    while (fonts_init())                                /* Initialize Font */
    {
        lcd_clear(WHITE);                               /* clear screen */
        lcd_show_string(30, 30, 200, 16, 16, "ESP32-S3", RED);
        
        key = fonts_update_font(30, 50, 16, (uint8_t *)"0:", RED);  /* update font library */

        while (key)
        {
            lcd_show_string(30, 50, 200, 16, 16, "Font Update Failed!", RED);
            vTaskDelay(200);
            lcd_fill(20, 50, 200 + 20, 90 + 16, WHITE);
            vTaskDelay(200);
        }

        lcd_show_string(30, 50, 200, 16, 16, "Font Update Success!   ", RED);
        vTaskDelay(1500);
        lcd_clear(WHITE);                               /* clear screen */
    }

    text_show_string(30, 50, 200, 16, "正点原子STM32开发板",16,0, RED);
    text_show_string(30, 70, 200, 16, "图片显示 实验", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "正点原子@ALIENTEK", 16, 0, RED);
    text_show_string(30, 110, 200, 16, "KEY0:NEXT KEY1:PREV", 16, 0, RED);
    text_show_string(30, 130, 200, 16, "KEY_UP:PAUSE:", 16, 0, RED);

    while (f_opendir(&picdir, "0:/PICTURE"))
    {
        text_show_string(30, 150, 240, 16, "PICTURE文件夹错误!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(30, 150, 240, 186, WHITE);
        vTaskDelay(200);
    }

    totpicnum = pic_get_tnum("0:/PICTURE");
 
    while (totpicnum == NULL)
    {
        text_show_string(30, 150, 240, 16, "没有图片文件!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(30, 150, 240, 186, WHITE);
        vTaskDelay(200);
    }

    picfileinfo = (FILINFO *)malloc(sizeof(FILINFO));
    pname = malloc(255 * 2 + 1);
    picoffsettbl = malloc(4 * totpicnum);

    while (!picfileinfo || !pname || !picoffsettbl)
    {
        text_show_string(30, 150, 240, 16, "内存分配失败!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(30, 150, 240, 186, WHITE);
        vTaskDelay(200);
    }

    res = f_opendir(&picdir, "0:/PICTURE");

    if (res == FR_OK)
    {
        curindex = 0;

        while (1)
        {
            temp = picdir.dptr;
            res = f_readdir(&picdir, picfileinfo);
            if (res != FR_OK || picfileinfo->fname[0] == 0)break;

            res = exfuns_file_type(picfileinfo->fname);

            if ((res & 0X0F) != 0X00)
            {
                picoffsettbl[curindex] = temp;
                curindex++;
            }
        }
    }

    text_show_string(30, 150, 240, 16, "开始显示...", 16, 0, RED);
    vTaskDelay(1500);
    piclib_init();
    curindex = 0;
    res = f_opendir(&picdir, (const TCHAR *)"0:/PICTURE");

    while (res == FR_OK)
    {
        dir_sdi(&picdir, picoffsettbl[curindex]);
        res = f_readdir(&picdir, picfileinfo);

        if (res != FR_OK || picfileinfo->fname[0] == 0)break;

        strcpy((char *)pname, "0:/PICTURE/");
        strcat((char *)pname, (const char *)picfileinfo->fname);
        lcd_clear(BLACK);
        piclib_ai_load_picfile(pname, 0, 0, lcd_self.width, lcd_self.height);
        text_show_string(2, 2, lcd_self.width, 16, (char *)pname, 16, 0, RED);
        t = 0;

        while (1)
        {
            key = xl9555_key_scan(0);

            if (t > 250)key = 1;

            if ((t % 20) == 0)
            {
                LED_TOGGLE();
            }

            if (key == KEY1_PRES)
            {
                if (curindex)
                {
                    curindex--;
                }
                else
                {
                    curindex = totpicnum - 1;
                }
                
                break;
            }
            else if (key == KEY0_PRES)
            {
                curindex++;

                if (curindex >= totpicnum)
                {
                    curindex = 0;
                }

                break;
            }
            else if (key == KEY3_PRES)
            {
                pause = !pause;
                LED(pause);
            }

            if (pause == 0)t++;

            vTaskDelay(10);
        }

        res = 0;
    }

    free(picfileinfo);
    free(pname);
    free(picoffsettbl);
}

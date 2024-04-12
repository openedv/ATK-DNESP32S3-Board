/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       RECORDER example
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
#include "fonts.h"
#include "spi_sdcard.h"
#include "text.h"
#include "exfuns.h"
#include "es8388.h"
#include "audioplay.h"
#include "recorder.h"
#include "i2s.h"


i2c_obj_t i2c0_master;

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;
    uint8_t key = 0;

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

    es8388_init(i2c0_master);                           /* Initialize ES8388 */
    es8388_hpvol_set(25);
    es8388_spkvol_set(25);
    xl9555_pin_write(SPK_EN_IO,0);                      /* Initialize XL9555 */
    i2s_init();                                         /* Initialize I2S */

    while (sd_spi_init())                               /* Initialize SD */
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);
        vTaskDelay(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        vTaskDelay(500);
    }

    while (fonts_init())                                /* Initialize Font */
    {
        lcd_clear(WHITE);
        lcd_show_string(30, 30, 200, 16, 16, "ESP32-S3", RED);
        
        key = fonts_update_font(30, 50, 16, (uint8_t *)"0:", RED);

        while (key)
        {
            lcd_show_string(30, 50, 200, 16, 16, "Font Update Failed!", RED);
            vTaskDelay(200);
            lcd_fill(20, 50, 200 + 20, 90 + 16, WHITE);
            vTaskDelay(200);
        }

        lcd_show_string(30, 50, 200, 16, 16, "Font Update Success!   ", RED);
        vTaskDelay(1500);
        lcd_clear(WHITE);
    }
    
    ret = exfuns_init();
    vTaskDelay(500);

    text_show_string(30, 50, 200, 16, "正点原子ESP32开发板", 16, 0, RED);
    text_show_string(30, 70, 200, 16, "WAV 录音机 实验", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "正点原子@ALIENTEK", 16, 0, RED);

    while (1)
    {
        wav_recorder();                                 /* recorder */
    }
}

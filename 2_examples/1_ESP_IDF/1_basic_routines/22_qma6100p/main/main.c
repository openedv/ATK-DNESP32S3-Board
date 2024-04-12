/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       QMA6100P example
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
#include "iic.h"
#include "spi.h"
#include "xl9555.h"
#include "lcd.h"
#include "qma6100p.h"


i2c_obj_t i2c0_master;

/**
 * @brief       show angle
 * @param       x, y : coordinates
 * @param       title: title
 * @param       angle: angle
 * @retval      None
 */
void user_show_mag(uint16_t x, uint16_t y, char *title, float val)
{
    char buf[20];

    sprintf(buf,"%s%3.1f", title, val);
    lcd_fill(x + 30, y + 16, x + 160, y + 16, WHITE);
    lcd_show_string(x, y, 160, 16, 16, buf, BLUE);
}

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    uint8_t t;
    qma6100p_rawdata_t xyz_rawdata;
    esp_err_t ret;
    
    ret = nvs_flash_init();             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                         /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);  /* Initialize IIC0 */
    spi2_init();                        /* Initialize SPI2 */
    xl9555_init(i2c0_master);           /* Initialize XL9555 */
    lcd_init();                         /* Initialize LCD */
    qma6100p_init(i2c0_master);         /* Initialize QMA6100P */
    
    lcd_show_string(30, 50, 200, 16, 16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, 16, "QMA6100P TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, " ACC_X :", RED);
    lcd_show_string(30, 130, 200, 16, 16, " ACC_Y :", RED);
    lcd_show_string(30, 150, 200, 16, 16, " ACC_Z :", RED);
    lcd_show_string(30, 170, 200, 16, 16, " Pitch :", RED);
    lcd_show_string(30, 190, 200, 16, 16, " Roll  :", RED);

    while (1)
    {
        vTaskDelay(10);
        t++;

        if (t == 20)
        {   
            qma6100p_read_rawdata(&xyz_rawdata);
            
            user_show_mag(30, 110, "ACC_X :", xyz_rawdata.acc_x);
            user_show_mag(30, 130, "ACC_Y :", xyz_rawdata.acc_y);
            user_show_mag(30, 150, "ACC_Z :", xyz_rawdata.acc_z);
            user_show_mag(30, 170, "Pitch :", xyz_rawdata.pitch);
            user_show_mag(30, 190, "Roll  :", xyz_rawdata.roll);
            
            t = 0;
            LED_TOGGLE();
        }
    }
}

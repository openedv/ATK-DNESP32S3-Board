/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       SD example
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "led.h"
#include "spi.h"
#include "iic.h"
#include "lcd.h"
#include "xl9555.h"
#include "spi_sdcard.h"


i2c_obj_t i2c0_master;

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;
    size_t bytes_total, bytes_free;

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

    lcd_show_string(30, 50, 200, 16, 16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, 16, "SD TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    while (sd_spi_init())                               /* Initialize SD */
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);
        vTaskDelay(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        vTaskDelay(500);
    }

    lcd_show_string(30, 110, 200, 16, 16, "SD Card OK!", RED);
    lcd_show_string(30, 130, 200, 16, 16, "Total:      MB", RED);
    lcd_show_string(30, 150, 200, 16, 16, "Free :      MB", RED);
    sd_get_fatfs_usage(&bytes_total, &bytes_free);

    lcd_show_num(80, 130,(int)bytes_total / 1024,5,16,BLUE);
    lcd_show_num(80, 150,(int)bytes_free / 1024,5,16,BLUE);

    while (1)
    {
        LED_TOGGLE();
        vTaskDelay(500);
    }
}

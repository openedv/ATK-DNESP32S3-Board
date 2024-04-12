/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       RNG example
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
#include "esp_log.h"
#include "led.h"
#include "iic.h"
#include "spi.h"
#include "xl9555.h"
#include "lcd.h"
#include "rng.h"
#include "key.h"


i2c_obj_t i2c0_master;

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    uint8_t key;
    uint32_t random;
    uint8_t t = 0;
    esp_err_t ret;
    
    ret = nvs_flash_init();                                         /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                                                     /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);                              /* Initialize IIC0 */
    spi2_init();                                                    /* Initialize SPI2 */
    xl9555_init(i2c0_master);                                       /* Initialize XL9555 */
    lcd_init();                                                     /* Initialize LCD */
    key_init();                                                     /* Initialize KEY */
    
    lcd_show_string(30, 50, 200, 16, 16, "ESP32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "RNG TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "RNG Ready!   ", RED);
    lcd_show_string(30, 130, 200, 16, 16, "BOOT:Get Random Num", RED);
    lcd_show_string(30, 150, 200, 16, 16, "Random Num:", RED);
    lcd_show_string(30, 180, 200, 16, 16, "Random Num[0-9]:", RED);

    while(1)
    {
        key = key_scan(0);
        
        if (key == BOOT)
        {
            random = rng_get_random_num();
            lcd_show_num(30 + 8 * 11, 150, random, 10, 16, BLUE);
        }
        
        if ((t % 20) == 0)
        {
            LED_TOGGLE();
            random = rng_get_random_range(0, 9);
            lcd_show_num(30 + 8 * 16, 180, random, 1, 16, BLUE);
        }

        vTaskDelay(10);
        t++;
    }
}

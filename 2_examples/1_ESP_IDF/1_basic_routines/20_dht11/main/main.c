/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       DHT11 example
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
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "led.h"
#include "iic.h"
#include "spi.h"
#include "lcd.h"
#include "xl9555.h"
#include "dht11.h"


i2c_obj_t i2c0_master;

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    uint8_t err;
    uint8_t t = 0;
    uint8_t temperature;
    uint8_t humidity;
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

    lcd_show_string(30, 50, 200, 16, 16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DHT11 TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    err = dht11_init();
    
    if (err != 0)
    {
        while (1)
        {
            lcd_show_string(30, 110, 200, 16, 16, "DHT11 Error", RED);
            vTaskDelay(200);
            lcd_fill(30, 110, 239, 130 + 16, WHITE);
            vTaskDelay(200);
        }
    }

    lcd_show_string(30, 110, 200, 16, 16, "DHT11 OK", RED);
    lcd_show_string(30, 130, 200, 16, 16, "Temp:  C", BLUE);
    lcd_show_string(30, 150, 200, 16, 16, "Humi:  %", BLUE);
    
    while (1)
    {
        if (t % 10 == 0)
        {
            dht11_read_data(&temperature, &humidity);
            lcd_show_num(30 + 40, 130, temperature, 2, 16, BLUE);
            lcd_show_num(30 + 40, 150, humidity, 2, 16, BLUE);
        }

        vTaskDelay(10);
        t++;

        if (t == 20)
        {
            t = 0;
            LED_TOGGLE();
        }
    }
}

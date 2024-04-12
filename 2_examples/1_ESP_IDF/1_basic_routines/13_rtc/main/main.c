/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       RTC example
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
#include "esp_system.h"
#include "nvs_flash.h"
#include "led.h"
#include "lcd.h"
#include "esp_rtc.h"


i2c_obj_t i2c0_master;

char* weekdays[]={"Sunday","Monday","Tuesday","Wednesday",
                  "Thursday","Friday","Saterday"};

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;
    uint8_t tbuf[40];
    uint8_t t = 0;
    

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
    rtc_set_time(2023,8,26,00,00,00);   /* set time */

    lcd_show_string(10, 40, 240, 32, 32, "ESP32-S3",RED);
    lcd_show_string(10, 80, 240, 24, 24, "RTC Test",RED);
    lcd_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK",RED);

    while (1)
    {
        t++;

        if ((t % 10) == 0)
        {
            rtc_get_time();
            sprintf((char *)tbuf, "Time:%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
            printf("Time:%02d:%02d:%02d\r\n", calendar.hour, calendar.min, calendar.sec);
            lcd_show_string(10, 130, 210, 16, 16, (char *)tbuf,BLUE);
            sprintf((char *)tbuf, "Date:%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
            printf("Date:%02d-%02d-%02d\r\n",  calendar.year,  calendar.month,  calendar.date);
            lcd_show_string(10, 150, 210, 16, 16, (char *)tbuf,BLUE);
            sprintf((char *)tbuf, "Week:%s", weekdays[calendar.week - 1]);
            lcd_show_string(10, 170, 210, 16, 16, (char *)tbuf,BLUE);
        }

        if ((t % 20) == 0)
        {
            LED_TOGGLE();
        }

        vTaskDelay(10);
    }
}

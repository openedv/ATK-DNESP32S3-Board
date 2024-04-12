/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       TUD_FLASH example
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
#include "tud_flash.h"


i2c_obj_t i2c0_master;

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;

    ret = nvs_flash_init();                             /* Initialize NVS */
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    led_init();                                         /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);                  /* Initialize IIC0 */
    spi2_init();                                        /* Initialize SPI2 */
    xl9555_init(i2c0_master);                           /* Initialize XL9555 */
    lcd_init();                                         /* Initialize LCD */
    
    lcd_show_string(30, 50, 200, 16, 16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, 16, "USB FLASH TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "status:", RED);
    tud_usb_flash();                                    /* Initialize USB */

    while(1)
    {
        if ((g_usbdev.status & 0x0f) == 0x01)
        {
            lcd_show_string(110, 110, lcd_self.width, 16, 16, "connect success.....", BLUE);
        }
        else if ((g_usbdev.status & 0x0f) == 0x00)
        {
            lcd_show_string(110, 110, lcd_self.width, 16, 16, "connect fail........", BLUE);
        }

        LED_TOGGLE();
        vTaskDelay(500);
    }
}

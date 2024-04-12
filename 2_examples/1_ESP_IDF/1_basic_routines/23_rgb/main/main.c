/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       LTDC example
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
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "led.h"
#include "ltdc.h"


i2c_obj_t i2c0_master;

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    uint8_t x = 0;
    uint8_t lcd_id[12];
    esp_err_t ret;
    
    ret = nvs_flash_init();                                             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                                                         /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);                                  /* Initialize IIC0 */
    xl9555_init(i2c0_master);                                           /* Initialize XL9555 */
    ltdc_init();                                                        /* Initialize LTDC */

    sprintf((char *)lcd_id, "LCD ID:%04X", ltdcdev.id);

    while (1)
    {
        switch (x)
        {
            case 0:
                ltdc_clear(WHITE);
                break;

            case 1:
                ltdc_clear(BLACK);
                break;

            case 2:
                ltdc_clear(BLUE);
                break;

            case 3:
                ltdc_clear(RED);
                break;

            case 4:
                ltdc_clear(MAGENTA);
                break;

            case 5:
                ltdc_clear(GREEN);
                break;

            case 6:
                ltdc_clear(CYAN);
                break;

            case 7:
                ltdc_clear(YELLOW);
                break;
        }

        ltdc_show_string(10, 40, 240, 32, 32, "ESP32S3", RED);
        ltdc_show_string(10, 80, 240, 24, 24, "LTDC TEST", RED);
        ltdc_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK", RED);
        ltdc_show_string(10, 130, 240, 16, 16, (char *)lcd_id, RED);

        x++;

        if (x == 8)
        {
            x = 0;
        }

        vTaskDelay(1000);
    }
}

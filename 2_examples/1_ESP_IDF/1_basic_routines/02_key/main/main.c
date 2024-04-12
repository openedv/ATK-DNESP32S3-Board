/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       KEY example
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
#include "key.h"


/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    uint8_t key;
    esp_err_t ret;
    
    ret = nvs_flash_init();     /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                 /* Initialize LED */
    key_init();                 /* Initialize KEY */

    while(1)
    {
        key = key_scan(0);

        switch (key)
        {
            case BOOT_PRES:
            {
                LED_TOGGLE();
                break;
            }
            default:
            {
                break;
            }
        }

        vTaskDelay(10);
    }
}

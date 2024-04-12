/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       WDT example
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
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "led.h"
#include "key.h"
#include "wdt.h"


/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t rets;
    
    rets = nvs_flash_init();            /* Initialize NVS */

    if (rets == ESP_ERR_NVS_NO_FREE_PAGES || rets == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        rets = nvs_flash_init();
    }

    led_init();                         /* Initialize LED */
    key_init();                         /* Initialize KEY */
    wdt_init(1000000);                  /* Initialize Timer */
    LED(0);

    while (1)
    {
        if (key_scan(0) == BOOT_PRES)
        {
            restart_timer(1000000);     /* Restart Timer */
        }

        vTaskDelay(10);
    }
}

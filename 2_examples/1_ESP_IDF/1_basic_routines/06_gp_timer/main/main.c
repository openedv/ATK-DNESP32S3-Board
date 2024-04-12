/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       GPTIM example
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
#include "esp_log.h"
#include "led.h"
#include "gptim.h"


/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    uint8_t record;
    esp_err_t ret;
    gptimer_event_t g_tim_evente;
    
    ret = nvs_flash_init();         /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    led_init();                     /* Initialize LED */
    gptim_int_init(100,1000000);        /* Initialize GPTIM */
    
    while (1)
    {
        record = 1;

        if (xQueueReceive(queue, &g_tim_evente, 2000))
        {
            ESP_LOGI("GPTIMER_ALARM", "Timer reloaded, count=%llu", g_tim_evente.event_count);
            record--;
        }
        else
        {
            ESP_LOGW("GPTIMER_ALARM", "Missed one count event");
        }
    }

    vQueueDelete(queue);
}

/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       LED example
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
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_psram.h"
#include "esp_flash.h"


/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;
    uint32_t flash_size;
    esp_chip_info_t chip_info;                                      /* print chip information */

    ret = nvs_flash_init();                                         /* initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    esp_flash_get_size(NULL, &flash_size);

    esp_chip_info(&chip_info);
    printf("chip with %d CPU cores\n",chip_info.cores);             /* number of CPU cores */
    printf("External-Flash:%ld MB \n",flash_size / (1024 * 1024));  /* get flash size */
    printf("External-PSRAM:%d MB\n", esp_psram_get_size());         /* get PSRAM size */

    while(1)
    {
        printf("Hello-ESP32\r\n");
        vTaskDelay(1000);                                           /* sleep for 1 second */
    }
}

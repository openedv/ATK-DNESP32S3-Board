/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       HW_PWM example
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

#include "nvs_flash.h"
#include "pwm.h"


/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;

    ret = nvs_flash_init();             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    pwm_init(13, 5000);                 /* Initialize PWM */

    while(1) 
    {
        vTaskDelay(10);
        pwm_set_duty(LEDC_PWM_DUTY);    /* Set duty */
    }
}

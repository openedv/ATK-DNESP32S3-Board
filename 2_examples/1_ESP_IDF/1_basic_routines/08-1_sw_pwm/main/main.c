/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       SW_PWM example
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
    uint8_t dir = 1;
    uint16_t ledpwmval = 0;

    ret = nvs_flash_init(); /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    pwm_init(10, 1000);     /* Initialize PWM */

    while(1) 
    {
        vTaskDelay(10);

        if (dir == 1)
        {
            ledpwmval += 5; /* dir==1 ledpwmval+5 */
        }
        else
        {
            ledpwmval -= 5; /* dir==0 ledpwmval-5 */
        }

        if (ledpwmval > 1005)
        {
            dir = 0;
        }

        if (ledpwmval < 5)
        {
            dir = 1;
        }

        /* Set duty */
        pwm_set_duty(ledpwmval);
    }
}

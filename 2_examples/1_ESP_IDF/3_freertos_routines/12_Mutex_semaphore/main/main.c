/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       freertos example
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
#include "led.h"
#include "lcd.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "freertos_demo.h"


i2c_obj_t i2c0_master;

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;
    
    ret= nvs_flash_init();              /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    led_init();                         /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);  /* Initialize IIC0 */
    spi2_init();                        /* Initialize SPI2 */
    xl9555_init(i2c0_master);           /* Initialize XL9555 */
    lcd_init();                         /* Initialize SPILCD */

    freertos_demo();                    /* Run FreeRTOS demo */
}

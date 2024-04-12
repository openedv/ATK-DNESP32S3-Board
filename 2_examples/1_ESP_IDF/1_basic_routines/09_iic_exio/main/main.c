/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       EXIO example
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
#include "iic.h"
#include "xl9555.h"


i2c_obj_t i2c0_master;

/**
 * @brief       Display experimental information
 * @param       None
 * @retval      None
 */
void show_mesg(void)
{
    printf("\n");
    printf("********************************\n");
    printf("ESP32-S3\n");
    printf("EXIO TEST\n");
    printf("ATOM@ALIENTEK\n");
    printf("KEY0:Beep On, KEY1:Beep Off\n");
    printf("KEY2:LED On, KEY3:LED Off\n");
    printf("********************************\n");
    printf("\n");
}

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    uint8_t key;
    esp_err_t ret;
    
    ret = nvs_flash_init();             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                         /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);  /* Initialize IIC0 */
    xl9555_init(i2c0_master);           /* Initialize XL9555 */
    show_mesg();                        /* display experimental information */

    while(1)
    {
        key = xl9555_key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                printf("KEY0 has been pressed \n");
                xl9555_pin_write(BEEP_IO, 0);
                break;
            }
            case KEY1_PRES:
            {
                printf("KEY1 has been pressed \n");
                xl9555_pin_write(BEEP_IO, 1);
                break;
            }
            case KEY2_PRES:
            {
                printf("KEY2 has been pressed \n");
                LED(0);
                break;
            }
            case KEY3_PRES:
            {
                printf("KEY3 has been pressed \n");
                LED(1);
                break;
            }
            default:
            {
                break;
            }
        }

        vTaskDelay(200);
    }
}

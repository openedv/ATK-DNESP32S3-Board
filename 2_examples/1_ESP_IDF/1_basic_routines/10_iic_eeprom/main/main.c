/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       EERPOM example
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
#include "iic.h"
#include "xl9555.h"
#include "24cxx.h"


i2c_obj_t i2c0_master;

const uint8_t g_text_buf[] = {"ESP32-S3 EEPROM"};   /* write to 24c02 data */
#define TEXT_SIZE   sizeof(g_text_buf)              /* write data length */

/**
 * @brief   Display experimental information
 * @param   None
 * @retval  None
 */
void show_mesg(void)
{
    printf("\n");
    printf("********************************\n");
    printf("ESP32\n");
    printf("IIC EEPROM TEST\n");
    printf("ATOM@ALIENTEK\n");
    printf("KEY0:Write Data,KEY1:Read Data\n");
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
    uint16_t i = 0;
    uint8_t err = 0;
    uint8_t key;
    uint8_t datatemp[TEXT_SIZE];
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
    at24cxx_init(i2c0_master);          /* Initialize 24CXX */
    show_mesg();                        /* Display experimental information */

    err = at24cxx_check();              /* check 24c02 */
    
    if (err != 0)
    {
        while (1)
        {
            printf("24C02 check failed, please check!\n");
            vTaskDelay(500);
            LED_TOGGLE();
        }
    }

    printf("24C02 Ready!\n");
    printf("\n");

    while(1)
    {
        key = xl9555_key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                at24cxx_write(0, (uint8_t *)g_text_buf, TEXT_SIZE);
                printf("The data written is:%s\n", g_text_buf);
                break;
            }
            case KEY1_PRES:
            {
                at24cxx_read(0, datatemp, TEXT_SIZE);
                printf("The data read is:%s\n", datatemp);
                break;
            }
            default:
            {
                break;
            }
        }

        i++;

        if (i == 20)
        {
            LED_TOGGLE();
            i = 0;
        }
        
        vTaskDelay(10);
    }
}

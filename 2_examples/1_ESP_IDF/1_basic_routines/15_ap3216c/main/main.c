/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       AP3216C example
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
#include "lcd.h"
#include "spi.h"
#include "iic.h"
#include "xl9555.h"
#include "ap3216c.h"


i2c_obj_t i2c0_master;

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    uint16_t ir, als, ps;
    esp_err_t ret;
    
    ret = nvs_flash_init();                             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                                         /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);                  /* Initialize IIC0 */
    spi2_init();                                        /* Initialize SPI2 */
    xl9555_init(i2c0_master);                           /* Initialize XL9555 */
    lcd_init();                                         /* Initialize LCS */
    ap3216c_init(i2c0_master);                          /* Initialize AP3216C */

    lcd_show_string(10, 50, 200, 16, 16, "ESP32-S3", RED);
    lcd_show_string(10, 70, 200, 16, 16, "AP3216C TEST", RED);
    lcd_show_string(10, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    lcd_show_string(10, 130, 200, 16, 16, "AP3216C Ready!", RED);
    lcd_show_string(10, 150, 200, 16, 16, "IR :", RED);
    lcd_show_string(10, 170, 200, 16, 16, "PS :", RED);
    lcd_show_string(10, 190, 200, 16, 16, "ALS:", RED);

    while(1)
    {
        ap3216c_read_data(&ir, &ps, &als);              /* read data */
        lcd_show_num(10 + 40, 150, ir, 5, 16, BLUE);    /* lcd display IR */
        lcd_show_num(10 + 40, 170, ps, 5, 16, BLUE);    /* lcd display PS */
        lcd_show_num(10 + 40, 190, als, 5, 16, BLUE);   /* lcd display ALS */

        LED_TOGGLE();
        vTaskDelay(200);
    }
}

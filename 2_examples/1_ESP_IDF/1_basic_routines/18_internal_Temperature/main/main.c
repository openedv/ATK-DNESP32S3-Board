/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       内部温度传感器实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "led.h"
#include "lcd.h"
#include "sensor.h"


i2c_obj_t i2c0_master;

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    int16_t temp;
    esp_err_t ret;

    ret = nvs_flash_init();                                                     /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                                                                 /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);                                          /* Initialize IIC0 */
    spi2_init();                                                                /* Initialize SPI2 */
    xl9555_init(i2c0_master);                                                   /* Initialize XL9555 */
    lcd_init();                                                                 /* Initialize LCD */
    temperature_sensor_init();                                                  /* Initialize sensor */
    
    lcd_show_string(30, 50, 200, 16, 16, "ESP32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "Temperature TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 120, 200, 16, 16, "TEMPERATE: 00.00C", BLUE);

    while(1)
    {
        temp = sensor_get_temperature();

        if (temp < 0)
        {
            temp = -temp;
            lcd_show_string(30 + 10 * 8, 120, 16, 16, 16, "-", BLUE);
        }
        else
        {
            lcd_show_string(30 + 10 * 8, 120, 16, 16, 16, " ", BLUE);
        }
        lcd_show_xnum(30 + 11 * 8, 120, temp, 2, 16, 0, BLUE);
        lcd_show_xnum(30 + 14 * 8, 120, temp * 100 % 100, 2, 16, 0x80, BLUE);
        
        LED_TOGGLE();
        vTaskDelay(250);
    }
}

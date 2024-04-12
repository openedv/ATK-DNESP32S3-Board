/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       ADC code
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
#include "lcd.h"
#include "adc1.h"


i2c_obj_t i2c0_master;

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    uint16_t adcdata;
    float voltage;
    esp_err_t ret;
    
    
    ret = nvs_flash_init();                                     /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                                                 /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);                          /* Initialize IIC0 */
    spi2_init();                                                /* Initialize SPI2 */
    xl9555_init(i2c0_master);                                   /* Initialize XL9555 */
    lcd_init();                                                 /* Initialize LCD */
    adc_init();                                                 /* Initialize ADC */

    lcd_show_string(10, 50, 200, 16, 16, "ESP32-S3", RED);
    lcd_show_string(10, 70, 200, 16, 16, "ADC TEST", RED);
    lcd_show_string(10, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(10, 110, 200, 16, 16, "ADC1_CH7_VAL:0", BLUE);
    lcd_show_string(10, 130, 200, 16, 16, "ADC1_CH7_VOL:0.000V", BLUE);
    
    while(1)
    {
        adcdata = adc_get_result_average(ADC_ADCX_CHY, 20);
        lcd_show_xnum(114, 110, adcdata, 5, 16, 0, BLUE);

        voltage = (float)adcdata * (3.3 / 4095);
        adcdata = voltage;
        lcd_show_xnum(114, 130, adcdata, 1, 16, 0, BLUE);
        
        voltage -= adcdata;
        voltage *= 1000;
        lcd_show_xnum(130, 130, voltage, 3, 16, 0x80, BLUE);
    
        LED_TOGGLE();
        vTaskDelay(100);
    }
}

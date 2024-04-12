/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       BOOTLOADER code
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
#include "iic.h"
#include "xl9555.h"
#include "lvgl_demo.h"
#include "spi.h"
#include "key.h"
#include "led.h"
#include "24cxx.h"
#include "manage.h"
#include "app_test.h"
#include "adc1.h"
#include "qma6100p.h"
#include "es8388.h"
#include "i2s.h"
#include "exfuns.h"
#include "esp_rtc.h"


i2c_obj_t i2c0_master;

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

    i2c0_master = iic_init(I2C_NUM_0);  /* Initialize IIC0 */
    spi2_init();                        /* Initialize SPI2 */
    led_init();                         /* Initialize LED */
    key_init();                         /* Initialize KEY */
    xl9555_init(i2c0_master);           /* Initialize XL9555 */
    at24cxx_init(i2c0_master);          /* Initialize AT24C02 */
    ap3216c_init(i2c0_master);          /* Initialize AP3216C */
    qma6100p_init(i2c0_master);         /* Initialize QMA6100P */
    es8388_init(i2c0_master);           /* Initialize ES8388 */
    rtc_set_time(2023,12,26,11,32,00);
    lcd_init();                         /* Initialize NVS */
    exfuns_init();
    func_test();
    lvgl_demo();
}

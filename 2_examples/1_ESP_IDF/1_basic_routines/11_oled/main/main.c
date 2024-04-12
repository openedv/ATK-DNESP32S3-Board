/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       OLED example
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

#include "driver/gpio.h"
#include "led.h"
#include "iic.h"
#include "xl9555.h"
#include "oled.h"


i2c_obj_t i2c0_master;
i2c_obj_t i2c1_master;

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    uint8_t t = 0;

    led_init();                             /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);      /* Initialize IIC0 */
    i2c1_master = iic_init(I2C_NUM_1);      /* Initialize IIC1 */
    xl9555_init(i2c0_master);               /* Initialize XL9555 */
    oled_init(i2c1_master);                 /* Initialize OLED */

    oled_show_string(0, 0, "ALIENTEK", 24);
    oled_show_string(0, 24, "0.96' OLED TEST", 16);
    oled_show_string(0, 40, "ATOM 2023/8/26", 12);
    oled_show_string(0, 52, "ASCII:", 12);
    oled_show_string(64, 52, "CODE:", 12);
    oled_refresh_gram();
    
    t = ' ';
    while(1)
    {
        oled_show_char(36, 52, t, 12, 1);
        oled_show_num(94, 52, t, 3, 12);
        oled_refresh_gram();
        t++;

        if (t > '~')
        {
            t = ' ';
        }

        vTaskDelay(500);
        LED_TOGGLE();
    }
}

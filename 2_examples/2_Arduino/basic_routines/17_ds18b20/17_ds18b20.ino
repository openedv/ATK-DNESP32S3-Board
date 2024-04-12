/**
 ****************************************************************************************************
 * @file        17_ds18b20.ino
 * @author      ALIENTEK
 * @brief       DS18B20 example
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

#include "led.h"
#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "ds18b20.h"


short temperature;
uint8_t t = 0;

/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    led_init();             /* led initialization */
    uart_init(0, 115200);   /* uart0 initialization */
    xl9555_init();          /* xl9555 initialization */
    lcd_init();             /* lcd initialization */
    ds18b20_init();         /* ds18b20 initialization */

    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "DS18B20 TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "Temp:   . C", BLUE);
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    if (t % 10 == 0)
    {
        temperature = ds18b20_get_temperature();

        if (temperature < 0)
        {
            lcd_show_char(30 + 40, 110, '-', LCD_FONT_16, 0, BLUE); 
            temperature = -temperature;                    
        }
        else
        {
            lcd_show_char(30 + 40, 110, ' ', LCD_FONT_16, 0, BLUE); 
        }

        lcd_show_num(30 + 40 + 8, 110, temperature / 10, 2, LCD_FONT_16, BLUE); 
        lcd_show_num(30 + 40 + 32, 110, temperature % 10, 1, LCD_FONT_16, BLUE); 
    }

    delay(10);

    t++;
    if (t == 20)
    {
        t = 0;
        LED_TOGGLE();
    }
}

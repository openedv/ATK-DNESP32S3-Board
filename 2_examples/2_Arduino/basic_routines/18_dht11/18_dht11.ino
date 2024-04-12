/**
 ****************************************************************************************************
 * @file        18_dht11.ino
 * @author      ALIENTEK
 * @brief       DHT11 example
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
#include "dht11.h"


uint8_t temperature;
uint8_t humidity;
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
    dht11_init();           /* dht11 initialization */

    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "DHT11 TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "Temp:  C", BLUE);
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "Humi:  %", BLUE);
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
        dht11_read_data(&temperature, &humidity);                       /* read temperature and humidity */
        lcd_show_num(30 + 40, 110, temperature, 2, LCD_FONT_16, BLUE);  /* lcd display temperature */
        lcd_show_num(30 + 40, 130, humidity, 2, LCD_FONT_16, BLUE);     /* lcd display humidity  */
    }

    delay(10);
    t++;

    if (t == 20)
    {
        t = 0;
        LED_TOGGLE();
    }
}

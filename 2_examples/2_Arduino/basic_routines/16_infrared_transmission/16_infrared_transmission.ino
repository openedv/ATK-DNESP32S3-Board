/**
 ****************************************************************************************************
 * @file        16_infrared_transmission.ino
 * @author      ALIENTEK
 * @brief       INFRARED_TRANSMISSION example
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
#include "remote.h"
#include "emission.h"


uint8_t key_value = 0;
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
    remote_init();          /* remote initialization */
    emission_init();        /* emission initialization */

    lcd_show_string(30,  50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30,  70, 200, 16, LCD_FONT_16, "EMISSION TEST", RED);
    lcd_show_string(30,  90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "TX KEYVAL:", RED);
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "RX KEYVAL:", RED);
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    t++;
    if (t == 0)
    {
        t = 1;
    }

    emission_send(t);               /* infrared transmitting key value */
    lcd_show_num(110, 110, t, 3, LCD_FONT_16, BLUE);  

    key_value = remote_scan();      /* nfrared receive key value */
    if (key_value)
    {
        lcd_show_num(110, 130, key_value, 3, LCD_FONT_16, BLUE);
    }

    LED_TOGGLE();
    delay(200);
}

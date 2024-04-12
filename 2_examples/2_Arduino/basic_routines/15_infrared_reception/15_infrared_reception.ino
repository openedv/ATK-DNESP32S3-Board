/**
 ****************************************************************************************************
 * @file        15_infrared_reception.ino
 * @author      ALIENTEK
 * @brief       INFRARED_RECEPTION example
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


uint8_t rmt_key;
char *str = "0";

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
    
    lcd_show_string(30,  50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30,  70, 200, 16, LCD_FONT_16, "REMOTE TEST", RED);
    lcd_show_string(30,  90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "KEYVAL:", RED);
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "SYMBOL:", RED);
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    rmt_key  = remote_scan();

    if (rmt_key)
    {
        lcd_show_num(86, 110, rmt_key, 3, LCD_FONT_16, BLUE);

        switch (rmt_key)
        {
            case 0:
                str = "ERROR";
                break;

            case 162:
                str = "POWER";
                break;

            case 98:
                str = "UP";
                break;

            case 2:
                str = "PLAY";
                break;

            case 226:
                str = "ALIENTEK";
                break;

            case 194:
                str = "RIGHT";
                break;

            case 34:
                str = "LEFT";
                break;

            case 224:
                str = "VOL-";
                break;

            case 168:
                str = "DOWN";
                break;

            case 144:
                str = "VOL+";
                break;

            case 104:
                str = "1";
                break;

            case 152:
                str = "2";
                break;

            case 176:
                str = "3";
                break;

            case 48:
                str = "4";
                break;

            case 24:
                str = "5";
                break;

            case 122:
                str = "6";
                break;

            case 16:
                str = "7";
                break;

            case 56:
                str = "8";
                break;

            case 90:
                str = "9";
                break;

            case 66:
                str = "0";
                break;

            case 82:
                str = "DELETE";
                break;
        }

        lcd_fill(86, 130, 116 + 8 * 8, 170 + 16, WHITE);
        lcd_show_string(86, 130, 200, 16, LCD_FONT_16, str, BLUE);
    }

    LED_TOGGLE();
    delay(500);
}

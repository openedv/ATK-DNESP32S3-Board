/**
 ****************************************************************************************************
 * @file        20_spi_sdcard.ino
 * @author      ALIENTEK
 * @brief       SPI_SDCARD example
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
#include "key.h"
#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "spi_sdcard.h"
#include <SD.h>


/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    led_init();             /* led initialization */
    key_init();             /* key initialization */
    uart_init(0, 115200);   /* uart0 initialization */
    xl9555_init();          /* xl9555 initialization */
    lcd_init();             /* lcd initialization */

    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "SD TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    while (sdcard_init())    /* check sd? */    
    {
        lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "SD Card Error!", RED);
        delay(500);
        lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "Please Check! ", RED);
        delay(500);
        LED_TOGGLE();
    }

    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "SD Card OK    ", BLUE);
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "SD Card Size:     MB", BLUE);
    lcd_show_num(30 + 13 * 8, 130, SD.cardSize() / (1024 * 1024), 5, LCD_FONT_16, BLUE); 
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    if (KEY == 0)
    {
        sd_test();
    }

    LED_TOGGLE();    
    delay(500);
}

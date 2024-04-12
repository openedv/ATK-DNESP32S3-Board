/**
 ****************************************************************************************************
 * @file        11_spi_lcd.ino
 * @author      ALIENTEK
 * @brief       SPILCD example
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

#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "alientek_logo.h"
#include "demo_show.h"


/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    uart_init(0, 115200);   /* uart0 initialization */
    xl9555_init();          /* xl9555 initialization */
    lcd_init();             /* lcd initialization */

    /* screen brushing test */
    lcd_clear(BLACK);
    delay(500);
    lcd_clear(RED);
    delay(500);
    lcd_clear(GREEN);
    delay(500);
    lcd_clear(BLUE);
    delay(500);
    lcd_clear(YELLOW);
    delay(500);
    lcd_clear(WHITE);
    delay(500);

    lcd_show_pic(0, 0, 240, 82, ALIENTEK_LOGO);                           /* LCD display ALIENTEK picture */
    lcd_show_string(10, 100, 200, 32, LCD_FONT_32, "ESP32-S3", RED);      /* LCD display 32 font ESP32S3 */
    lcd_show_string(10, 132, 200, 24, LCD_FONT_24, "TFTLCD TEST", RED);   /* LCD display 24 font TFTLCD TEST */
    lcd_show_string(10, 156, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED); /* LCD display 16 font ATOM@ALIENTEK */
    delay(500);
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    demo_show_cube();   /* show cube 3D rotation */
}

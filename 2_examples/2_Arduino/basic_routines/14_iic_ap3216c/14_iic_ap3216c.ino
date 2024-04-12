/**
 ****************************************************************************************************
 * @file        14_iic_ap3216c.ino
 * @author      ALIENTEK
 * @brief       AP3216C example
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
#include "ap3216c.h"


uint16_t ir, als, ps;       /* Ambient Light Intensity (ALS)/ Proximity Distance (PS)/ Infrared Intensity (IR) */

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

    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "AP3216C TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    while (ap3216c_init())  /* check ap3216c */
    {
        lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "AP3216C Check Failed!", RED);
        delay(500);
    }
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "AP3216C Ready!", RED);
        
    lcd_show_string(30, 160, 200, 16, LCD_FONT_16, " IR:", RED);
    lcd_show_string(30, 180, 200, 16, LCD_FONT_16, " PS:", RED);
    lcd_show_string(30, 200, 200, 16, LCD_FONT_16, "ALS:", RED);
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    ap3216c_read_data(&ir, &ps, &als);                      /* read data  */

    lcd_show_num(30 + 32, 160, ir, 5, LCD_FONT_16, BLUE);   /* lcd display IR */
    lcd_show_num(30 + 32, 180, ps, 5, LCD_FONT_16, BLUE);   /* lcd display PS */
    lcd_show_num(30 + 32, 200, als, 5, LCD_FONT_16, BLUE);  /* lcd display ALS */

    delay(500); 
}

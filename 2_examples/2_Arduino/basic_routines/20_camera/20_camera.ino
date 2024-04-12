/**
 ****************************************************************************************************
 * @file        20_camera.ino
 * @author      ALIENTEK
 * @brief       CAMERA example
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
#include "led.h"
#include "led.h"
#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "camera.h"


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

    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "CAMERA TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    while (camera_init())   /* camera initialization */
    {
        lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "Please check camera!", RED);
        delay(200);
        lcd_fill(30, 110, 200, 126, WHITE);
        delay(200);
    }        
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    camera_capture_show();  /* capture the camera image and display */
}

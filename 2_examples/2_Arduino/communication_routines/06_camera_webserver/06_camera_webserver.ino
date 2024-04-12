/**
 ****************************************************************************************************
 * @file        06_camera_webserver.ino
 * @author      ALIENTEK
 * @brief       CAMERA_WEBSERVER example
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
#include "esp_camera.h"
#include <WiFi.h>

char* ssid     = "wiking";      //"ALIENTEK-YF";   
char* password = "12345678";    //"15902020353";   

void startCameraServer();

/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    char ip_buf[20];

    led_init();             /* led initialization */
    uart_init(0, 115200);   /* uart0 initialization */
    xl9555_init();          /* xl9555 initialization */
    lcd_init();             /* lcd initialization */
    camera_init();          /* camera initialization */
    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "CAMERA WEB SERVER TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    WiFi.begin(ssid, password);
    WiFi.setSleep(false);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "waitting to connect wifi", RED);
    }
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "wifi connected          ", RED);

    /* Automatically starts streaming the camera feed to a web page that can be accessed 
     * through the board's IP address */
    startCameraServer();    

    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "Camera Ready!", RED);
    lcd_show_string(0, 150, 200, 16, LCD_FONT_16, "Use 'http://", RED);
    sprintf(ip_buf, "%s", WiFi.localIP().toString().c_str());           
    lcd_show_string(96, 150, 200, 16, LCD_FONT_16, ip_buf, BLUE);
    lcd_show_string(216, 150, 200, 16, LCD_FONT_16, "' to connect", RED);
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    LED_TOGGLE();
    delay(500);
}

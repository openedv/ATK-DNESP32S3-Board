/**
 ****************************************************************************************************
 * @file        03_wifi_client.ino
 * @author      ALIENTEK
 * @brief       WIFI_CLIENT example
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
#include <WiFiMulti.h>     

WiFiMulti WiFiMulti;

char* ssid     = "ALIENTEK-YF";   
char* password = "15902020353";  

/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    uint16_t i = 0;
    char ip_buf[20];

    led_init();             /* led initialization */
    uart_init(0, 115200);   /* uart0 initialization */
    xl9555_init();          /* xl9555 initialization */
    lcd_init();             /* lcd initialization */
    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "WIFI CLIENT TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    WiFiMulti.addAP(ssid, password);  /* start a network connection */
    
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "WIFI_NAME:", RED);  
    lcd_show_string(118, 110, 200, 16, LCD_FONT_16, ssid, BLUE);

    while(WiFiMulti.run() != WL_CONNECTED)  /* waitting for connecting network */
    {
        delay(500);
        lcd_show_char(30 + 8 * i, 130, '.', LCD_FONT_16, 1, BLUE);
        i++;
    }

    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "WiFi connected.", RED);
    lcd_show_string(30, 150, 200, 16, LCD_FONT_16, "IP address:", RED);
    
    sprintf(ip_buf, "%s", WiFi.localIP().toString().c_str());    
    lcd_show_string(126, 150, 200, 16, LCD_FONT_16, ip_buf, BLUE);

    delay(500);
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    const uint16_t port = 80;       
    char *host = "192.168.2.15";    
    WiFiClient client;

    lcd_fill(0, 170, 319, 239, WHITE);
    lcd_show_string(30, 170, 200, 16, LCD_FONT_16, "Connecting to      ", RED);
    lcd_show_string(142, 170, 200, 16, LCD_FONT_16, host, BLUE);

    if (!client.connect(host, port))  /* connect to a network server */
    {
        lcd_show_string(30, 170, 290, 16, LCD_FONT_16, "Connection failed.                  ", RED);
        lcd_show_string(30, 190, 290, 16, LCD_FONT_16, "Waiting 5 seconds before retrying...", RED);
        delay(3000);
        return;
    }

    // client.print("hello server!This is ESP32-S3\n\n");    /* send data to the server */
    client.print("GET /index.html HTTP/1.1\n\n");           /* send a request to the server */

    int maxloops = 0;
    
    while (!client.available() && maxloops < 30000)  /* waitting for a reply from the server */
    {
        maxloops++;
        delay(1); 
    }

    if (client.available() > 0)                     /* whether the server has data */
    {
        String line = client.readStringUntil('\r'); /* read back a line from the server */
        lcd_show_string(30, 190, 290, 16, LCD_FONT_16, "Read: ", RED);
        lcd_show_string(78, 190, 200, 16, LCD_FONT_16, (char *)line.c_str(), BLUE);
    }
    else
    {
        lcd_show_string(30, 190, 290, 16, LCD_FONT_16, "client.available() timed out ", RED);
    }

    lcd_show_string(30, 210, 290, 16, LCD_FONT_16, "Closing connection.                 ", RED);
    client.stop();    /* disconnect */

    lcd_show_string(30, 210, 290, 16, LCD_FONT_16, "Waiting 3 seconds before restarting.", RED);
    delay(3000);
}

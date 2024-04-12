/**
 ****************************************************************************************************
 * @file        02_wifi_webserver.ino
 * @author      ALIENTEK
 * @brief       WIFI_WEBSERVER example
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
#include "WiFi.h"


char* ssid     = "ALIENTEK-YF";   
char* password = "15902020353";   
WiFiServer server(80);            
WiFiClient client;                
void webpage_display(void);       

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
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "WIFI WEBSERVER TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    WiFi.begin(ssid, password);   /* linking network */

    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "WIFI_NAME:", RED); 
    lcd_show_string(118, 110, 200, 16, LCD_FONT_16, ssid, BLUE);

    while (WiFi.status() != WL_CONNECTED)   /* waitting for connecting network */
    {
        delay(500);
        lcd_show_char(30 + 8 * i, 130, '.', LCD_FONT_16, 1, BLUE);
        i++;
    }
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "WiFi connected.", RED);
    lcd_show_string(30, 150, 200, 16, LCD_FONT_16, "IP address:", RED);
    
    sprintf(ip_buf, "%s", WiFi.localIP().toString().c_str());       
    lcd_show_string(126, 150, 200, 16, LCD_FONT_16, ip_buf, BLUE);

    server.begin();   /* start the Internet of Things web server built by ESP32S3 */

    lcd_show_string(30, 170, 200, 16, LCD_FONT_16, "Please connect.....", RED);   /* wait for the client to connect to the IP address through the web */
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    client = server.available();    /* detects whether there are active client connections on the server side */

    if (client)                     /* client connect? */
    {                             
        String currentLine = "";              /* create a string to hold incoming data from the client */
        while (client.connected())            /* check whether the device is successfully connected to the server */
        { 
            lcd_show_string(30, 170, 200, 16, LCD_FONT_16, "Client is connected", RED);

            if (client.available())           /* check whether the network client receives messages from the server */
            {             
                char c = client.read();       /* read the message from the server */
                //Serial.write(c);   
                if (c == '\n')                /* used to determine whether the received web page access data is complete */
                {   
                    /* If the current action is empty, there are two newlines in a line. 
                     * This is the end of the client's HTTP request, so send a response: */                
                    if (currentLine.length() == 0) 
                    {
                        webpage_display();
                        break;
                    } 
                    else                /* if there is a newline, then clear currentLine */ 
                    {    
                        currentLine = ""; 
                    }
                } 
                else if (c != '\r')     /* if there is no carriage return */
                {  
                    currentLine += c;   /* add it to the end of currentLine */  
                }

                /* check that the client request is "GET /H" or "GET /L" */
                if (currentLine.endsWith("GET /LED_ON")) 
                {
                    LED(0);   /* GET /H turn on light */
                    lcd_show_string(30, 210, 200, 16, LCD_FONT_16, "LED ON ", RED);
                }

                if (currentLine.endsWith("GET /LED_OFF")) 
                {
                    LED(1);   /* GET /L turn on light */
                    lcd_show_string(30, 210, 200, 16, LCD_FONT_16, "LED OFF", RED);
                }
          }
      }
      client.stop();  /* disconnect */
    }
}

/**
 * @brief    web data content
 * @param    None
 * @retval   None
 */
void webpage_display(void)
{
    /* HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK) and 
     * a content type so that the client knows what comes next, followed by a blank line: */
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");   /* browser response data type is text data */
    client.println();

    client.print("<!DOCTYPE html>");
    client.print("<html>");
    client.print("<head>");
    client.print("<title>ATK ESP32-S3 Web Server</title>");
    client.print("</head>");

    client.print("<body>");
    client.print("<h1> ESP32-S3 Web Server </h1>");
    client.print("<p> LED State </p>");    
    client.print("<p> <a href=\"/LED_ON\"> <button> ON </button> </p>");   
    client.print("<p> <a href=\"/LED_OFF\"> <button> OFF </button> </p>");   
    client.print("</body>");
    client.print("</html>");

    /* the HTTP response ends with another blank line */
    client.println();
}
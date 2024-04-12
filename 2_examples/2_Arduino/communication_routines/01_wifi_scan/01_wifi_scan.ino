/**
 ****************************************************************************************************
 * @file        01_wifi_scan.ino
 * @author      ALIENTEK
 * @brief       WIFI_SCAN example
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


char wifi_name[20];  
char wifi_rssi[20];  

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
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "WIFI SCAN TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    WiFi.mode(WIFI_STA);    /* wifi mode is STA mode */
    WiFi.disconnect();      /* disconnect any previous WIFI connections */
    delay(100);
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "Scan start", BLUE);
    int network_cnt = WiFi.scanNetworks();  /* start scanning for available networks */
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "Scan done ", BLUE);

    if (network_cnt == 0) 
    {
        lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "no wifi networks found ", RED);
    }
    else 
    {
        lcd_show_num(30, 130, network_cnt, 2, LCD_FONT_16, RED);
        lcd_show_string(62, 130, 200, 16, LCD_FONT_16, "networks found", RED);
        lcd_show_string(30, 150, 300, 16, LCD_FONT_16, "Nr|SSID          |RSSI|CH|ENC_TYPE", BLUE);
        
        int wifi_info_show = (network_cnt > 4 ? 4 : network_cnt);           
        /* due to the limited display area of the screen, only the first 4 wifi is displayed, 
         * when the number of wifi is less than 4, the current number is displayed */
        for (int i = 0; i < wifi_info_show; ++i)   
        {
            lcd_show_num(30, 170 + i * 16, i + 1, 2, LCD_FONT_16, RED);    

            sprintf(wifi_name, "%-15s", WiFi.SSID(i).c_str());              
            lcd_show_string(54, 170 + i * 16, 112, 16, LCD_FONT_16, wifi_name, RED);  
                
            sprintf(wifi_rssi, "%4d", WiFi.RSSI(i));                        
            lcd_show_string(174, 170 + i * 16, 300, 16, LCD_FONT_16, wifi_rssi, RED); 

            //Serial.printf("%2d", WiFi.channel(i));                          
            lcd_show_num(214, 170 + i * 16,  WiFi.channel(i), 2, LCD_FONT_16, RED);    

            switch (WiFi.encryptionType(i))                                
            {
                case WIFI_AUTH_OPEN:            
                    lcd_show_string(238, 170 + i * 16, 80, 16, LCD_FONT_16, "open     ", RED);
                    break;
                case WIFI_AUTH_WEP:             
                    lcd_show_string(238, 170 + i * 16, 80, 16, LCD_FONT_16, "WEP      ", RED);
                    break;
                case WIFI_AUTH_WPA_PSK:         
                    lcd_show_string(238, 170 + i * 16, 80, 16, LCD_FONT_16, "WPA      ", RED);
                    break;
                case WIFI_AUTH_WPA2_PSK:
                    lcd_show_string(238, 170 + i * 16, 80, 16, LCD_FONT_16, "WPA2     ", RED);
                    break;
                case WIFI_AUTH_WPA_WPA2_PSK:
                    lcd_show_string(238, 170 + i * 16, 80, 16, LCD_FONT_16, "WPA+WPA2 ", RED);
                    break;
                case WIFI_AUTH_WPA2_ENTERPRISE:
                    lcd_show_string(238, 170 + i * 16, 80, 16, LCD_FONT_16, "WPA2-EAP ", RED);
                    break;
                case WIFI_AUTH_WPA3_PSK:
                    lcd_show_string(238, 170 + i * 16, 80, 16, LCD_FONT_16, "WPA3     ", RED);
                    break;
                case WIFI_AUTH_WPA2_WPA3_PSK:
                    lcd_show_string(238, 170 + i * 16, 80, 16, LCD_FONT_16, "WPA2+WPA3", RED);
                    break;
                case WIFI_AUTH_WAPI_PSK:
                    lcd_show_string(238, 170 + i * 16, 80, 16, LCD_FONT_16, "WAPI     ", RED);
                    break;
                default:
                    lcd_show_string(238, 170 + i * 16, 80, 16, LCD_FONT_16, "unknown  ", RED);
            }
            delay(10);
        }
    }
    WiFi.scanDelete();
    delay(5000);
    lcd_fill(0, 110, 319, 239, WHITE);
}

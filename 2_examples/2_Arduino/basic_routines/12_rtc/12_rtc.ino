/**
 ****************************************************************************************************
 * @file        12_rtc.ino
 * @author      ALIENTEK
 * @brief       RTC example
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
#include <ESP32Time.h>    /* The ESP32Time library needs to be installed */


ESP32Time rtc;
uint8_t tbuf[100];        /* Store RTC information */

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
    rtc.setTime(00, 51, 17, 1, 12, 2023);  /* December 1, 2023, 17:52:00 */
    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "RTC TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    struct tm timeinfo = rtc.getTimeStruct();
    sprintf((char *)tbuf, "Time:%02d:%02d:%02d", timeinfo.tm_hour - 1, timeinfo.tm_min, timeinfo.tm_sec);     
    lcd_show_string(30, 130, 210, 16, LCD_FONT_16, (char *)tbuf, RED);
    sprintf((char *)tbuf, "Date:%04d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
    lcd_show_string(30, 150, 210, 16, LCD_FONT_16, (char *)tbuf, RED);
    sprintf((char *)tbuf, "Week:%d", timeinfo.tm_wday);
    lcd_show_string(30, 170, 210, 16, LCD_FONT_16, (char *)tbuf, RED);

    delay(1000);

    /* ESP32Time other function interfaces, you can use the Serial.println function to print
    getTime()           //  (String) 15:24:38
    getDate()           //  (String) Sun, Jan 17 2021
    getDate(true)       //  (String) Sunday, January 17 2021
    getDateTime()       //  (String) Sun, Jan 17 2021 15:24:38
    getDateTime(true)   //  (String) Sunday, January 17 2021 15:24:38
    getTimeDate()       //  (String) 15:24:38 Sun, Jan 17 2021
    getTimeDate(true)   //  (String) 15:24:38 Sunday, January 17 2021

    getMicros()         //  (unsigned long) 723546
    getMillis()         //  (unsigned long) 723
    getEpoch()          //  (unsigned long) 1609459200
    getLocalEpoch()     //  (unsigned long) 1609459200 // local epoch without offset
    getSecond()         //  (int)     38    (0-59)
    getMinute()         //  (int)     24    (0-59)
    getHour()           //  (int)     3     (0-12)
    getHour(true)       //  (int)     15    (0-23)
    getAmPm()           //  (String)  pm
    getAmPm(false)      //  (String)  PM
    getDay()            //  (int)     17    (1-31)
    getDayofWeek()      //  (int)     0     (0-6)
    getDayofYear()      //  (int)     16    (0-365)
    getMonth()          //  (int)     0     (0-11)
    getYear()           //  (int)     2021
    */
}

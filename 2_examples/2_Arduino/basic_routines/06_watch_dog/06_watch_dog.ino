/**
 ****************************************************************************************************
 * @file        06_watch_dog.ino
 * @author      ALIENTEK
 * @brief       WATCH_DOG example
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
#include "key.h"
#include "watchdog.h"


#define wdg_timeout 1200    /* time in 1200ms to trigger the watchdog */

/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    key_init();                           /* key initialization */
    uart_init(0, 115200);                 /* uart0 initialization */
    Serial.println("running setup");      

    wdg_init(wdg_timeout * 1000, 80);     /* timer 0, div 80, Timing time 1.2s */
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    Serial.println("running main loop");  

    timerWrite(wdg_timer, 0);             /* reset timer (feed watchdog) */

    long looptime = millis();             /* millis function gets the number of milliseconds that have elapsed since the board started running the current program */

    while (!KEY)                          /* while button is pressed, delay up to 1.2 seconds to trigger the timer */
    {
        Serial.println("key pressed, delay_500ms");
        delay(500);
    }

    delay(1000);                          /* simulate work */         
    looptime = millis() - looptime;       /* get process time */

    Serial.print("loop time is = ");      /* should be under 1200 */
    Serial.println(looptime); 
}

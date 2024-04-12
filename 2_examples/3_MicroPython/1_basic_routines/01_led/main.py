"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       LED example
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
"""

from machine import Pin
import time


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if  __name__ == '__main__':
    
    # set pin high on creation
    led = Pin(1,Pin.OUT,value = 1)
    
    while True:
        
        led.value(0)                    # set pin to off/low
        time.sleep_ms(500)              # sleep for 500 milliseconds
        led.value(1)                    # set pin to on/high
        time.sleep_ms(500)              # sleep for 500 milliseconds


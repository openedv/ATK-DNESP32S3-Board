"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       KEY example
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
if __name__ == '__main__':
    
    # set pin high on creation
    led = Pin(1,Pin.OUT,value = 1)
    # enable internal pull-up resistor
    key = Pin(0,Pin.IN,Pin.PULL_UP)
    
    while True:
        
        if key.value() == 0:                # check whether the key is pressed
            
            time.sleep_ms(10)               # sleep for 10 milliseconds
            
            if key.value() == 0:            # check whether the key is pressed
                led_state = led.value()
                led.value(not led_state)
                
                while not key.value():      # detects whether the key is released
                    pass

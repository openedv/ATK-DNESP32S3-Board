"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       EXIT example
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
 * @brief       Interrupt service function
 * @param       key:handle
 * @retval      None
"""
def KEY_INT_IRQHandler(key):
    
    time.sleep_ms(10)           # sleep for 10 milliseconds
    
    if key.value() == 0:
        global led_state
        led_state = led.value()
        led.value(not led_state)

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
    # define interrupt, falling edge trigger
    key.irq(KEY_INT_IRQHandler,Pin.IRQ_FALLING)
    # main loop
    while True:
        pass
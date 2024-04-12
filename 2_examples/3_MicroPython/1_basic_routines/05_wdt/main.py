"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       WDT example
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

from machine import Pin,I2C,WDT
import atk_xl9555 as io_ex
import time


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    
    # set pin high on creation
    led = Pin(1,Pin.OUT,value = 1)
    time.sleep_ms(100)
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    xl9555 = io_ex.init(i2c0)
    # enable the WDT with a timeout of 3s
    wdt = WDT(timeout=3000)
    led.value(0)
    
    while True:
        
        # get key value
        key = int(xl9555.key_scan())
        
        if key == io_ex.KEY0:
            # free dogs
            wdt.feed()

        time.sleep_ms(10)                       # sleep for 10 milliseconds


"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       IIC_EXIO example
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

from machine import Pin,I2C
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
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    xl9555 = io_ex.init(i2c0)
    
    while True:
        
        # get key value
        key = int(xl9555.key_scan())
        
        if key == io_ex.KEY0:
            xl9555.write_bit(io_ex.BEEP,0)      # turn on the buzzer
        elif key == io_ex.KEY1:
            xl9555.write_bit(io_ex.BEEP,1)      # turn off the buzzer
        elif key == io_ex.KEY2:
            led.value(0)                        # set pin to off/low
        elif key == io_ex.KEY3:
            led.value(1)                        # set pin to on/high

        time.sleep_ms(10)                       # sleep for 10 milliseconds

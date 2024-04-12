"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       SPILCD example
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

from machine import Pin,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
import time


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    
    x = 0
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    xl9555 = io_ex.init(i2c0)
    
    # reset lcd
    xl9555.write_bit(io_ex.SLCD_RST,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.SLCD_RST,1)
    time.sleep_ms(100)
    
    spi = SPI(2,baudrate = 80000000, sck = Pin(12), mosi = Pin(11), miso = Pin(13))
    display = lcd.init(spi,dc = Pin(40,Pin.OUT,Pin.PULL_UP,value = 1),cs = Pin(21,Pin.OUT,Pin.PULL_UP,value = 1),dir = 1,lcd = 0)
    # turn on the backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    
    while True:
        
        # create dictionary
        seasondict = {
        0: lcd.BLACK,
        1: lcd.BLUE,
        2: lcd.RED,
        3: lcd.GREEN,
        4: lcd.CYAN,
        5: lcd.MAGENTA,
        6: lcd.YELLOW}

        # refresh color
        display.clear(seasondict[x])
        display.string(0, 5, 240, 32, 32, "ESP32-S3",lcd.RED)
        display.string(0, 34, 240, 16, 16, "SPI LCD Test",lcd.RED)
        display.string(0, 50, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
        x += 1
        
        if x == 7:
            x = 0
        time.sleep(1)

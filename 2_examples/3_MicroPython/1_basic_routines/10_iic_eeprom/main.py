"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       EEPROM example
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
import atk_at24cx as at24c02
import time


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    
    var1 = 'Hello ALIENTEK ESP32-S3'
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
    # display experiment information
    display.string(0, 5, 240, 32, 32, "ESP32-S3",lcd.RED)
    display.string(30, 70, 200, 16, 16, "EEPORM TEST", lcd.RED)
    display.string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", lcd.RED)
    display.string(30, 110, 200, 16, 16, "Data:", lcd.RED)
    at24cx = at24c02.init(i2c0)
    time.sleep_ms(500)              # sleep for 500 milliseconds
    
    # check whether the device exists
    while at24cx.at24cxx_check():
        print("Failed to query the device...")
        time.sleep_ms(500)          # sleep for 500 milliseconds
    
    while True:
        
        # cet key value
        key = int(xl9555.key_scan())
        
        if key == io_ex.KEY0:
            # write data
            at24cx.at24cxx_write(0,var1,len(var1))
        elif key == io_ex.KEY1:
            # read data
            data = str(at24cx.at24cxx_read(0,len(var1)))
            display.string(110, 110, 200, 16, 16, str(data), lcd.BLUE)

        time.sleep_ms(10)          # sleep for 10 milliseconds

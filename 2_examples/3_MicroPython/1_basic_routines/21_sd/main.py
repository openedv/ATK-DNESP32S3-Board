"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       SD example
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
from sdcard import SDCard
import atk_xl9555 as io_ex
import atk_lcd as lcd
import time
import uos

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
    
    spi = SPI(2,baudrate = 24000000, sck = Pin(12), mosi = Pin(11), miso = Pin(13))
    display = lcd.init(spi,dc = Pin(40,Pin.OUT,Pin.PULL_UP,value = 1),cs = Pin(21,Pin.OUT,Pin.PULL_UP,value = 1),dir = 1,lcd = 0)
    # turn on the backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    
    sd = SDCard(spi,Pin(2,Pin.OUT))
    # display experiment information
    display.string(30, 50, 240, 32, 32, "ESP32-S3",lcd.RED)
    display.string(30, 80, 240, 24, 24, "SD TEST",lcd.RED)
    display.string(30, 110, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(30, 130, 200, 16, 16, "File Read:", lcd.BLUE)

    # mount
    uos.mount(sd,'/sd')
    
    # list directory contents
    print('system directory:{}'.format(uos.listdir()))
    with open("/sd/test.txt", "w") as f:
            f.write(str("Hello ALIENTEK"))

    # read the content of the hello.txt file from the SD card directory
    with open("/sd/test.txt", "r") as f:

        data = f.read()
    
    display.string(130, 130, 200, 16, 16, str(data), lcd.BLUE)
    # eject
    uos.umount('/sd')
    
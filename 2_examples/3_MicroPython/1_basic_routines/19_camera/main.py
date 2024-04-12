"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       CAMERA example
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

import time
import camera
import atk_xl9555 as io_ex
import atk_lcd as lcd
from machine import Pin,SPI,I2C


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    xl9555 = io_ex.init(i2c0)
    xl9555.write_bit(io_ex.BEEP,1)
    
    # reset camera
    xl9555.write_bit(io_ex.OV_RESET,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_RESET,1)
    time.sleep_ms(100)
    # open camera
    xl9555.write_bit(io_ex.OV_PWDN,1)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_PWDN,0)
    time.sleep_ms(100)
    # sleep for 1000 milliseconds
    time.sleep_ms(1000)
    
    for i in range(5):
        cam = camera.init(0, format=camera.RGB565, fb_location=camera.PSRAM,framesize = camera.FRAME_240X240,xclk_freq = 24000000)
        print("Camera ready?: ", cam)
        if cam:
            print("Camera ready")
            break
        else:
            camera.deinit()
            camera.init(0, format=camera.RGB565, fb_location=camera.PSRAM,framesize = camera.FRAME_240X240,xclk_freq = 24000000)
            time.sleep(2)
    else:
        print('Timeout')
        reset()
    
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
    display.camera(42,5)


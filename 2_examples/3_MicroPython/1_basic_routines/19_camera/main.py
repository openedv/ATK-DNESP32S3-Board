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
    
    # Initialize I2C communication with specified pins and frequency
    i2c0 = I2C(0, scl=Pin(42), sda=Pin(41), freq=400000)
    # Initialize XL9555 IO expansion module
    xl9555 = io_ex.init(i2c0)
    # Turn on the beep
    xl9555.write_bit(io_ex.BEEP, 1)
    
    # Reset camera
    xl9555.write_bit(io_ex.OV_RESET, 0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_RESET, 1)
    time.sleep_ms(100)
    # Open camera
    xl9555.write_bit(io_ex.OV_PWDN, 1)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_PWDN, 0)
    time.sleep_ms(1000)
    
    # Retry camera initialization up to 5 times
    for i in range(5):
        cam = camera.init(0, format=camera.RGB565, fb_location=camera.PSRAM, framesize=camera.FRAME_240X240, xclk_freq=24000000)
        print("Camera ready?: ", cam)
        if cam:
            print("Camera ready")
            break
        else:
            camera.deinit()
            camera.init(0, format=camera.RGB565, fb_location=camera.PSRAM, framesize=camera.FRAME_240X240, xclk_freq=24000000)
            time.sleep(2)
    else:
        print('Timeout')
        reset()
    
    # Reset LCD
    xl9555.write_bit(io_ex.SLCD_RST, 0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.SLCD_RST, 1)
    time.sleep_ms(100)
    
    # Initialize SPI communication with specified parameters and pins
    spi = SPI(2, baudrate=80000000, sck=Pin(12), mosi=Pin(11), miso=Pin(13))
    # Initialize LCD display
    display = lcd.init(spi, dc=Pin(40, Pin.OUT, Pin.PULL_UP, value=1), cs=Pin(21, Pin.OUT, Pin.PULL_UP, value=1), dir=1, lcd=0)
    # Turn on the backlight
    xl9555.write_bit(io_ex.SLCD_PWR, 1)
    time.sleep_ms(100)
    # Display camera feed on LCD
    display.camera(42, 5)

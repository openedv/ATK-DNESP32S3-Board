"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       face_recongnition example
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
import esp_who
import atk_xl9555 as io_ex
import atk_lcd as lcd
from machine import Pin,SPI,I2C


if __name__ == '__main__':
    # Initialize I2C communication with specified pins and frequency
    i2c0 = I2C(0, scl=Pin(42), sda=Pin(41), freq=400000)
    xl9555 = io_ex.init(i2c0)
    
    # Reset CAMERA
    xl9555.write_bit(io_ex.OV_RESET, 0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_RESET, 1)
    time.sleep_ms(100)
    
    # Power on CAMERA
    xl9555.write_bit(io_ex.OV_PWDN, 1)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_PWDN, 0)
    time.sleep_ms(100)

    time.sleep_ms(1000)

    # Attempt to initialize camera module multiple times
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

    # Initialize SPI communication and LCD display
    spi = SPI(2, baudrate=80000000, sck=Pin(12), mosi=Pin(11), miso=Pin(13))
    display = lcd.init(spi, dc=Pin(40, Pin.OUT, Pin.PULL_UP, value=1), cs=Pin(21, Pin.OUT, Pin.PULL_UP, value=1), dir=1, lcd=0)
    xl9555.write_bit(io_ex.SLCD_PWR, 1)
    time.sleep_ms(100)
    display.clear(lcd.BLACK)

    # Flip up and down (OV5640 must be flipped up and down, while OV2640 does not need to be flipped up and down)
    camera.flip(1)
    camera.speffect(camera.EFFECT_NONE)
    camera.saturation(0)
    camera.brightness(0)
    camera.contrast(0)
    camera.quality(10)
    
    # Enable facial recognition
    esp_who.ai_detection_strat(esp_who.FACE_RECONGNITION)
    display.ai_identify_camerashow(42, 0)
    # display.ai_identify_camerashow(0, 42)

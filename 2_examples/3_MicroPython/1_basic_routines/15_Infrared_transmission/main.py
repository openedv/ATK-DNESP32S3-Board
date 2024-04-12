"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       EMISSION example
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
from emission import NEC
import remote
import time


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == "__main__":
    
    t = 0
    # set pin high on creation
    led = Pin(1,Pin.OUT, value = 1)
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
    display.string(30, 50, 240, 16, 16, "ESP32-S3",lcd.RED)
    display.string(30, 70, 240, 16, 16, "REMOTE RX/TX TEST",lcd.RED)
    display.string(30, 90, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(30, 120, 200, 16, 16, "RX CMD:", lcd.RED)
    display.string(30, 140, 200, 16, 16, "TX CMD:", lcd.RED)
    display.string(30, 160, 200, 16, 16, "DECODE:", lcd.RED)

    ir = remote.REMOTE_IR(2)
    irb = NEC(Pin(8, Pin.OUT, value = 0), 38000)
    
    while True :

        t += 1
        
        if t == 0:
            t = 1
        
        irb.transmit(0,t)
        cmd,s = ir.remote_scan()
        time.sleep_ms(200)         # sleep for 100 milliseconds
        display.fill(30 + 11 * 8 - 2,120,30 + 11 * 8 + 100,200,lcd.WHITE)
        display.string(30 + 11 * 8, 120, 200, 16, 16, str(cmd), lcd.BLUE)
        display.string(30 + 11 * 8, 140, 200, 16, 16, str(t - 1), lcd.BLUE)
        display.string(30 + 11 * 8, 160, 200, 16, 16, str(s), lcd.BLUE)
        led_state = led.value()
        led.value(not led_state)
        time.sleep_ms(1000)         # sleep for 1 second

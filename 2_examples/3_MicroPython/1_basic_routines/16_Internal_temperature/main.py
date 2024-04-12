"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       TEMPERATURE example
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
import atk_sensor as sensor
import time


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    xl9555 = io_ex.init(i2c0)
    
    # reset LCD
    xl9555.write_bit(io_ex.SLCD_RST,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.SLCD_RST,1)
    time.sleep_ms(100)
    
    spi = SPI(2,baudrate = 80000000, sck = Pin(12), mosi = Pin(11), miso = Pin(13))
    display = lcd.init(spi,dc = Pin(40,Pin.OUT,Pin.PULL_UP,value = 1),cs = Pin(21,Pin.OUT,Pin.PULL_UP,value = 1),dir = 1,lcd = 0)
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    # display experiment information
    display.string(30, 50, 240, 16, 16, "ESP32-S3",lcd.RED)
    display.string(30, 70, 240, 16, 16, "Temperature TEST",lcd.RED)
    display.string(30, 90, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(30, 120, 200, 16, 16, "TEMPERATE: 00.00C", lcd.RED)
    sensor.init()

    while True:
        
        umber = float(sensor.sensor_read())
        display.num(30 + 11 * 8,120,int(umber),2,16,lcd.BLUE)
        display.num(30 + 14 * 8,120,int(umber * 100 % 100),2,16,lcd.BLUE)
        time.sleep_ms(500)

"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       RTC example
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

from machine import Pin,RTC,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
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
    display.string(30, 550, 240, 16, 16, "ESP32-S3",lcd.RED)
    display.string(30, 70, 200, 16, 16, "RTC TEST", lcd.RED)
    display.string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", lcd.RED)
    display.string(30, 110, 200, 16, 16, "Specific:", lcd.RED)
    display.string(30, 130, 200, 16, 16, "Time:", lcd.RED)
    display.string(30, 150, 200, 16, 16, "Date:", lcd.RED)

    rtc = RTC()
    # set a specific date and time
    if rtc.datetime()[0] != 2023:
        rtc.datetime((2023, 8, 15, 2, 0, 0, 0, 0))
    
    while True:

        data_time = rtc.datetime()
        display.string(110, 110, 200, 16, 16, str(data_time[0:3]), lcd.RED)
        display.string(110, 130, 200, 16, 16, str(data_time[4:7]), lcd.RED)
        display.string(110, 150, 200, 16, 16, str(int(data_time[3]) + 1), lcd.RED)

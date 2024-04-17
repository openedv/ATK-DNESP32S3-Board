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
    
    # Initialize I2C communication on bus 0 with specified pins and frequency
    i2c0 = I2C(0, scl=Pin(42), sda=Pin(41), freq=400000)
    # Initialize XL9555 IO expansion module
    xl9555 = io_ex.init(i2c0)
    
    # Reset LCD
    xl9555.write_bit(io_ex.SLCD_RST, 0)     # Set SLCD_RST bit to 0
    time.sleep_ms(100)                      # Sleep for 100 milliseconds
    xl9555.write_bit(io_ex.SLCD_RST, 1)     # Set SLCD_RST bit to 1
    time.sleep_ms(100)                      # Sleep for 100 milliseconds
    
    # Initialize SPI communication with specified parameters and pins
    spi = SPI(2, baudrate=80000000, sck=Pin(12), mosi=Pin(11), miso=Pin(13))
    # Initialize LCD display
    display = lcd.init(spi, dc=Pin(40, Pin.OUT, Pin.PULL_UP, value=1), cs=Pin(21, Pin.OUT, Pin.PULL_UP, value=1), dir=1, lcd=0)
    xl9555.write_bit(io_ex.SLCD_PWR, 1)     # Set SLCD_PWR bit to 1
    time.sleep_ms(100)                      # Sleep for 100 milliseconds
    
    # Display experiment information on LCD
    display.string(30, 50, 240, 16, 16, "ESP32-S3", lcd.RED)            # Display "ESP32-S3" in red color
    display.string(30, 70, 240, 16, 16, "Temperature TEST", lcd.RED)    # Display "Temperature TEST" in red color
    display.string(30, 90, 240, 16, 16, "ATOM@ALIENTEK", lcd.RED)       # Display "ATOM@ALIENTEK" in red color
    display.string(30, 120, 200, 16, 16, "TEMPERATE: 00.00C", lcd.RED)  # Display initial temperature in red color
    
    # Initialize sensor
    sensor.init()

    while True:
        
        # Read temperature from sensor
        temperature = float(sensor.sensor_read())
        # Display temperature on LCD
        display.num(30 + 11 * 8, 120, int(temperature), 2, 16, lcd.BLUE)                # Display integer part of temperature in blue color
        display.num(30 + 14 * 8, 120, int(temperature * 100 % 100), 2, 16, lcd.BLUE)    # Display fractional part of temperature in blue color
        time.sleep_ms(500)              # Sleep for 500 milliseconds


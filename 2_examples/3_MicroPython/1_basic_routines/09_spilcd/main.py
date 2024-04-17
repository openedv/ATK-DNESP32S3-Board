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
    
    x = 0  # Initialize variable x to 0
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
    # Turn on the backlight
    xl9555.write_bit(io_ex.SLCD_PWR, 1)     # Set SLCD_PWR bit to 1
    time.sleep_ms(100)                      # Sleep for 100 milliseconds
    
    while True:                             # Start an infinite loop
        
        # Create a dictionary for different LCD colors
        seasondict = {
            0: lcd.BLACK,
            1: lcd.BLUE,
            2: lcd.RED,
            3: lcd.GREEN,
            4: lcd.CYAN,
            5: lcd.MAGENTA,
            6: lcd.YELLOW}

        # Refresh the LCD display with the current color
        display.clear(seasondict[x])                                    # Clear the display with the color indexed by x
        display.string(0, 5, 240, 32, 32, "ESP32-S3", lcd.RED)          # Display "ESP32-S3" in red color
        display.string(0, 34, 240, 16, 16, "SPI LCD Test", lcd.RED)     # Display "SPI LCD Test" in red color
        display.string(0, 50, 240, 16, 16, "ATOM@ALIENTEK", lcd.RED)    # Display "ATOM@ALIENTEK" in red color
        x += 1          # Increment x by 1
        
        if x == 7:      # If x reaches 7, reset it to 0
            x = 0
        time.sleep(1)   # Sleep execution for 1 second
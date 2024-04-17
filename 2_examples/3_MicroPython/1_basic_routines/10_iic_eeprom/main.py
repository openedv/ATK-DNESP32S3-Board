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
    
    var1 = 'Hello ALIENTEK ESP32-S3'  # Assign a string to variable var1
    
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
    xl9555.write_bit(io_ex.SLCD_PWR, 1)  # Set SLCD_PWR bit to 1
    time.sleep_ms(100)  # Sleep for 100 milliseconds
    
    # Display experiment information on LCD
    display.string(0, 5, 240, 32, 32, "ESP32-S3", lcd.RED)          # Display "ESP32-S3" in red color
    display.string(30, 70, 200, 16, 16, "EEPROM TEST", lcd.RED)     # Display "EEPROM TEST" in red color
    display.string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", lcd.RED)   # Display "ATOM@ALIENTEK" in red color
    display.string(30, 110, 200, 16, 16, "Data:", lcd.RED)          # Display "Data:" in red color
    
    # Initialize AT24C02 EEPROM module
    at24cx = at24c02.init(i2c0)
    time.sleep_ms(500)  # Sleep for 500 milliseconds
    
    # Check whether the EEPROM device exists
    while at24cx.at24cxx_check():
        print("Failed to query the device...")  # Print error message
        time.sleep_ms(500)                      # Sleep for 500 milliseconds
    
    while True:                                 # Start an infinite loop
        
        # Get key value from XL9555 keypad
        key = int(xl9555.key_scan())
        
        if key == io_ex.KEY0:                   # If KEY0 is pressed
            # Write data to EEPROM
            at24cx.at24cxx_write(0, var1, len(var1))
        elif key == io_ex.KEY1:                 # If KEY1 is pressed
            # Read data from EEPROM
            data = str(at24cx.at24cxx_read(0, len(var1)))
            # Display read data on LCD in blue color
            display.string(110, 110, 200, 16, 16, str(data), lcd.BLUE)

        time.sleep_ms(10)                       # Sleep execution for 10 milliseconds


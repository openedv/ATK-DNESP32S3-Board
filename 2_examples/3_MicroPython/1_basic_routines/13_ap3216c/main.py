"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       AP3216C example
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
import atk_ap3216 as ap3216c
import time


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
   
    # Set pin high on creation
    led = Pin(1, Pin.OUT, value=1)
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
    display.string(30, 50, 240, 16, 16, "ESP32-S3", lcd.RED)        # Display "ESP32-S3" in red color
    display.string(30, 70, 200, 16, 16, "AP3216C TEST", lcd.RED)    # Display "AP3216C TEST" in red color
    display.string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", lcd.RED)   # Display "ATOM@ALIENTEK" in red color
    display.string(30, 110, 200, 16, 16, "ir:", lcd.RED)            # Display "ir:" in red color
    display.string(30, 130, 200, 16, 16, "ps:", lcd.RED)            # Display "ps:" in red color
    display.string(30, 150, 200, 16, 16, "als:", lcd.RED)           # Display "als:" in red color
    
    ap3216 = ap3216c.init(i2c0)     # Initialize AP3216C sensor
    
    while True:                     # Start an infinite loop
        # Read sensor values
        data = tuple(ap3216.ap3216c_read())
        # Data conversion
        ir = (data[1] << 8) | data[0]
        ps = (data[3] << 8) | data[2]
        als = (data[5] << 8) | data[4]
        # Display data on LCD
        display.num(110, 110, int(ir), 5, 16, lcd.BLUE)     # Display IR value
        display.num(110, 130, int(ps), 5, 16, lcd.BLUE)     # Display PS value
        display.num(110, 150, int(als), 5, 16, lcd.BLUE)    # Display ALS value
        # Toggle LED state
        led_state = led.value()
        led.value(not led_state)
        time.sleep_ms(500)  # Sleep for 500 milliseconds


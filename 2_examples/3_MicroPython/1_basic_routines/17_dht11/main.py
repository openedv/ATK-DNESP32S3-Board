"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       DHT11 example
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
import dht


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':

    # Set pin 1 as an output pin and initialize it with a high value (1)
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
    xl9555.write_bit(io_ex.SLCD_PWR, 1)     # Set SLCD_PWR bit to 1
    time.sleep_ms(100)                      # Sleep for 100 milliseconds

    # Display experiment information on LCD
    display.string(5, 10, 240, 32, 32, "ESP32-S3", lcd.RED)             # Display "ESP32-S3" in red color
    display.string(5, 43, 240, 24, 24, "DHT11 TEST", lcd.RED)           # Display "DHT11 TEST" in red color
    display.string(5, 68, 240, 16, 16, "ATOM@ALIENTEK", lcd.RED)        # Display "ATOM@ALIENTEK" in red color
    display.string(5, 84, 200, 16, 16, "temperature: 00.00C", lcd.RED)  # Display initial temperature in red color
    display.string(5, 100, 200, 16, 16, "humidity: 00.00H", lcd.RED)    # Display initial humidity in red color

    # Initialize DHT11 sensor with specified pin
    dht11 = dht.DHT11(Pin(0, Pin.IN, Pin.PULL_UP))
    
    while True:
        
        # Measure temperature and humidity
        dht11.measure()
        # Read temperature data
        temperature = float(dht11.temperature())
        # Read humidity data
        humidity = float(dht11.humidity())

        display.num(5 + 13 * 8, 84, int(temperature), 2, 16, lcd.BLUE)              # Display integer part of temperature in blue color
        display.num(5 + 16 * 8, 84, int(temperature * 100 % 100), 2, 16, lcd.BLUE)  # Display fractional part of temperature in blue color
        display.num(5 + 10 * 8, 100, int(humidity), 2, 16, lcd.BLUE)                # Display integer part of humidity in blue color
        display.num(5 + 13 * 8, 100, int(humidity * 100 % 100), 2, 16, lcd.BLUE)    # Display fractional part of humidity in blue color
        led_state = led.value()
        led.value(not led_state)
        time.sleep(1)  # Sleep for 1 second

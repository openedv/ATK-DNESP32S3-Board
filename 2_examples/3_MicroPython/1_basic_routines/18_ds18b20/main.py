"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       DS18B20 example
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
import onewire, ds18x20
import atk_xl9555 as io_ex
import atk_lcd as lcd
import time


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
    # Turn on the backlight
    xl9555.write_bit(io_ex.SLCD_PWR, 1)  # Set SLCD_PWR bit to 1
    time.sleep_ms(100)  # Sleep for 100 milliseconds

    # Display experiment information on LCD
    display.string(30, 50, 240, 32, 32, "ESP32-S3", lcd.RED)                # Display "ESP32-S3" in red color
    display.string(30, 80, 240, 24, 24, "DS18B20 TEST", lcd.RED)            # Display "DS18B20 TEST" in red color
    display.string(30, 110, 240, 16, 16, "ATOM@ALIENTEK", lcd.RED)          # Display "ATOM@ALIENTEK" in red color
    display.string(30, 130, 200, 16, 16, "TEMPERATE: 00.00C", lcd.BLUE)     # Display initial temperature in blue color

    # Initialize DS18X20 sensor with specified pin
    ds_18b20 = ds18x20.DS18X20(onewire.OneWire(Pin(0)))

    """
     * @brief       Read DS18X20 sensor value
     * @param       None
     * @retval      None
    """
    def read_sensor():
        # Detect DS18X20 devices
        roms = ds_18b20.scan()
        # Get temperature value
        ds_18b20.convert_temp()
        for rom in roms:
            temp = ds_18b20.read_temp(rom)
            if isinstance(temp, float):
                temp = round(temp, 2)
                return temp

    while True:
        # Read sensor value
        temperature = float(read_sensor())
        display.num(30 + 11 * 8, 130, int(temperature), 2, 16, lcd.RED)             # Display integer part of temperature in red color
        display.num(30 + 14 * 8, 130, int(temperature * 100 % 100), 2, 16, lcd.RED) # Display fractional part of temperature in red color
        led_state = led.value()
        led.value(not led_state)
        time.sleep(1)  # Sleep for 1 second

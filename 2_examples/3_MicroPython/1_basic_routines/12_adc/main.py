"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       ADC example
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

from machine import Pin,ADC,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
import time


"""
 * @brief       Calculate the average value
 * @param       times: Number of times to take the reading
 * @retval      average value
"""
def adc_get_result_average(times):
    
    temp_val = 0
    
    for i in range(0, times):
        # Read a raw analog value
        temp_val += adc.read()
        
    return temp_val / times
    

"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    
    adcdata = 0
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
    xl9555.write_bit(io_ex.SLCD_PWR, 1)     # Set SLCD_PWR bit to 1
    time.sleep_ms(100)                      # Sleep for 100 milliseconds
    
    # Display experiment information on LCD
    display.string(30, 50, 240, 32, 32, "ESP32-S3", lcd.RED)            # Display "ESP32-S3" in red color
    display.string(30, 80, 240, 24, 24, "ADC TEST", lcd.RED)            # Display "ADC TEST" in red color
    display.string(30, 110, 240, 16, 16, "ATOM@ALIENTEK", lcd.RED)      # Display "ATOM@ALIENTEK" in red color
    display.string(30, 130, 200, 16, 16, "ADC:  0 . 0    V", lcd.BLUE)  # Display initial ADC value
    
    # Create an ADC object acting on pin 8
    adc = ADC(Pin(8))
    adc.atten(ADC.ATTN_11DB)
    adc.width(ADC.WIDTH_12BIT)  # 2^12 = 4095
    
    while True:  # Start an infinite loop

        # Read a raw analog value and calculate the average of 20 readings
        adcdata = adc_get_result_average(20)
        # Convert the raw value to voltage (assuming Vref=3.3V and 12-bit resolution)
        voltage = float(adcdata * (3.3 / 4096))
        # Display the integer part of voltage on LCD
        display.num(30 + 40, 130, int(voltage), 2, 16, lcd.RED)
        # Display the decimal part of voltage on LCD
        display.num(30 + 72, 130, int(voltage * 100 % 100), 2, 16, lcd.RED)
        # Toggle the LED state
        led_state = led.value()
        led.value(not led_state)
        time.sleep_ms(100)  # Sleep for 100 milliseconds


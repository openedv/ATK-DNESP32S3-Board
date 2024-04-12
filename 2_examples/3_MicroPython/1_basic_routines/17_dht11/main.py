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

    # set pin high on creation
    led = Pin(1,Pin.OUT,value = 1)

    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    xl9555 = io_ex.init(i2c0)
    
    # reset LCD
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
    display.string(5, 10, 240, 32, 32, "ESP32-S3",lcd.RED)
    display.string(5, 43, 240, 24, 24, "DHT11 TEST",lcd.RED)
    display.string(5, 68, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(5, 84, 200, 16, 16, "temperature: 00.00C", lcd.RED)
    display.string(5, 100, 200, 16, 16, "humidity: 00.00H", lcd.RED)

    dht11 = dht.DHT11(Pin(0, Pin.IN, Pin.PULL_UP))
    
    while True:
        
        # measured data 
        dht11.measure()
        # read Temperature data
        temperature = float(dht11.temperature())
        # read Humidity data
        humidity = float(dht11.humidity())

        display.num(5 + 13 * 8,84,int(temperature),2,16,lcd.BLUE)
        display.num(5 + 16 * 8,84,int(temperature * 100 % 100),2,16,lcd.BLUE)
        display.num(5 + 10 * 8,100,int(humidity),2,16,lcd.BLUE)
        display.num(5 + 13 * 8,100,int(humidity * 100 % 100),2,16,lcd.BLUE)
        led_state = led.value()
        led.value(not led_state)
        time.sleep(1)

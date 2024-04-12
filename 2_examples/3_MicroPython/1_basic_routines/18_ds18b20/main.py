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
    display.string(30, 50, 240, 32, 32, "ESP32-S3",lcd.RED)
    display.string(30, 80, 240, 24, 24, "DS18B20 TEST",lcd.RED)
    display.string(30, 110, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(30, 130, 200, 16, 16, "TEMPERATE: 00.00C", lcd.BLUE)

    ds_18b20 = ds18x20.DS18X20(onewire.OneWire(Pin(0)))

    """
     * @brief       read ds18x20 value
     * @param       None
     * @retval      None
    """
    def read_sensor():
        # devices detected
        roms = ds_18b20.scan()
        # get value
        ds_18b20.convert_temp()
        for rom in roms:
            temp = ds_18b20.read_temp(rom)
            if isinstance(temp, float):
                temp = round(temp, 2)
                return temp

    while True:
        # read sensor
        umber = float(read_sensor())
        display.num(30 + 11 * 8,130,int(umber),2,16,lcd.RED)
        display.num(30 + 14 * 8,130,int(umber * 100 % 100),2,16,lcd.RED)
        led_state = led.value()
        led.value(not led_state)
        time.sleep(1)

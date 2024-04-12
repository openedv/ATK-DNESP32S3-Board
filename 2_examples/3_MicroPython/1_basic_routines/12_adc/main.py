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
 * @param       times:Number
 * @retval      average value
"""
def adc_get_result_average(times):
    
    temp_val = 0
    
    for i in range(0,times):
        # read a raw analog value
        temp_val += adc.read()
        
    return temp_val / times
    

"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    
    adcdata = 0
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
    display.string(30, 80, 240, 24, 24, "ADC TEST",lcd.RED)
    display.string(30, 110, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(30, 130, 200, 16, 16, "ADC:  0 . 0    V", lcd.BLUE)
    # create an ADC object acting on a pin
    adc = ADC(Pin(8))
    adc.atten(ADC.ATTN_11DB)
    adc.width(ADC.WIDTH_12BIT)  #2^12 = 4095
    
    while True:

        # read a raw analog value
        adcdata = adc_get_result_average(20)
        # calculate
        umber = float(adcdata * (3.3 / 4096))
        display.num(30 + 40 ,130,int(umber),2,16,lcd.RED)
        display.num(30 + 72 ,130,int(umber * 100 % 100),2,16,lcd.RED)
        led_state = led.value()
        led.value(not led_state)
        time.sleep_ms(100)


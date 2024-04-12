"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       QMA9100P example
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
import atk_qma6100p as qma6100p
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
    
    qma6100 = qma6100p.init(i2c0)
    
    # display experiment information
    display.string(30, 50, 200, 16, 16, "ESP32-S3",lcd.RED)
    display.string(30, 70, 200, 16, 16, "QMA6100P TEST", lcd.RED)
    display.string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", lcd.RED)
    display.string(30, 110, 200, 16, 16, "acc_x:", lcd.RED)
    display.string(30, 130, 200, 16, 16, "acc_y:", lcd.RED)
    display.string(30, 150, 200, 16, 16, "acc_z:", lcd.RED)
    display.string(30, 170, 200, 16, 16, "acc_g:", lcd.RED)
    display.string(30, 190, 200, 16, 16, "pitch:", lcd.RED)
    display.string(30, 210, 200, 16, 16, "roll:", lcd.RED)
    
    while True :
        
        qma6100.qma6100p_read()
        display.string(110, 110, 200, 16, 16,str(qma6100.qma6100p_acc_x()),lcd.BLUE)
        display.string(110, 130, 200, 16, 16,str(qma6100.qma6100p_acc_y()),lcd.BLUE)
        display.string(110, 150, 200, 16, 16,str(qma6100.qma6100p_acc_z()),lcd.BLUE)
        display.string(110, 170, 200, 16, 16,str(qma6100.qma6100p_acc_g()),lcd.BLUE)
        display.string(110, 190, 200, 16, 16,str(qma6100.qma6100p_acc_pitch()),lcd.BLUE)
        display.string(110, 210, 200, 16, 16,str(qma6100.qma6100p_acc_roll()),lcd.BLUE)
        led_state = led.value()
        led.value(not led_state)
        time.sleep_ms(100)         # sleep for 100 milliseconds

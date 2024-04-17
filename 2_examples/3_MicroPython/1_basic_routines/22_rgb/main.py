"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       RGBLCD example
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

from machine import Pin,I2C
import atk_xl9555 as io_ex
import atk_ltdc as ltdc
import time


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
   
    x = 0
    # Set pin high on creation
    led = Pin(1, Pin.OUT, value=1)
    # Initialize I2C communication with specified pins and frequency
    i2c0 = I2C(0, scl=Pin(42), sda=Pin(41), freq=400000)
    # Initialize XL9555 IO expansion module
    xl9555 = io_ex.init(i2c0)
    
    # Initialize LTDC display
    display = ltdc.init(dir=1)
    # Turn on the backlight
    xl9555.write_bit(io_ex.LCD_BL, 1)
    time.sleep_ms(100)
    
    while True:
        
        # Create dictionary for colors
        seasondict = {
            0: ltdc.BLACK,
            1: ltdc.BLUE,
            2: ltdc.RED,
            3: ltdc.GREEN,
            4: ltdc.CYAN,
            5: ltdc.MAGENTA,
            6: ltdc.YELLOW}

        # Refresh color
        display.clear(seasondict[x])
        display.string(0, 5, 240, 32, 32, "ESP32S3", ltdc.RED)
        display.string(0, 34, 240, 16, 16, "RGB Test", ltdc.RED)
        display.string(0, 50, 240, 16, 16, "ATOM@ALIENTEK", ltdc.RED)
        x += 1
        
        if x == 7:
            x = 0
        
        led_state = led.value()
        led.value(not led_state)
        time.sleep(1)


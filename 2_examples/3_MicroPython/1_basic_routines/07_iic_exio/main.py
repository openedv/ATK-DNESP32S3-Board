"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       IIC_EXIO example
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

from machine import Pin, I2C
import atk_xl9555 as io_ex
import time


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    # This block is executed if the script is run as the main program.

    # Set pin 1 as an output pin and initialize it with a high value (1).
    led = Pin(1, Pin.OUT, value=1)

    # Initialize I2C communication on bus 0 with SCL pin 42 and SDA pin 41, at a frequency of 400 kHz.
    i2c0 = I2C(0, scl=Pin(42), sda=Pin(41), freq=400000)

    # Initialize the XL9555 device using the I2C communication.
    xl9555 = io_ex.init(i2c0)

    while True:
        # This loop runs indefinitely.

        # Get the value of the key from the XL9555 device.
        key = int(xl9555.key_scan())

        if key == io_ex.KEY0:
            xl9555.write_bit(io_ex.BEEP, 0)     # Turn on the buzzer.
        elif key == io_ex.KEY1:
            xl9555.write_bit(io_ex.BEEP, 1)     # Turn off the buzzer.
        elif key == io_ex.KEY2:
            led.value(0)                        # Turn off the LED (set pin to low).
        elif key == io_ex.KEY3:
            led.value(1)                        # Turn on the LED (set pin to high).

        # Sleep for 10 milliseconds.
        time.sleep_ms(10)


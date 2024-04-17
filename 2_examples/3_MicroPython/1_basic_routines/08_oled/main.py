"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       OLED example
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
import atk_oled as oled
import time


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    # This block is executed if the script is run as the main program.

    # Set pin 46 as an output pin and initialize it with a high value (1).
    led = Pin(46, Pin.OUT, value=1)

    # Initialize I2C communication on bus 0 with SCL pin 42 and SDA pin 41, at a frequency of 400 kHz.
    i2c0 = I2C(0, scl=Pin(42), sda=Pin(41), freq=400000)

    # Initialize I2C communication on bus 1 with SCL pin 4 and SDA pin 5, at a frequency of 400 kHz.
    i2c1 = I2C(1, scl=Pin(4), sda=Pin(5), freq=400000)

    # Initialize the XL9555 device using the I2C communication.
    xl9555 = io_ex.init(i2c0)

    # Set pin 38 as an output pin and initialize it with a low value (0).
    dc = Pin(38, Pin.OUT, value=0)

    # Reset the XL9555 device.
    xl9555.write_bit(io_ex.OV_RESET, 0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_RESET, 1)
    time.sleep_ms(100)

    # Initialize the OLED display using the I2C communication.
    display = oled.init(i2c1)

    # Display experiment information on the OLED screen.
    display.string(0, 0, str("ALIENTEK"), 24)
    display.string(0, 24, str("0.96' OLED TEST"), 16)
    display.string(0, 40, str("ATOM 2023/09/13"), 12)
    display.string(0, 52, str("ASCII:"), 12)
    display.string(64, 52, str("CODE:"), 12)
    display.refresh_gram()

    t = ' '

    while True:
        # This loop runs indefinitely.

        # Display ASCII characters and their corresponding codes on the OLED screen.
        display.char(36, 52, t, 12, 1)
        display.num(94, 52, ord(t), 3, 12)
        display.refresh_gram()

        # Increment the character for the next iteration.
        t = chr(ord(t) + 1)

        # Reset the character to ' ' if it exceeds '~'.
        if t > '~':
            t = ' '

        # Sleep for 500 milliseconds.
        time.sleep_ms(500)

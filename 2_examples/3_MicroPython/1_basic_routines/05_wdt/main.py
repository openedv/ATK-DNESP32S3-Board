"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       WDT example
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

from machine import Pin,I2C,WDT
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

    # Sleep for 100 milliseconds.
    time.sleep_ms(100)

    # Initialize I2C communication on bus 0 with SCL pin 42 and SDA pin 41, at a frequency of 400 kHz.
    i2c0 = I2C(0, scl=Pin(42), sda=Pin(41), freq=400000)

    # Initialize the XL9555 device using the I2C communication.
    xl9555 = io_ex.init(i2c0)

    # Enable the Watchdog Timer (WDT) with a timeout of 3 seconds.
    wdt = WDT(timeout=3000)

    # Turn off the LED.
    led.value(0)

    while True:
        # This loop runs indefinitely.

        # Get the value of the key from the XL9555 device.
        key = int(xl9555.key_scan())

        if key == io_ex.KEY0:
            # If the key is pressed, feed the watchdog to prevent it from timing out.
            wdt.feed()

        # Sleep for 10 milliseconds.
        time.sleep_ms(10)



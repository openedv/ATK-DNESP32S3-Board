"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       KEY example
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

from machine import Pin
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

    # Set pin 0 as an input pin with a pull-up resistor enabled.
    key = Pin(0, Pin.IN, Pin.PULL_UP)

    while True:
        # This loop runs indefinitely.

        if key.value() == 0:                # Check if the key is pressed (value is 0).

            time.sleep_ms(10)               # Sleep for 10 milliseconds.

            if key.value() == 0:            # Check again if the key is still pressed.
                led_state = led.value()     # Read the current state of the LED.
                led.value(not led_state)    # Toggle the LED state (turn it on if it's off and vice versa).

                while not key.value():
                    # This loop waits until the key is released.
                    pass

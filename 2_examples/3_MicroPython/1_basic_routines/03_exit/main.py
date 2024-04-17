"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       EXIT example
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
 * @brief       Interrupt service function
 * @param       key:handle
 * @retval      None
"""
def KEY_INT_IRQHandler(key):
    # This function is the interrupt service routine triggered by the key interrupt.

    time.sleep_ms(10)  # Sleep for 10 milliseconds.

    if key.value() == 0:
        global led_state  # Access the global variable led_state.
        led_state = led.value()  # Read the current state of the LED.
        led.value(not led_state)  # Toggle the LED state (turn it on if it's off and vice versa).

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

    # Define an interrupt on the key pin, triggered on the falling edge, which will call KEY_INT_IRQHandler function.
    key.irq(KEY_INT_IRQHandler, Pin.IRQ_FALLING)

    # Main loop that runs indefinitely.
    while True:
        pass  # Do nothing (placeholder for further code).
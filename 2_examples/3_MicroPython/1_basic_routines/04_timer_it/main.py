"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       TIMER example
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

from machine import Pin,Timer
import time


"""
 * @brief       Interrupt service function
 * @param       tim:handle
 * @retval      None
"""
def BTMR_TIMEX_INT_IRQHandler(tim):
    # This function is the interrupt service routine triggered by the timer interrupt.
    
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

    # Initialize a timer with timer ID 1.
    tim = Timer(1)

    # Initialize the timer to trigger an interrupt every 1000 milliseconds (1 second)
    # in periodic mode and call the BTMR_TIMEX_INT_IRQHandler function when triggered.
    tim.init(period=1000, mode=Timer.PERIODIC, callback=BTMR_TIMEX_INT_IRQHandler)


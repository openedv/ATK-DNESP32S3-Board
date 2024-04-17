"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       LED_PWM example
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

from machine import Pin,PWM
import time


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    # This block is executed if the script is run as the main program.

    # Construct a PWM (Pulse Width Modulation) object with an LED connected to pin 1.
    pwm = PWM(Pin(1), freq=1000)

    while True:
        # This loop runs indefinitely.

        # Fade the LED in and out a few times.

        # Increase the duty cycle gradually from 0 to 1023.
        for i in range(0, 1024):
            pwm.duty(i)  # Set the duty cycle of the PWM signal.
            time.sleep_ms(1)  # Sleep for 1 millisecond.

        # Decrease the duty cycle gradually from 1023 to 0.
        for i in range(1023, 0, -1):
            pwm.duty(i)  # Set the duty cycle of the PWM signal.
            time.sleep_ms(1)  # Sleep for 1 millisecond.


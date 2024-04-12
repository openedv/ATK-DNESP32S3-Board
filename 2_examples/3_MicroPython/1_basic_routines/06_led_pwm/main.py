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
    
    # construct pwm object, with led on pin(1).
    pwm = PWM(Pin(1),freq = 1000)
    
    while True:
        
        # fade the led in and out a few times.
        for i in range(0,1024):
            pwm.duty(i)
            time.sleep_ms(1)
            
        for i in range(1023,0,-1):
            pwm.duty(i)
            time.sleep_ms(1)

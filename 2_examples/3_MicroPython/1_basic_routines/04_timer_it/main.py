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
    led_state = led.value()
    led.value(not led_state)


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    
    # set pin high on creation
    led = Pin(1,Pin.OUT,value = 1)
    tim = Timer(1)
    tim.init(period = 1000, mode = Timer.PERIODIC,callback = BTMR_TIMEX_INT_IRQHandler)

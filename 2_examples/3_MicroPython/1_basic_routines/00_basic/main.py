"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       BASIC example
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

import machine
import esp
import time
import micropython


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if  __name__ == '__main__':
    
    esp.osdebug(1)                                      # turn off vendor O/S debugging messages
    esp.osdebug(0)                                      # redirect vendor O/S debugging messages to UART(0)
    freq = machine.freq()                               # get the current frequency of the CPU
    print(f'CPU frequency{freq}')
    print(f'flash size{esp.flash_size()/1024/1024}MB')  # get flash size
    micropython.mem_info()
    
    while True:
        
        time.sleep_ms(10)                               # sleep for 10 milliseconds

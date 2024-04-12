"""
 ****************************************************************************************************
 * @file        remote.py
 * @author      ALIENTEK
 * @brief       AP3216C example
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
 * Class REMOTE
"""
class REMOTE_IR(object):

    # define key value
    REMOTE_CODE = {
        0: "ERROR", 162: "POWER", 98: "UP",
        2: "PLAY", 226: "ALIENTEK", 194: "RIGHT",
        34: "LEFT", 224: "VOL-", 168: "DOWN",
        144: "VOL+", 104: "1", 152: "2",
        176: "3", 48: "4", 24: "5",
        122: "6", 16: "7", 56: "8",
        90: "9", 66: "0", 82: "DELETE"
    }

    def __init__(self, gpio_num):
        self.irRecv = Pin(gpio_num, Pin.IN, Pin.PULL_UP)
        self.irRecv.irq(trigger = Pin.IRQ_RISING | Pin.IRQ_FALLING, handler = self.ex_handler)
        self.ir_step = 0
        self.ir_count = 0
        self.rx_buf = [0 for i in range(64)]
        self.rx_ok = False
        self.cmd = None
        self.repeat = 0
        self.t_ok = None
        self.start = 0
        self.start_last = 0

    # interrupt service function
    def ex_handler(self, source):

        thisComeInTime = time.ticks_us()

        # update time
        curtime = time.ticks_diff(thisComeInTime, self.start)
        self.start = thisComeInTime
        

        if curtime >= 8500 and curtime <= 9500:
            self.ir_step = 1
            return

        if self.ir_step == 1:
            if curtime >= 4000 and curtime <= 5000:
                self.ir_step = 2
                self.rx_ok = False
                self.ir_count = 0
            elif curtime >= 2000 and curtime <= 3000:
                self.ir_step = 3
                self.repeat += 1

        elif self.ir_step == 2:
            self.rx_buf[self.ir_count] = curtime
            self.ir_count += 1
            if self.ir_count >= 64:
                self.rx_ok = True
                self.t_ok = self.start
                self.ir_step = 0

        elif self.ir_step == 3:
            if curtime >= 500 and curtime <= 650:
                self.repeat += 1

    # detection command
    def check_cmd(self):
        one_byte = 0
        for i in range(32):
            x = i * 2
            t = self.rx_buf[x] + self.rx_buf[x + 1]
            one_byte <<= 1
            if t >= 1800 and t <= 2800:
                one_byte += 1
        cmd_data = (one_byte & 0x0000ff00) >> 8
        self.cmd = cmd_data
    # infrared decoding
    def remote_scan(self):
        if self.rx_ok:
            self.check_cmd()
            self.rx_ok = False
        s = self.REMOTE_CODE.get(self.cmd)
        return self.cmd,s


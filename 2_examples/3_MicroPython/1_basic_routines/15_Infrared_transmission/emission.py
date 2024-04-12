"""
 ****************************************************************************************************
 * @file        emission.py
 * @author      ALIENTEK
 * @brief       EMISSION example
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

from esp32 import RMT
from machine import Pin, PWM
from micropython import const
from micropython import const
from array import array
from time import ticks_us, ticks_diff


# Shared by NEC
STOP = const(0)  # End of data
_TBURST = const(563)
_T_ONE = const(1687)

# IR abstract base class. Array holds periods in μs between toggling 36/38KHz
# carrier on or off. Physical transmission occurs in an ISR context controlled
# by timer 2 and timer 5. See TRANSMITTER.md for details of operation.
class IR:
    _active_high = True  # Hardware turns IRLED on if pin goes high.
    _space = 0  # Duty ratio that causes IRLED to be off
    timeit = False  # Print timing info

    def active_low(cls):
        cls._active_high = False
        cls._space = 100

    def __init__(self, pin, cfreq, asize, duty, verbose):
        self._rmt = RMT(0, pin = pin, clock_div = 80, tx_carrier = (cfreq, duty, 1))  # 1μs resolution
        
        self._tcb = self._cb  # Pre-allocate
        self._arr = array('H', 0 for _ in range(asize))  # on/off times (μs)
        self._mva = memoryview(self._arr)
        # Subclass interface
        self.verbose = verbose
        self.carrier = False  # Notional carrier state while encoding biphase
        self.aptr = 0  # Index into array

    def _cb(self, t):  # T5 callback, generate a carrier mark or space
        t.deinit()
        p = self.aptr
        v = self._arr[p]
        
        if v == STOP:
            self._ch.pulse_width_percent(self._space)  # Turn off IR LED.
            return
        
        self._ch.pulse_width_percent(self._space if p & 1 else self._duty)
        self._tim.init(prescaler = 84, period = v, callback = self._tcb)
        self.aptr += 1
        
    # Public interface
    # Before populating array, zero pointer, set notional carrier state (off).
    def transmit(self, addr, data, toggle = 0, validate = False):  # NEC: toggle is unused
        t = ticks_us()
        if validate:
            if addr > self.valid[0] or addr < 0:
                raise ValueError('Address out of range', addr)
            if data > self.valid[1] or data < 0:
                raise ValueError('Data out of range', data)
            if toggle > self.valid[2] or toggle < 0:
                raise ValueError('Toggle out of range', toggle)
        self.aptr = 0  # Inital conditions for tx: index into array
        self.carrier = False
        self.tx(addr, data, toggle)  # Subclass populates ._arr
        self.trigger()  # Initiate transmission
        if self.timeit:
            dt = ticks_diff(ticks_us(), t)
            print('Time = {}μs'.format(dt))
            
    # Subclass interface
    def trigger(self):  # Used by NEC to initiate a repeat frame
        self._rmt.write_pulses(tuple(self._mva[0 : self.aptr]))
        
    def append(self, *times):  # Append one or more time peiods to ._arr
        for t in times:
            self._arr[self.aptr] = t
            self.aptr += 1
            self.carrier = not self.carrier  # Keep track of carrier state
            self.verbose and print('append', t, 'carrier', self.carrier)

    def add(self, t):  # Increase last time value (for biphase)
        assert t > 0
        self.verbose and print('add', t)
        # .carrier unaffected
        self._arr[self.aptr - 1] += t
        
class NEC(IR):
    valid = (0xffff, 0xff, 0)  # Max addr, data, toggle
    samsung = False
    
    def __init__(self, pin, freq = 38000, verbose = False):  # NEC specifies 38KHz
        super().__init__(pin, freq, 68, 33, verbose)  # Measured duty ratio 33%

    def _bit(self, b):
        self.append(_TBURST, _T_ONE if b else _TBURST)
    
    def m2m(self,data):
        y = 0
        
        for i in range(8):
            if ((data >> i) & 0x01) == 1:
                y |= (0x01 << (7 - i))
        
        return y
    
    def tx(self, addr, data, _):  # Ignore toggle
        
        if self.samsung:
            self.append(4500, 4500)
        else:
            self.append(9000, 4500)
        data = self.m2m(data)

        data = ((~data & 0xFF) << 24) + ((data & 0xFF) << 16) + ((~addr & 0xff) << 8) + addr
        for _ in range(32):
            self._bit((data >> _) & 0x00000001)
        self.append(_TBURST)

    def repeat(self):
        self.aptr = 0
        self.append(9000, 2250, _TBURST)
        self.trigger()  # initiate physical transmission.

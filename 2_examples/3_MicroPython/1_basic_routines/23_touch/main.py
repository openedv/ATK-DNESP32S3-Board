"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       TOUCH example
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

from machine import Pin,I2C
import atk_xl9555 as io_ex
import atk_ltdc as ltdc
import atk_touch as touch
import time


# set screen orientation: 0 is vertical screen; 1 is landscape screen
atk_dir = 1

if atk_dir == 1:
    LCD_WIDTH  = 800
    LCD_HEIGHT = 480
else:
    LCD_WIDTH  = 480
    LCD_HEIGHT = 800

"""
 * @brief       RST
 * @param       None
 * @retval      None
"""
def load_draw_dialog():
    
    display.clear(ltdc.WHITE)
    display.string(LCD_WIDTH - 30, 0, 200, 16, 16, "RST", ltdc.BLUE)


"""
 * @brief       Draw lines
 * @param       x1,y1: Starting point coordinates
 * @param       x2,y2: End point coordinates
 * @param       size : size
 * @param       color: color
 * @retval      None
"""
def lcd_draw_bline(x1,y1,x2,y2,size,color):
    
    t = 0
    xerr = 0
    yerr = 0
    delta_x = 0
    delta_y = 0
    distance = 0
    incx = 0
    incy = 0
    row = 0
    col = 0

    delta_x = x2 - x1                       # calculate coordinate increment
    delta_y = y2 - y1
    row = x1
    col = y1

    if delta_x > 0:
        incx = 1
    elif delta_x == 0:
        incx = 0
    else:
        incx = -1
        delta_x = -delta_x

    if delta_y > 0:
        incy = 1
    elif delta_y == 0:
        incy = 0
    else:
        incy = -1
        delta_y = -delta_y

    if delta_x > delta_y:
        distance = delta_x;
    else:
        distance = delta_y

    for t in range(0,distance + 1):
        display.circle(row, col, size, color)
        xerr += delta_x
        yerr += delta_y

        if xerr > distance:
            xerr -= distance
            row += incx

        if yerr > distance:
            yerr -= distance
            col += incy

"""
 * @brief       Test
 * @param       None
 * @retval      None
"""
def ctp_test():
    
    t = 0
    i = 0
    lastpos = [[0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0]]

    while True:
        
        touch.scan(0)
        
        x_data = touch.get_x()
        y_data = touch.get_y()
        sta_data = touch.get_sta()
        
        x = (x_data[9] << 8) | x_data[8]
        y = (y_data[9] << 8) | y_data[8]
        sta = (sta_data[1] << 8) | sta_data[0]
        
        for t in range(0,5):
            
            if sta & (1 << t):
                
                if (x < LCD_WIDTH and y < LCD_HEIGHT):
                    
                    if lastpos[t][0] == 0xFFFF:
                        
                        lastpos[t][0] = x
                        lastpos[t][1] = y
                    
                    lcd_draw_bline(lastpos[t][0], lastpos[t][1], x,y, 2, ltdc.RED);
                    lastpos[t][0] = x
                    lastpos[t][1] = y
                    
                    if (x > (LCD_WIDTH - 30) and y < 20):
                        
                        load_draw_dialog()
                else:
                    lastpos[t][0] = 0xFFFF
                
        time.sleep_ms(5)
        
        i += 1
        
        if (i % 20 == 0):
            
            led_state = led.value()
            led.value(not led_state)


"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
   
    x = 0
    # set pin high on creation
    led = Pin(1,Pin.OUT,value = 1)

    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    xl9555 = io_ex.init(i2c0)
    
    display = ltdc.init(dir = atk_dir)
    # # turn on the backlight
    xl9555.write_bit(io_ex.LCD_BL,1)
    time.sleep_ms(100)
    
    # reset touch ic
    xl9555.write_bit(io_ex.CT_RST,0)
    time.sleep_ms(10)
    xl9555.write_bit(io_ex.CT_RST,1)
    time.sleep_ms(10)
    
    touch.init()
    
    # RST
    load_draw_dialog()
    
    while True:
        # test
        ctp_test()

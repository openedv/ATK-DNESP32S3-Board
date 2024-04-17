"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       WebCAM example
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

import network
import time
import camera
import atk_xl9555 as io_ex
from machine import Pin,SPI,I2C
from microdot import Microdot


SSID = "xxx"          # WiFi name
PASSWORD = "xxx"      # WiFi password

"""
 * @brief       Connect to the network
 * @param       None
 * @retval      None
"""
def connect():
    
    wlan = network.WLAN(network.STA_IF)
    wlan.active(False)
    wlan.active(True)
    
    if not wlan.isconnected():
        print('Connecting to network...')
        wlan.connect(SSID, PASSWORD)
        while not wlan.isconnected():
            pass
    print('Network config: ', wlan.ifconfig())

"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    
    # Connect to the network
    connect()
    # Establishing a Microdot website server
    app = Microdot()
    time.sleep_ms(1000)
    i2c0 = I2C(0, scl=Pin(42), sda=Pin(41), freq=400000)
    xl9555 = io_ex.init(i2c0)
    xl9555.write_bit(io_ex.BEEP, 1)
    # Reset CAMERA
    xl9555.write_bit(io_ex.OV_RESET, 0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_RESET, 1)
    time.sleep_ms(100)
    # Open CAMERA
    xl9555.write_bit(io_ex.OV_PWDN, 1)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_PWDN, 0)
    time.sleep_ms(100)
    
    for i in range(5):
        cam = camera.init(0, format=camera.JPEG, fb_location=camera.PSRAM, framesize=camera.FRAME_240X240, xclk_freq=24000000)
        print("Camera ready?: ", cam)
        if cam:
            print("Camera ready")
            break
        else:
            camera.deinit()
            camera.init(0, format=camera.JPEG, fb_location=camera.PSRAM, framesize=camera.FRAME_240X240, xclk_freq=24000000)
            time.sleep(2)
    else:
        print('Timeout')
        reset()

    time.sleep_ms(1000)
    
    @app.route('/')
    def index(request):
        return '''<!doctype html>
    <html>
      <head>
        <title>Microdot Video Streaming</title>
      </head>
      <body>
        <h1>ATK WebCAM </h1>
        <img src="/video_feed" width="30%">
      </body>
    </html>''', 200, {'Content-Type': 'text/html'}

    @app.route('/video_feed')
    def video_feed(request):
        def stream():
            yield b'--frame\r\n'
            while True:
                frame = camera.capture()
                yield b'Content-Type: image/jpeg\r\n\r\n' + frame + \
                    b'\r\n--frame\r\n'

        return stream(), 200, {'Content-Type':
                               'multipart/x-mixed-replace; boundary=frame'}
    app.run(debug=True)

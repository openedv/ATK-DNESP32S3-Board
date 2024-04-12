"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       BLE example
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
import bluetooth


BLE_MESSAGE = ""

"""
 * @brief       Ble
 * @param       None
 * @retval      None
"""
class ESP32S3_BLE():

    def __init__(self, name):
        self.timer1 = Timer(0)
        self.name = name
        self.ble = bluetooth.BLE()
        self.ble.active(True)
        self.ble.config(gap_name=name)
        self.disconnected()
        self.ble.irq(self.ble_irq)
        self.register()
        self.advertiser()

    def connected(self):
        led.value(1)
        self.timer1.deinit()

    def disconnected(self):        
        self.timer1.init(period=1000, mode=Timer.PERIODIC, callback=lambda t: led.value(not led.value()))

    # bluetooth callback function
    def ble_irq(self, event, data):
        
        global BLE_MESSAGE
        if event == 1:
            self.connected()
        elif event == 2:
            self.advertiser()
            self.disconnected()
        elif event == 3:  
            buffer = self.ble.gatts_read(self.rx)
            BLE_MESSAGE = buffer.decode('UTF-8').strip()
            
    def register(self):
        service_uuid = '6E400001-B5A3-F393-E0A9-E50E24DCCA9E'
        reader_uuid = '6E400002-B5A3-F393-E0A9-E50E24DCCA9E'
        sender_uuid = '6E400003-B5A3-F393-E0A9-E50E24DCCA9E'
        services = (
            (
                bluetooth.UUID(service_uuid),
                (
                    (bluetooth.UUID(sender_uuid), bluetooth.FLAG_NOTIFY),
                    (bluetooth.UUID(reader_uuid), bluetooth.FLAG_WRITE),
                )
            ),
        )
        ((self.tx, self.rx,), ) = self.ble.gatts_register_services(services)

    def send(self, data):
        self.ble.gatts_notify(1, self.tx, data + '\n')

    def advertiser(self):
        name = bytes(self.name, 'UTF-8')
        adv_data = bytearray('\x02\x01\x02', 'UTF-8') + bytearray((len(name) + 1, 0x09), 'UTF-8') + name
        self.ble.gap_advertise(100, adv_data)

"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == "__main__":
    
    led = Pin(1, Pin.OUT,value = 1)
    ble = ESP32S3_BLE("ESP32-S3 BLUE")
    
    while True:
        
        if BLE_MESSAGE == 'LED ON':
            led.value(0)
            print('LED is ON.')
            ble.send('LED is ON.')
            BLE_MESSAGE = ""
        elif BLE_MESSAGE == 'LED OFF':
            led.value(1)
            print('LED is OFF.')
            ble.send('LED is OFF.')
            BLE_MESSAGE = ""
        
        time.sleep_ms(100)

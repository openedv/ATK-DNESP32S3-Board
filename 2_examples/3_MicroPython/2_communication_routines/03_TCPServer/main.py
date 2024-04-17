"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       TCPServer example
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

import socket
import network
import time


SSID = "xxx"          # WiFi name
PASSWORD = "xxx"      # WiFi password
wlan = None
pos = 0

"""
 * @brief       Connect to the network
 * @param       None
 * @retval      None
"""
def connect():
    
    global wlan
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
 * @brief       goto
 * @param       label: flag
 * @retval      None
"""
def goto(label):

    global pos
    pos = label

"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""  
if __name__ == '__main__':
    
    # Connect to the network
    connect()
    
    while True:
        
        while pos == 0:
            # Get local IP
            ip = wlan.ifconfig()[0]
            print('Network config:', ip)

            server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, True)
            server_socket.bind(('', 8080))
            server_socket.listen(128)
            try:
                new_socket, client_ip_port = server_socket.accept()
            except:
                new_socket.close()
                server_socket.close()
                goto(0)
                break
            
            new_socket.send("**********************************\r\n")
            new_socket.send(f'ALIENTEK ESP32-S3 Board TCPServer\r\n'.encode('utf-8'))
            new_socket.send("**********************************\r\n")
            
            while True:
                try:
                    # Receive 4096 bytes each time
                    data = new_socket.recv(4096)
                except:
                    new_socket.close()
                    server_socket.close()
                    break

                if len(data) == 0:
                    new_socket.close()
                    server_socket.close()
                    break
                
                # Echo
                new_socket.send(data)

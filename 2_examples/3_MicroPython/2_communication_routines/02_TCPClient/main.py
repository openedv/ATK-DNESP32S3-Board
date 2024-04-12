"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       TCPClient example
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


SSID = "xxx"                    # wifi name
PASSWORD = "xxx"                # wifi password
Server_IP = '192.168.101.33'    # remote IP address
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
        print('connecting to network...')
        wlan.connect(SSID, PASSWORD)
        while not wlan.isconnected():
            pass
    print('network config: ', wlan.ifconfig())

"""
 * @brief       goto
 * @param       label:flag
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
    
    # connect to the network
    connect()
    
    while True:
        
        while pos == 0:
            # get local IP
            ip = wlan.ifconfig()[0]
            
            client_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
            print('network config:', ip)
            
            try:
                client_socket.connect((Server_IP,8080))
            except BaseExc1eption:
                client_socket.close()
                goto(0)
                break
            
            print('Connection established successfully......')

            client_socket.send("**********************************\r\n")
            client_socket.send(f'ALIENTEK ESP32-S3 Board TCPClient\r\n'.encode('utf-8'))
            client_socket.send("**********************************\r\n")
            
            while True:
                try:
                    # receive 4096 bytes each time
                    data = client_socket.recv(4096)
                except BaseExc1eption:
                    client_socket.close()
                    break
  
                if (len(data) == 0):
                
                    print("close socket")
                    client_socket.close()
                    break

                # echo
                client_socket.send(data)

"""
 ****************************************************************************************************
 * @file        main.py
 * @author      ALIENTEK
 * @brief       UDP example
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
Server_IP = '192.168.2.245'      # remote IP address
wlan = None

"""
 * @brief       Connect to the network
 * @param       None
 * @retval      None
"""
def connect():
    
    global wlan
    # create site interface
    wlan = network.WLAN(network.STA_IF)
    wlan.active(False)
    # enable site interface
    wlan.active(True)
    
    # determine whether to connect
    if not wlan.isconnected():
        print('connecting to network...')
        # connect wifi
        wlan.connect(SSID, PASSWORD)
        while not wlan.isconnected():
            pass
    print('network config: ', wlan.ifconfig())

"""
 * @brief       The application entry point
 * @param       None
 * @retval      None
"""
if __name__ == '__main__':
    
    # connect to the network
    connect()
    
    while True:
        # get local IP
        ip = wlan.ifconfig()[0]
        print('network config:', ip)

        udp_socket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        # serverIP+ServerPort
        addr = socket.getaddrinfo('192.168.101.33', 8080)[0][-1]
        print(addr)
        # Sending messages
        udp_socket.sendto("**********************************\r\n",addr)
        udp_socket.sendto(f'ALIENTEK ESP32-S3 Board UDP Test\r\n'.encode('utf-8'),addr)
        udp_socket.sendto("**********************************\r\n",addr)
        
        while True:
            try:
                # receive 4096 bytes each time
                data = udp_socket.recv(4096)
            except :
                udp_socket.close()
                break
            # echo
            udp_socket.sendto(data,addr)


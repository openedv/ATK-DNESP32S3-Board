/**
 ****************************************************************************************************
 * @file        04_ble_scan.ino
 * @author      ALIENTEK
 * @brief       BLE_SCAN example
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
 */

#include "led.h"
#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include <BLEDevice.h>            
#include <BLEUtils.h>
#include <BLEScan.h>              
#include <BLEAdvertisedDevice.h>  


int scanTime = 5;       
BLEScan* pBLEScan;      
char ble_addr[100];     
uint8_t show_index = 0;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks  /* BLE broadcast callback function (called every time a scan is made to a broadcast device) */
{
    void onResult(BLEAdvertisedDevice advertisedDevice) 
    {
        lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "BLE_DEV_Name", BLUE);  
        lcd_show_string(238, 110, 200, 16, LCD_FONT_16, "RSSI", BLUE);  

        if (advertisedDevice.haveName())
        {
            lcd_fill(30, 130  + show_index * 20, 319, 148  + show_index * 20, WHITE);
            lcd_show_string(30, 130  + show_index * 20, 300, 16, LCD_FONT_16, (char *)advertisedDevice.getName().c_str(), BLUE);    

            if (advertisedDevice.haveRSSI()) 
            {
                char ble_rssi[20];   
                sprintf(ble_rssi, "%4d", advertisedDevice.getRSSI());
                lcd_show_string(238, 130  + show_index * 20, 200, 16, LCD_FONT_16, ble_rssi, RED);   
            }

            show_index++;
            if (show_index == 4)  show_index = 0;   /* The screen only shows 4 BLE devices */
        }
        // Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());   /* Output device information */
    }
};

/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    led_init();             /* led initialization */
    uart_init(0, 115200);   /* uart0 initialization */
    xl9555_init();          /* xl9555 initialization */
    lcd_init();             /* lcd initialization */
    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "BLE SCAN TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "Scanning...", RED);
    BLEDevice::init("ESP BLEDevice");   /* create a BLE device */
    pBLEScan = BLEDevice::getScan();    /* create a new scan */
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());  /* initializes the callback function */
    pBLEScan->setActiveScan(true);      /* active scanning consumes more energy but gets results faster */
    pBLEScan->setInterval(100);         /* set scan interval */
    pBLEScan->setWindow(99);            /* set window size */
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);   /* Start the BLE scan and call the callback function when the scan reaches the broadcast device */
    lcd_show_string(120, 210, 200, 16, LCD_FONT_16, "Devices found: ", RED);
    lcd_show_num(240, 210, foundDevices.getCount(), 2, LCD_FONT_16, RED);
    lcd_show_string(30, 210, 200, 16, LCD_FONT_16, "Scan done!", RED);
    pBLEScan->clearResults();   /* Removes the result from the BLEScan buffer to free memory */  
    delay(2000);
}

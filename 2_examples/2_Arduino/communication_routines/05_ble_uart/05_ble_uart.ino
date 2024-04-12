/**
 ****************************************************************************************************
 * @file        05_ble_uart.ino
 * @author      ALIENTEK
 * @brief       BLE_UART example
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
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


BLEServer *pServer = NULL;              
BLECharacteristic * pTxCharacteristic; 
BLECharacteristic * pRxCharacteristic;  
bool deviceConnected = false;           
bool oldDeviceConnected = false;        
uint8_t txValue = 0;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"   
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"   
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"   

class MyServerCallbacks: public BLEServerCallbacks   
{
    void onConnect(BLEServer* pServer) 
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) 
    {
        deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks 
{
    void onWrite(BLECharacteristic *pCharacteristic) 
    {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0)
        {
            lcd_show_string(30, 150, 290, 16, LCD_FONT_16, "rec value:", RED);
            for (int i = 0; i < rxValue.length(); i++)
            {
                lcd_fill(110 + i * 8, 150, 319, 168, WHITE);
                lcd_show_char(110 + i * 8, 150, rxValue[i], LCD_FONT_16, 0, BLUE);
            }
        }

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
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "BLE UART TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    BLEDevice::init("ESP32-S3 BLE Service");          /* create a BLE device */

    pServer = BLEDevice::createServer();              /* create a BLE service */
    pServer->setCallbacks(new MyServerCallbacks());   /* set callback */

    BLEService *pService = pServer->createService(SERVICE_UUID); /* creating a Bluetooth Server */

    /* create send feature, add descriptor, set notification permissions/Create Receive feature, set callback function, set writable permissions */
    pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902());
    pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
    pRxCharacteristic->setCallbacks(new MyCallbacks()); 

    pService->start();                      /* start service */

    pServer->getAdvertising()->start();     /* start broadcast */
    lcd_show_string(30, 110, 290, 16, LCD_FONT_16, "Waiting client connect to notify", RED);
    delay(500);
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    if (deviceConnected)   /* connect to device */
    {
        lcd_show_string(30, 110, 290, 16, LCD_FONT_16, "client connected                ", RED);
        pTxCharacteristic->setValue(&txValue, 1);   /* Set the value to be sent to 1 */
        lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "notify value:", RED);
        pTxCharacteristic->notify();                /* broadcast txValue++ */
        lcd_show_num(134, 130, txValue, 3, LCD_FONT_16, BLUE);
        txValue++;
        delay(10);                                  /* if too many packets are sent, the Bluetooth stack will become congested */ 
    }

    if (!deviceConnected && oldDeviceConnected)     /* disconnect */
    {
        lcd_fill(0, 110, 319, 239, WHITE);          /* clear display area */
        delay(500);                                 /* the Bluetooth stack has a chance to be ready */
        pServer->startAdvertising();                /* restart broadcast */
        lcd_show_string(30, 110, 290, 16, LCD_FONT_16, "start advertising...               ", RED);
        oldDeviceConnected = deviceConnected;
    }

    if (deviceConnected && !oldDeviceConnected)     /* connecting */
    {
        oldDeviceConnected = deviceConnected;
    }
}

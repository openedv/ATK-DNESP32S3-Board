/**
 ****************************************************************************************************
 * @file        04_uart.ino
 * @author      ALIENTEK
 * @brief       UART example
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

#include "uart.h"


uint32_t chip_id = 0;

/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    uart_init(0, 115200);   /* uart0 initialization */

    for(int i = 0; i < 17; i = i + 8) 
    {
        chip_id |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }

    Serial.printf("ESP32 Chip model = %s Rev %d \n", ESP.getChipModel(), ESP.getChipRevision());   
    Serial.printf("This chip has %d cores \n", ESP.getChipCores());
    Serial.print("Chip ID: "); Serial.println(chip_id);
    Serial.printf("CpuFreqMHz: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("SdkVersion: %s \n", ESP.getSdkVersion());
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop()
{
    Serial.println("Waitting for Serial Data  \n");   

    while (Serial.available() > 0)                    /* send data when you receive data */
    {
        Serial.println("Serial Data Available...");   

        String serial_data;                           

        int c = Serial.read();                        /* read the incoming byte */
        while (c >= 0)
        {
            serial_data += (char)c;                   
            c = Serial.read();                       
        }

        Serial.print("Received Serial Data: ");       /* say what you got: */
        Serial.println(serial_data);                  
    }

    delay(1000);
}

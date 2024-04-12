/**
 ****************************************************************************************************
 * @file        08_iic_eeprom.ino
 * @author      ALIENTEK
 * @brief       EERPOM example
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
 
#include "24c02.h"
#include "key.h"
#include "uart.h"


const uint8_t g_text_buf[] = {"ESP32S3 IIC TEST"};  /* write to 24c02 data */
#define TEXT_SIZE   sizeof(g_text_buf)              /* write data length */
uint8_t datatemp[TEXT_SIZE];                        /* read data buf */

/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    key_init();               /* key initialization */
    uart_init(0, 115200);     /* uart0 initialization */
    at24c02_init();           /* 24C02 initialization */
    
    while (at24c02_check())   /* check 24c02 */
    {
        Serial.println("24C02 Check Failed!");
        delay(500);
    }
    Serial.println("24C02 Ready!");
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    at24c02_read(0, datatemp, TEXT_SIZE);                       /* read data from 24c02 addr0 */
    Serial.printf("The Data Readed Is:%s \r\n", datatemp);

    if (KEY == 0)
    {
        at24c02_write(0, (uint8_t *)g_text_buf, TEXT_SIZE);     /* write data to 24c02 addr0 */
        Serial.printf("24C02 Write %s Finished! \r\n", g_text_buf);
    }

    delay(1000);
}

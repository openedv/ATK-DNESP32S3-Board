/**
 ****************************************************************************************************
 * @file        24c02.cpp
 * @author      ALIENTEK
 * @brief       EEPROM code
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
#include <Wire.h>

/**
 * @brief       configure 24C02
 * @param       None
 * @retval      None
 */
void at24c02_init(void)
{
    Wire.begin(IIC_SDA, IIC_SCL, 400000);    /* iic initialization */
}

/**
 * @brief       read data from 24c02
 * @param       addr: read data address
 * @retval      data / 0xFF:no data
 */
uint8_t at24c02_read_one_byte(uint8_t addr)
{
    Wire.beginTransmission(EEPROM_ADDR);      /* i2c address of device */
    Wire.write(addr);                         /* address to read */
    Wire.endTransmission(0);                  /* end */

    Wire.requestFrom(EEPROM_ADDR, 1);         /* number of bytes to read */
    if (Wire.available() != 0)                /* have data? */
    {
        return Wire.read();                   /* read data */
    }

    return 0xFF;
}

/**
 * @brief       write data to 24c02
 * @param       addr: write data address
 * @param       data: write data 
 * @retval      None
 */
void at24c02_write_one_byte(uint8_t addr, uint8_t data)
{
    Wire.beginTransmission(EEPROM_ADDR);    /* i2c address of device */
    Wire.write(addr);                       /* address to write */
    Wire.write(data);                       /* data to write */
    Wire.endTransmission(1);                /* end */

    delay(10);    /* Note: EEPROM is slow to write and must wait until 10ms before writing a byte */
}

/**
 * @brief       check AT24C02
 *   @note      Detection principle: Write the value 0X55 at the end address of the device, 
 *              and then read the value. If the read value is 0X55, the detection is normal. 
 *              Otherwise, the detection fails.
 * @param       None
 * @retval      0: succeed, 1:failed
 */
uint8_t at24c02_check(void)
{
    uint8_t temp;

    temp = at24c02_read_one_byte(255);      /* read data from addr 255 */

    if (temp == 0X55)                       /* data is correct */
    {
        return 0;
    }
    else                                    /* first time */
    {
        at24c02_write_one_byte(255, 0X55);  /* write data first */
        temp = at24c02_read_one_byte(255);  /* then read data */
        
        if (temp == 0X55) 
        {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief       read bytes data from 24c02
 * @param       addr: read data address
 * @param       pbuf: data buf
 * @param       datalen: read data length 
 * @retval      None
 */
void at24c02_read(uint8_t addr, uint8_t *pbuf, uint8_t datalen)
{
    while (datalen--)
    {
        *pbuf++ = at24c02_read_one_byte(addr++);
    }
}

/**
 * @brief       write bytes data to 24c02
 * @param       addr: write data address
 * @param       pbuf: data buf
 * @param       datalen: write data length 
 * @retval      None
 */
void at24c02_write(uint8_t addr, uint8_t *pbuf, uint8_t datalen)
{
    while (datalen--)
    {
        at24c02_write_one_byte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}

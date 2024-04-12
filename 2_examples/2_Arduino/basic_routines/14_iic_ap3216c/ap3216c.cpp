/**
 ****************************************************************************************************
 * @file        ap3216c.cpp
 * @author      ALIENTEK
 * @brief       AP3216C code
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

#include "ap3216c.h"
#include <Wire.h>

/**
 * @brief       configure ap3216c 
 * @param       None
 * @retval      0:succeed;1:failed
 */
uint8_t ap3216c_init(void)
{
    uint8_t temp;

    Wire.begin(IIC_SDA, IIC_SCL, 400000);   /* iic initialization */

    ap3216c_write_one_byte(0x00, 0X04);     /* ap3216c reset */
    delay(50);                              /* at least 10ms */

    ap3216c_write_one_byte(0x00, 0X03);     /* enable ALS and PS+IR */

    temp = ap3216c_read_one_byte(0X00);     /* verify that the configuration is successful */
    if (temp == 0X03)
    {
        return 0;           /* succeed */
    }
    else 
    {
        return 1;           /* failed */
    }
}

/**
 * @brief       write data to ap3216c register
 * @param       reg: register address
 * @param       data: write data 
 * @retval      None
 */
void ap3216c_write_one_byte(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(AP3216C_ADDR);     /* i2c address of device */
    Wire.write(reg);                          /* register to write */
    Wire.write(data);                         /* data to write */
    Wire.endTransmission();                   /* end */         
}

/**
 * @brief       read data from ap3216c register
 * @param       reg: register
 * @retval      register data / 0xFF:none
 */
uint8_t ap3216c_read_one_byte(uint8_t reg)
{
    uint8_t rd_num = 0;
    uint8_t rd_data = 0;

    Wire.beginTransmission(AP3216C_ADDR);       /* i2c address of device */
    Wire.write(reg);                            /* register to read */
    Wire.endTransmission(0);                    /* end */

    rd_num = Wire.requestFrom(AP3216C_ADDR, 1); /* 1 bytes to read */

    if (Wire.available() != 0)                  /* have data? */
    {
        return Wire.read();                     /* read data */
    }

    return 0xFF;
}

/**
 * @brief       read AP3216C data(ALS,PS+IR)
 * @note        The data read interval must be greater than 112.5ms
 * @param       ir      : IR data
 * @param       ps      : PS data
 * @param       als     : ALS data
 * @retval      None
 */
void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als)
{
    uint8_t buf[6];
    uint8_t i;

    for (i = 0; i < 6; i++)
    {
        buf[i] = ap3216c_read_one_byte(0X0A + i);                   /* reads all sensor data */
    }

    if (buf[0] & 0X80)
    {
        *ir = 0;                                                    /* IR_OF bit is 1,the data is invalid */
    }
    else 
    {
        *ir = ((uint16_t)buf[1] << 2) | (buf[0] & 0X03);            /* read IR sensor data */
    }

    *als = ((uint16_t)buf[3] << 8) | buf[2];                        /* read ALS sensor data */ 

    if (buf[4] & 0x40) 
    {
        *ps = 0;                                                    /* IR_OF bit is 1,the data is invalid */
    }
    else
    {
        *ps = ((uint16_t)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F);   /* read PS sensor data */
    }
}


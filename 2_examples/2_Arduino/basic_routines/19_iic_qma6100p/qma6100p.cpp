/**
 ****************************************************************************************************
 * @file        qma6100p.cpp
 * @author      ALIENTEK
 * @brief       QMA6100P code
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

#include "qma6100p.h"
#include <Wire.h>


#define M_G     9.80665f

/**
 * @brief       write data to qma6100p register
 * @param       reg  : register address
 * @param       data : write data
 * @retval      None
 */
void qma6100p_write_reg(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(IMU_DEV_ADDR);     /* i2c address of device */
    Wire.write(reg);                          /* register to write */
    Wire.write(data);                         /* data to write */
    Wire.endTransmission();                   /* end */
}

/**
 * @brief       read bytes data from qma6100p
 * @param       reg  : register address
 * @param       buf  : buffer
 * @param       num  : length
 * @retval      None
 */
void qma6100p_read_reg(uint8_t reg, uint8_t *buf, uint16_t num)
{
    uint8_t i = 0;

    Wire.beginTransmission(IMU_DEV_ADDR);     /* i2c address of device */
    Wire.write(reg);                          /* register to read */
    Wire.endTransmission(0);                  /* end */

    Wire.requestFrom(IMU_DEV_ADDR, num);      /* num bytes to read */

    while (Wire.available())                  /* have data? */
    {
        buf[i++] = Wire.read();               /* read data */
    }
}

/**
* @brief       configure qma6100p
* @param       None
* @retval      0:succeed; !0:failed
*/
uint8_t qma6100p_init(void)
{
    uint8_t id = 0;

    Wire.begin(IIC_SDA, IIC_SCL, 400000);           /* i2c  initialization */

    qma6100p_read_reg(QMA6100P_CHIP_ID, &id, 1);    /* read qma6100p ID */
    if (id != QMA6100P_DEVICE_ID)
    {
        Serial.printf("ID:%#x \r\n", id);
        return 0xFF;
    }

    /* software reset */
    qma6100p_write_reg(QMA6100P_REG_RESET, 0xb6);
    delay(5);
    qma6100p_write_reg(QMA6100P_REG_RESET, 0x00);
    delay(10);

    /* recommended initialization sequence */
    qma6100p_write_reg(0x11, 0x80);   
    qma6100p_write_reg(0x11, 0x84);
    qma6100p_write_reg(0x4a, 0x20);
    qma6100p_write_reg(0x56, 0x01);
    qma6100p_write_reg(0x5f, 0x80);
    delay(2);
    qma6100p_write_reg(0x5f, 0x00);
    delay(10);

    /* set qma6100p configuration parameters (range, output frequency, operating mode) */
    qma6100p_write_reg(QMA6100P_REG_RANGE, QMA6100P_RANGE_8G);                              
    qma6100p_write_reg(QMA6100P_REG_BW_ODR, (uint8_t)(QMA6100P_BW_100 | QMA6100P_LPF_OFF)); 
    qma6100p_write_reg(QMA6100P_REG_POWER_MANAGE, (uint8_t)QMA6100P_MCLK_51_2K | 0x80);     

    return 0;
}

/**
 * @brief       read data from QMA6100P
 * @param       data  : 3-axis data
 * @retval      None
 */
void qma6100p_read_raw_xyz(int16_t data[3])
{
    uint8_t databuf[6] = {0}; 
    int16_t raw_data[3];

    qma6100p_read_reg(QMA6100P_XOUTL, databuf, 6);

    raw_data[0] = (int16_t)(((databuf[1] << 8)) | (databuf[0]));
    raw_data[1] = (int16_t)(((databuf[3] << 8)) | (databuf[2]));
    raw_data[2] = (int16_t)(((databuf[5] << 8)) | (databuf[4]));

    data[0] = raw_data[0] >> 2;
    data[1] = raw_data[1] >> 2;
    data[2] = raw_data[2] >> 2;
}

/**
 * @brief       calculate the x,y, and z axis data of the accelerometer
 * @param       accdata  : 3-axis data
 * @retval      None
 */
void qma6100p_read_acc_xyz(float accdata[3]) 
{
    int16_t rawdata[3];

    qma6100p_read_raw_xyz(rawdata);

    accdata[0] = (float)(rawdata[0] * M_G) / 1024;
    accdata[1] = (float)(rawdata[1] * M_G) / 1024;
    accdata[2] = (float)(rawdata[2] * M_G) / 1024;
}


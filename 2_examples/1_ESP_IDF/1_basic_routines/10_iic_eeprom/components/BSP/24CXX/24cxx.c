/**
 ****************************************************************************************************
 * @file        24cxx.c
 * @author      ALIENTEK
 * @brief       24CXX code
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

#include "24cxx.h"


i2c_obj_t at24cxx_master;

/**
 * @brief       configure 24C02
 * @param       self:i2c handle
 * @retval      None
 */
void at24cxx_init(i2c_obj_t self)
{
    at24cxx_master = self;

    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_0);        /* Initialize IIC0 */
    }
}

/**
 * @brief       read data from 24c02
 * @param       addr: read data address
 * @retval      data / 0xFF:no data
 */
uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    uint8_t data = 0;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);

    if(EE_TYPE > AT24C16)
    {
        /* i2c address of device */
        i2c_master_write_byte(cmd, (AT_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
        i2c_master_write_byte(cmd, addr >> 8, ACK_CHECK_EN);
    }
    else
    {
        i2c_master_write_byte(cmd, 0XA0 + ((addr / 256) << 1), ACK_CHECK_EN);
    }
    i2c_master_write_byte(cmd, addr % 256, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AT_ADDR << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(at24cxx_master.port, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(10);

    return data;
}

/**
 * @brief       write data to 24c02
 * @param       addr: write data address
 * @param       data: write data 
 * @retval      None
 */
void at24cxx_write_one_byte(uint16_t addr, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
        
    if(EE_TYPE > AT24C16)
    {
        /* i2c address of device */
        i2c_master_write_byte(cmd, (AT_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
        i2c_master_write_byte(cmd, addr >> 8, ACK_CHECK_EN);
    }
    else
    {
        /* i2c address of device */
        i2c_master_write_byte(cmd, 0XA0 + ((addr/256) << 1), ACK_CHECK_EN);
    }

    i2c_master_write_byte(cmd, addr % 256, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(at24cxx_master.port, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(10);
}

/**
 * @brief       check AT24C02
 *   @note      Detection principle: Write the value 0X55 at the end address of the device, 
 *              and then read the value. If the read value is 0X55, the detection is normal. 
 *              Otherwise, the detection fails.
 * @param       None
 * @retval      0: succeed, 1:failed
 */
uint8_t at24cxx_check(void)
{
    uint8_t temp;
    uint16_t addr = EE_TYPE;

    temp = at24cxx_read_one_byte(addr);     /* read data from addr 255 */

    if (temp == 0X55)                       /* data is correct */
    {
        return 0;
    }
    else                                    /* first time */
    {
        at24cxx_write_one_byte(addr, 0X55); /* write data first */
        temp = at24cxx_read_one_byte(255);  /* then read data */

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
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        *pbuf++ = at24cxx_read_one_byte(addr++);
    }
}

/**
 * @brief       write bytes data to 24c02
 * @param       addr: write data address
 * @param       pbuf: data buf
 * @param       datalen: write data length 
 * @retval      None
 */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        at24cxx_write_one_byte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}

/**
 ****************************************************************************************************
 * @file        ap3216c.c
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

#define __IIC_VERSION__  "1.0"

#include "ap3216c.h"


i2c_obj_t ap3216c_master;

/**
 * @brief       read data from ap3216c register
 * @param       reg: register
 * @retval      ESP_OK:None
 */
static esp_err_t ap3216c_read_one_byte(uint8_t* data, uint8_t reg)
{
    uint8_t reg_buf[1];

    reg_buf[0] = reg;

    i2c_buf_t buf[2] = {
        {.len = 1, .buf = reg_buf},
        {.len = 1, .buf = data},
    };

    i2c_transfer(&ap3216c_master, AP3216C_ADDR, 2, buf, I2C_FLAG_WRITE | I2C_FLAG_READ | I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief       write data to ap3216c register
 * @param       reg: register address
 * @param       data: write data 
 * @retval      ESP_OK:None
 */
static esp_err_t ap3216c_write_one_byte(uint8_t reg, uint8_t data)
{
    uint8_t reg_buf[1];

    reg_buf[0] = reg;
    
    i2c_buf_t buf[2] = {
        {.len = 1, .buf = reg_buf},
        {.len = 1, .buf = &data},
    };

    i2c_transfer(&ap3216c_master, AP3216C_ADDR, 2, buf, I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief       configure ap3216c 
 * @param       None
 * @retval      0:succeed;1:failed
 */
uint8_t ap3216c_comfig(void)
{
    uint8_t temp;
    
    ap3216c_write_one_byte(0x00, 0X04); /* ap3216c reset */
    vTaskDelay(50);                     /* sleep for 50 milliseconds */
    ap3216c_write_one_byte(0x00, 0X03); /* enable ALS and PS+IR */
    ap3216c_read_one_byte(&temp,0X00);  /* verify that the configuration is successful */

    if (temp == 0X03)
    {
        ESP_LOGE("ap3216c", "AP3216C success!!!");
        return 0;
    }
    else
    {
        ESP_LOGE("ap3216c", "AP3216C fail!!!");
        return 1;
    }
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
    uint8_t buf[6] = {0};
    uint8_t i;

    for (i = 0; i < 6; i++)
    {
        ap3216c_read_one_byte(&buf[i],0X0A + i);                    /* reads all sensor data */
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

/**
 * @brief       Initialize the AP3216C
 * @param       None
 * @retval      None
 */
void ap3216c_init(i2c_obj_t self)
{
    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_0);    /* Initialize IIC  */
    }

    ap3216c_master = self;

    while (ap3216c_comfig())
    {
        ESP_LOGE("ap3216c", "ap3216c init fail!!!");
        vTaskDelay(500);
    }
}

/**
 ****************************************************************************************************
 * @file        qma6100p.c
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


i2c_obj_t qma6100p_i2c_master;

#define I2C_MASTER_TIMEOUT_MS       1000
#define M_PI                        3.141592653589793f
#define M_G                         9.80665f
#define RAD_TO_DEG                  (180.0f / M_PI)                         /* 0.017453292519943295 */

/**
 * @brief       read bytes data from qma6100p
 * @param       reg    : register address
 * @param       data   : buffer
 * @param       len    : length
 * @retval      ESP_OK : None
 */
esp_err_t qma6100p_register_read(const uint8_t reg, uint8_t *data, const size_t len)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0]  = reg;

    i2c_buf_t bufs[2] = {
        {.len = 1, .buf = memaddr_buf},
        {.len = len, .buf = data},
    };

    i2c_transfer(&qma6100p_i2c_master, QMA6100P_ADDR, 2, bufs,I2C_FLAG_WRITE | I2C_FLAG_READ | I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief       write data to qma6100p register
 * @param       reg  : register address
 * @param       data : write data
 * @retval      ESP_OK : None
 */
static esp_err_t qma6100p_register_write_byte(uint8_t reg, uint8_t data)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0]  = reg;

    i2c_buf_t bufs[2] = {
        {.len = 1, .buf = memaddr_buf},
        {.len = 1, .buf = &data},
    };

    i2c_transfer(&qma6100p_i2c_master, QMA6100P_ADDR, 2, bufs,I2C_FLAG_STOP);
    return ESP_OK;
}

uint8_t xyz_data[6] = {0};
short raw_data[3] = {0};
float accl_data[3];
float acc_normal;

/**
 * @brief       Read 3-axis data (raw data, acceleration, pitch angle, and roll angle)
 * @param       rawdata：qma6100p Data structure
 * @retval      None
 */
void qma6100p_read_rawdata(qma6100p_rawdata_t *rawdata)
{
    qma6100p_register_read(QMA6100P_REG_XOUTL, xyz_data, 6);
    
    raw_data[0] = (short)(((xyz_data[1] << 8)) | (xyz_data[0]));
    raw_data[1] = (short)(((xyz_data[3] << 8)) | (xyz_data[2]));
    raw_data[2] = (short)(((xyz_data[5] << 8)) | (xyz_data[4]));
    
    rawdata->acc_x = (float)((raw_data[0] >> 2) * M_G) / (1024);
    rawdata->acc_y = (float)((raw_data[1] >> 2) * M_G) / (1024);
    rawdata->acc_z = (float)((raw_data[2] >> 2) * M_G) / (1024);
    
    rawdata->acc_g = sqrt(rawdata->acc_x*rawdata->acc_x + rawdata->acc_y * rawdata->acc_y + rawdata->acc_z*rawdata->acc_z);
    
    acc_normal = sqrtf(rawdata->acc_x * rawdata->acc_x + rawdata->acc_y * rawdata->acc_y + rawdata->acc_z * rawdata->acc_z);
    accl_data[0] = rawdata->acc_x / acc_normal;
    accl_data[1] = rawdata->acc_y / acc_normal;
    accl_data[2] = rawdata->acc_z / acc_normal;

    rawdata->pitch = -atan2f(rawdata->acc_x, rawdata->acc_z) * RAD_TO_DEG;
    
    acc_normal = sqrtf(accl_data[0] * accl_data[0] + accl_data[1] * accl_data[1] + accl_data[2] * accl_data[2]);
    
    rawdata->roll = asinf((accl_data[1]/acc_normal)) * RAD_TO_DEG ;
}

/**
 * @brief       Configure Interrupt
 * @param       int_map :Interrupt mode
 * @param       enable  :Enable
 * @retval      None
 */
void qma6100p_step_int_config(int int_map, int enable)
{
    uint8_t    reg_16 = 0;
    uint8_t    reg_19 = 0;
    uint8_t    reg_1b = 0;

    qma6100p_register_read(0x16, &reg_16, 1);
    qma6100p_register_read(0x19, &reg_19, 1);
    qma6100p_register_read(0x1b, &reg_1b, 1);
    
    if (enable)
    {
        reg_16 |= 0x08;
        reg_19 |= 0x08;
        reg_1b |= 0x08;
        qma6100p_register_write_byte(0x16, reg_16);
        
        if(int_map == QMA6100P_MAP_INT1)
        {
            qma6100p_register_write_byte(0x19, reg_19);
        }
        else if(int_map == QMA6100P_MAP_INT2)
        {
            qma6100p_register_write_byte(0x1b, reg_1b);
        }
    }
    else
    {
        reg_16 &= (~0x08);
        reg_19 &= (~0x08);
        reg_1b &= (~0x08);

        qma6100p_register_write_byte(0x16, reg_16);
        qma6100p_register_write_byte(0x19, reg_19);
        qma6100p_register_write_byte(0x1b, reg_1b);
    }
}

/**
* @brief       Configure qma6100p
* @param       None
* @retval      0:succeed; !0:failed
*/
uint8_t qma6100p_comfig(void)
{
    static uint8_t id_data[2];
    qma6100p_register_read(QMA6100P_REG_CHIP_ID, id_data, 1);    /* read qma6100p ID */
    
    /* software reset */
    qma6100p_register_write_byte(QMA6100P_REG_RESET, QMA6100P_RESET);
    vTaskDelay(5);
    qma6100p_register_write_byte(QMA6100P_REG_RESET, QMA6100P_RESET_END);
    vTaskDelay(10);
    
    qma6100p_register_read(QMA6100P_REG_CHIP_ID, id_data, 1);    /* read qma6100p ID */
    
    qma6100p_register_write_byte(0x11, 0x80);
    qma6100p_register_write_byte(0x11, 0x84);
    qma6100p_register_write_byte(0x4a, 0x20);
    qma6100p_register_write_byte(0x56, 0x01);
    qma6100p_register_write_byte(0x5f, 0x80);
    vTaskDelay(1);
    qma6100p_register_write_byte(0x5f, 0x00);
    vTaskDelay(10);

    qma6100p_register_write_byte(QMA6100P_REG_RANGE, QMA6100P_RANGE_8G);
    qma6100p_register_write_byte(QMA6100P_REG_BW_ODR, QMA6100P_BW_100);
    qma6100p_register_write_byte(QMA6100P_REG_POWER_MANAGE, QMA6100P_MCLK_51_2K | 0x80);

    qma6100p_register_write_byte(0x21, 0x03);    /* default 0x1c, step latch mode */
    
    qma6100p_step_int_config(QMA6100P_MAP_INT1, 1);
    
    if (id_data[0] == 0x90)
    {
        ESP_LOGI("qma6100p", "qma6100p success");
        return 0;
    }
    else
    {
        ESP_LOGE("qma6100p", "qma6100p fail");
        return 1;
    }
} 

/**
* @brief       qma6100p initialization
* @param       None
* @retval      None
*/
void qma6100p_init(i2c_obj_t self)
{
    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_0);
    }

    qma6100p_i2c_master = self;
    
    while (qma6100p_comfig())
    {
        ESP_LOGE("qma6100p", "qma6100p init fail!!!");
        vTaskDelay(500);
    }
}

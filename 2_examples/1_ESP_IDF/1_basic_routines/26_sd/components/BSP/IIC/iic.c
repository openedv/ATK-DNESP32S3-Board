/**
 ****************************************************************************************************
 * @file        iic.c
 * @author      ALIENTEK
 * @brief       IIC code
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

#include "iic.h"


i2c_obj_t iic_master[I2C_NUM_MAX];

/**
 * @brief       i2c master initialization
 * @param       iic_port:IIC0/IIC1
 * @retval      IIC Handle
 */
i2c_obj_t iic_init(uint8_t iic_port)
{
    uint8_t i;
    i2c_config_t iic_config_struct = {0};

    if (iic_port == I2C_NUM_0)
    {
        i = 0;
    }
    else
    {
        i = 1;
    }
    
    iic_master[i].port = iic_port;
    iic_master[i].init_flag = ESP_FAIL;

    if (iic_master[i].port == I2C_NUM_0)
    {
        iic_master[i].scl = IIC0_SCL_GPIO_PIN;
        iic_master[i].sda = IIC0_SDA_GPIO_PIN;
    }
    else
    {
        iic_master[i].scl = IIC1_SCL_GPIO_PIN;
        iic_master[i].sda = IIC1_SDA_GPIO_PIN;
    }

    iic_config_struct.mode = I2C_MODE_MASTER;                               /* I2C mode */
    iic_config_struct.sda_io_num = iic_master[i].sda;                       /* GPIO number for I2C sda signal */
    iic_config_struct.scl_io_num = iic_master[i].scl;                       /* GPIO number for I2C scl signal */
    iic_config_struct.sda_pullup_en = GPIO_PULLUP_ENABLE;                   /* Internal GPIO pull mode for I2C sda signal */
    iic_config_struct.scl_pullup_en = GPIO_PULLUP_ENABLE;                   /* Internal GPIO pull mode for I2C scl signal */
    iic_config_struct.master.clk_speed = IIC_FREQ;                          /* I2C clock frequency for master mode, (no higher than 1MHz for now) */
    i2c_param_config(iic_master[i].port, &iic_config_struct);               /* Configure an I2C bus with the given configuration */

    /* Install an I2C driver */
    iic_master[i].init_flag = i2c_driver_install(iic_master[i].port,        /* I2C port number */
                                                 iic_config_struct.mode,    /* I2C mode (either master or slave) */
                                                 I2C_MASTER_RX_BUF_DISABLE, /* Receiving buffer size. Only slave mode will use this value, it is ignored in master mode */
                                                 I2C_MASTER_TX_BUF_DISABLE, /* Sending buffer size. Only slave mode will use this value, it is ignored in master mode */
                                                 0);                        /* Flags used to allocate the interrupt. One or multiple (ORred) ESP_INTR_FLAG_* values.See esp_intr_alloc.h for more info */

    if (iic_master[i].init_flag != ESP_OK)
    {
        while(1)
        {
            printf("%s , ret: %d", __func__, iic_master[i].init_flag);
            vTaskDelay(1000);
        }
    }

    return iic_master[i];
}

/**
 * @brief       read/write a byte to a device register
 * @param       self :i2c handle
 * @param       addr :Device address
 * @param       n    :data size
 * @param       bufs :Storage Area
 * @param       flags:flags
 * @retval      None
 */
esp_err_t i2c_transfer(i2c_obj_t *self, uint16_t addr, size_t n, i2c_buf_t *bufs, unsigned int flags)
{
    int data_len = 0;
    esp_err_t ret = ESP_FAIL;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    if (flags & I2C_FLAG_WRITE)
    {
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, addr << 1, ACK_CHECK_EN);
        i2c_master_write(cmd, bufs->buf, bufs->len, ACK_CHECK_EN);
        data_len += bufs->len; 
        --n;
        ++bufs;
    }

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1 | (flags & I2C_FLAG_READ), ACK_CHECK_EN);

    for (; n--; ++bufs)
    {
        if (flags & I2C_FLAG_READ)
        {
            i2c_master_read(cmd, bufs->buf, bufs->len, n == 0 ? I2C_MASTER_LAST_NACK : I2C_MASTER_ACK);
        }
        else
        {
            if (bufs->len != 0)
            {
                i2c_master_write(cmd, bufs->buf, bufs->len, ACK_CHECK_EN);
            }
        }
        data_len += bufs->len;
    }

    if (flags & I2C_FLAG_STOP)
    {
        i2c_master_stop(cmd);
    }

    /* TODO proper timeout */
    ret = i2c_master_cmd_begin(self->port, cmd, 100 * (1 + data_len) / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

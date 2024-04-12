/**
 ****************************************************************************************************
 * @file        gt9xxx.c
 * @author      ALIENTEK
 * @brief       GT9XXX code
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

#include "gt9xxx.h"


/* Default number of touch screen points supported (5 touch points) */
uint8_t g_gt_tnum = 5;

/**
 * @brief       write data to gt9xxx
 * @param       reg : register address
 * @param       buf : Data buffer storage area
 * @param       len : lenght
 * @retval      0:succeed;1:failed
 */
esp_err_t gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, GT9XXX_CMD_WR, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg >> 8, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg & 0XFF, ACK_CHECK_EN);
    i2c_master_write(cmd, buf, len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief       read gt9xxx data
 * @param       reg : register address
 * @param       buf : Data buffer storage area
 * @param       len : lenght
 * @retval      None
 */
esp_err_t gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    if (len == 0)
    {
        return ESP_OK;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, GT9XXX_CMD_WR, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg >> 8, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg & 0XFF, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000); 
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, GT9XXX_CMD_RD, ACK_CHECK_EN);
    if (len > 1)
    {
        i2c_master_read(cmd, buf, len - 1, 0);
    }
    i2c_master_read_byte(cmd, buf + len - 1, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret |= i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000);
    i2c_cmd_link_delete(cmd);

    return ret;
}

/**
 * @brief       gt9xxx initialization
 * @param       None
 * @retval      0:succeed;1:failed
 */
uint8_t gt9xxx_init(void)
{
    uint8_t temp[5];
    /* zero-initialize the config structure */
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;                 /* disable interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT;                        /* set as output mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_DISABLE;              /* disable pull-up mode */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;          /* disable pull-down mode */
    gpio_init_struct.pin_bit_mask = (1ull << GT9XXX_INT_GPIO_PIN);  /* bit mask of the pins that you want to set,e.g.GPIO1/2 */
    gpio_config(&gpio_init_struct);

    /* zero-initialize the config structure */
    i2c_config_t iic_config_struct = {0};

    iic_config_struct.mode = I2C_MODE_MASTER;                       /* I2C mode */
    iic_config_struct.sda_io_num = GT9XXX_IIC_SDA;                  /* GPIO number for I2C sda signal */
    iic_config_struct.scl_io_num = GT9XXX_IIC_CLK;                  /* GPIO number for I2C scl signal */
    iic_config_struct.sda_pullup_en = GPIO_PULLUP_ENABLE;           /* Internal GPIO pull mode for I2C sda signal */
    iic_config_struct.scl_pullup_en = GPIO_PULLUP_ENABLE;           /* Internal GPIO pull mode for I2C scl signal */
    iic_config_struct.master.clk_speed = GT9XXX_IIC_FREQ;           /* I2C clock frequency for master mode, (no higher than 1MHz for now) */
    i2c_param_config(GT9XXX_IIC_PORT, &iic_config_struct);          /* Configure an I2C bus with the given configuration */

    /* Install an I2C driver */
    i2c_driver_install(GT9XXX_IIC_PORT,                             /* I2C port number */
                       iic_config_struct.mode,                      /* I2C mode (either master or slave) */
                       I2C_MASTER_RX_BUF_DISABLE,                   /* Receiving buffer size. Only slave mode will use this value, it is ignored in master mode */
                       I2C_MASTER_TX_BUF_DISABLE,                   /* Sending buffer size. Only slave mode will use this value, it is ignored in master mode */ 
                       0);                                          /* Flags used to allocate the interrupt. One or multiple (ORred) ESP_INTR_FLAG_* values.See esp_intr_alloc.h for more info */ 

    for (int i = 2;i >= 0;i--)
    {
        CT_RST(0);
        vTaskDelay(200);
        CT_RST(1);
        vTaskDelay(200);
    }

    gt9xxx_rd_reg(GT9XXX_PID_REG, temp, 4);     /* resd ID */
    temp[4] = 0;

    if (strcmp((char *)temp, "911") && strcmp((char *)temp, "9147") && strcmp((char *)temp, "1158") && strcmp((char *)temp, "9271"))
    {
        return 1;
    }
    printf("CTP ID:%s\r\n", temp);
    
    if (strcmp((char *)temp, "9271") == 0)
    {
        g_gt_tnum = 10;
    }

    temp[0] = 0X02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);                    /*Soft reset GT9XXX */
    
    vTaskDelay(10);
    
    temp[0] = 0X00;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);

    return 0;
}

/* Register table corresponding to the maximum of 10 touch points in GT9XXX */
const uint16_t GT9XXX_TPX_TBL[10] =
{
    GT9XXX_TP1_REG, GT9XXX_TP2_REG, GT9XXX_TP3_REG, GT9XXX_TP4_REG, GT9XXX_TP5_REG,
    GT9XXX_TP6_REG, GT9XXX_TP7_REG, GT9XXX_TP8_REG, GT9XXX_TP9_REG, GT9XXX_TP10_REG,
};

/**
 * @brief       Scan touch screen (using query method)
 * @param       mode : None
 * @retval      Current touch screen status
 *   @arg       0:Touch screen without touch
 *   @arg       1:Touch screen with touch
 */
uint8_t gt9xxx_scan(uint8_t mode)
{
    uint8_t buf[4];
    uint8_t i = 0;
    uint8_t res = 0;
    uint16_t temp;
    uint16_t tempsta;
    static uint8_t t = 0;                                                               /* when idle, the CTP_Scan function is only detected once every 10 times, thereby saving CPU usage */
    t++;

    if ((t % 10) == 0 || t < 10)                                                        /* reading the status of touch points */
    {
        gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1);                                      /* 读取触摸点的状态 */

        if ((mode & 0X80) && ((mode & 0XF) <= g_gt_tnum))
        {
            i = 0;
            gt9xxx_wr_reg(GT9XXX_GSTID_REG, &i, 1);                                     /* clear flag */
        }

        if ((mode & 0XF) && ((mode & 0XF) <= g_gt_tnum))
        {
            temp = 0XFFFF << (mode & 0XF);
            tempsta = tp_dev.sta;
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            tp_dev.x[g_gt_tnum - 1] = tp_dev.x[0];
            tp_dev.y[g_gt_tnum - 1] = tp_dev.y[0];

            for (i = 0; i < g_gt_tnum; i++)
            {
                if (tp_dev.sta & (1 << i))
                {
                    gt9xxx_rd_reg(GT9XXX_TPX_TBL[i], buf, 4);

                    if (tp_dev.touchtype & 0X01)
                    {
                        tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
                        tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
                    }
                    else
                    {
                        tp_dev.x[i] = ltdcdev.width - (((uint16_t)buf[3] << 8) + buf[2]);
                        tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                    }
                }
            }

            res = 1;

            if (tp_dev.x[0] > ltdcdev.width || tp_dev.y[0] > ltdcdev.height)
            {
                if ((mode & 0XF) > 1)
                {
                    tp_dev.x[0] = tp_dev.x[1];
                    tp_dev.y[0] = tp_dev.y[1];
                    t = 0;
                }
                else
                {
                    tp_dev.x[0] = tp_dev.x[g_gt_tnum - 1];
                    tp_dev.y[0] = tp_dev.y[g_gt_tnum - 1];
                    mode = 0X80;
                    tp_dev.sta = tempsta;
                }
            }
            else 
            {
                t = 0;
            }
        }
    }

    if ((mode & 0X8F) == 0X80)
    {
        if (tp_dev.sta & TP_PRES_DOWN)
        {
            tp_dev.sta &= ~TP_PRES_DOWN;
        }
        else
        {
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
            tp_dev.sta &= 0XE000;
        }
    }

    if (t > 240)
    {
        t = 10;
    }

    return res;
}

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


uint8_t g_gt_tnum = 5;      /* Default number of touch screen points supported (5 touch points) */

/**
 * @brief       Delay clock
 * @param       ts : clock
 * @retval      None
 */
static __inline void delay_clock(int ts)
{
    uint32_t start, curr;

    __asm__ __volatile__("rsr %0, ccount" : "=r"(start));
    do
    {
        __asm__ __volatile__("rsr %0, ccount" : "=r"(curr));
        
    }while (curr - start <= ts);
}

/**
 * @brief       Delay microseconds
 * @param       us : microseconds
 * @retval      None
 */
void sys_delays_us(int us)
{
    while (us--)
    {
        delay_clock(240);
    }
}

/**
 * @brief       Millisecond delay
 * @param       ms : Millisecond
 * @retval      None
 */
void sys_delays_ms(int ms)
{
    while (ms--)
    {
        delay_clock(240 * 1000);
    }
}

/**
 * @brief       write data to gt9xxx
 * @param       reg : register address
 * @param       buf : Data buffer storage area
 * @param       len : lenght
 * @retval      0:succeed;1:failed
 */
void gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,GT9XXX_CMD_WR, ACK_CHECK_EN);
    i2c_master_write_byte(cmd,reg >> 8, ACK_CHECK_EN);
    i2c_master_write_byte(cmd,reg & 0XFF, ACK_CHECK_EN);
    i2c_master_write(cmd, buf, len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000);
    i2c_cmd_link_delete(cmd);
}

/**
 * @brief       read gt9xxx data
 * @param       reg : register address
 * @param       buf : Data buffer storage area
 * @param       len : lenght
 * @retval      None
 */
void gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,GT9XXX_CMD_WR, ACK_CHECK_EN);
    i2c_master_write_byte(cmd,reg >> 8, ACK_CHECK_EN);
    i2c_master_write_byte(cmd,reg & 0XFF, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000); 
    i2c_cmd_link_delete(cmd);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, GT9XXX_CMD_RD, ACK_CHECK_EN);
    
    if (len > 1)
    {
        i2c_master_read(cmd, buf, len - 1, ACK_VAL);
    }

    i2c_master_read_byte(cmd, buf + len - 1, NACK_VAL);

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000);
    i2c_cmd_link_delete(cmd);
}

/**
 * @brief       iic init
 * @param       port:port
 * @param       sda :sda
 * @param       scl :scl
 * @param       freq:clock
 * @retval      None
 */
void ct_iic_init(int port,int sda,int scl,uint32_t freq)
{
    static i2c_config_t iic_conf = {0};
    iic_conf.mode = I2C_MODE_MASTER;
    iic_conf.sda_io_num = sda;
    iic_conf.scl_io_num = scl;
    iic_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    iic_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    iic_conf.master.clk_speed = freq;
    
    i2c_param_config(port, &iic_conf);
    
    i2c_driver_install(port, iic_conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
 * @brief       gt9xxx initialization
 * @param       None
 * @retval      0:succeed;1:failed
 */
uint8_t gt9xxx_init(void)
{
    uint8_t temp[5];

    ct_iic_init(GT9XXX_IIC_PORT,GT9XXX_IIC_SDA,GT9XXX_IIC_CLK,400000);

    sys_delays_ms(100);
    gt9xxx_rd_reg(GT9XXX_PID_REG, temp, 4); /* read ID */

    temp[4] = 0;

    if (strcmp((char *)temp, "911") && strcmp((char *)temp, "9147") && strcmp((char *)temp, "1158") && strcmp((char *)temp, "9271"))
    {
        return 1;
    }

    printf("CTP ID:%s\r\n", temp);
    
    if (strcmp((char *)temp, "9271") == 0)      /* ID == 9271 */
    {
        g_gt_tnum = 10;
    }

    temp[0] = 0X02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);    /*Soft reset GT9XXX */
    
    sys_delays_ms(10);
    
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
    static uint8_t t = 0;
    t++;

    if ((t % 10) == 0 || t < 10)                    /* When idle, the CTP_Scan function is only detected once every 10 times, thereby saving CPU usage */
    {
        gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1);  /* Reading the status of touch points */

        if ((mode & 0X80) && ((mode & 0XF) <= g_gt_tnum))
        {
            i = 0;
            gt9xxx_wr_reg(GT9XXX_GSTID_REG, &i, 1); /* Clear flag */
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
                        tp_dev.x[i] = ltdc_self->width - (((uint16_t)buf[3] << 8) + buf[2]);
                        tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                    }
                }
            }

            res = 1;

            if (tp_dev.x[0] > ltdc_self->width || tp_dev.y[0] > ltdc_self->height)
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

    if (t > 240)t = 10;

    return res;
}

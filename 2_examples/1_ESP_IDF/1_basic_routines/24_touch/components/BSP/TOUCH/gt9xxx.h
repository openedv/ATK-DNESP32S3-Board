/**
 ****************************************************************************************************
 * @file        gt9xxx.h
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

#ifndef __GT9XXX_H
#define __GT9XXX_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "touch.h"
#include "string.h"
#include "iic.h"
#include "xl9555.h"


/******************************************************************************************/
/* GT9XXX IO */
#define GT9XXX_INT_GPIO_PIN             GPIO_NUM_40
#define GT9XXX_IIC_PORT                 I2C_NUM_1
#define GT9XXX_IIC_SDA                  GPIO_NUM_39
#define GT9XXX_IIC_CLK                  GPIO_NUM_38
#define GT9XXX_IIC_FREQ                 400000                                  /* IIC FREQ */
#define GT9XXX_INT                      gpio_get_level(GT9XXX_INT_GPIO_PIN)

#define CT_RST(x)       do { x ?                              \
                            xl9555_pin_write(CT_RST_IO, 1):   \
                            xl9555_pin_write(CT_RST_IO, 0);   \
                        } while(0)

/* command */
#define GT9XXX_CMD_WR                   0X28        /* write command */
#define GT9XXX_CMD_RD                   0X29        /* read command */

/* GT9XXX registers  */
#define GT9XXX_CTRL_REG                 0X8040
#define GT9XXX_CFGS_REG                 0X8047
#define GT9XXX_CHECK_REG                0X80FF
#define GT9XXX_PID_REG                  0X8140

#define GT9XXX_GSTID_REG                0X814E
#define GT9XXX_TP1_REG                  0X8150
#define GT9XXX_TP2_REG                  0X8158
#define GT9XXX_TP3_REG                  0X8160
#define GT9XXX_TP4_REG                  0X8168
#define GT9XXX_TP5_REG                  0X8170
#define GT9XXX_TP6_REG                  0X8178
#define GT9XXX_TP7_REG                  0X8180
#define GT9XXX_TP8_REG                  0X8188
#define GT9XXX_TP9_REG                  0X8190
#define GT9XXX_TP10_REG                 0X8198

/* function declaration */
uint8_t gt9xxx_init(void);
uint8_t gt9xxx_scan(uint8_t mode);
esp_err_t gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len);
esp_err_t gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len);

#endif

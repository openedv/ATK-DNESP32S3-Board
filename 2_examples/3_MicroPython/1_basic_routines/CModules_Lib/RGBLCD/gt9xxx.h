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
#include "ltdc.h"
#include "touch.h"
#include "string.h"
#include "driver/i2c.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"


#define I2C_MASTER_TX_BUF_DISABLE       0
#define I2C_MASTER_RX_BUF_DISABLE       0
#define ACK_CHECK_EN                    0x1
#define ACK_CHECK_DIS                   0x0
#define ACK_VAL                         0x0
#define NACK_VAL                        0x1


/******************************************************************************************/
/* GT9XXX IO */
#define GT9XXX_INT_GPIO_PIN             40
#define GT9XXX_IIC_PORT                 1
#define GT9XXX_IIC_SDA                  39
#define GT9XXX_IIC_CLK                  38

/* command */
#define GT9XXX_CMD_WR                   0X28        /* write command */
#define GT9XXX_CMD_RD                   0X29        /* read command */

/* GT9XXX Partial Register Definition  */
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

/* Function declaration */
void gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len);
void gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len); 
uint8_t gt9xxx_init(void);
uint8_t gt9xxx_scan(uint8_t mode);
void sys_delays_ms(int ms);
void sys_delays_us(int us);
#endif

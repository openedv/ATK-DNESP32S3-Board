/**
 ****************************************************************************************************
 * @file        iic.h
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

#ifndef __IIC_H
#define __IIC_H

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"


/* IIC Handle structures */
typedef struct _i2c_obj_t {
    i2c_port_t port;
    gpio_num_t scl;
    gpio_num_t sda;
    esp_err_t init_flag;
} i2c_obj_t;

/* Read and write data structures */
typedef struct _i2c_buf_t {
    size_t len;
    uint8_t *buf;
} i2c_buf_t;

extern i2c_obj_t iic_master[I2C_NUM_MAX];

#define I2C_FLAG_READ                   (0x01)                                                          /* if not set then it's a write */
#define I2C_FLAG_STOP                   (0x02)                                                          /* stop flag */
#define I2C_FLAG_WRITE                  (0x04)                                                          /* If set, the first mp_machine_i2c_buf_t in a transfer is a write */

#define IIC0_SDA_GPIO_PIN               GPIO_NUM_41                                                     /* GPIO number used for I2C0 master data */
#define IIC0_SCL_GPIO_PIN               GPIO_NUM_42                                                     /* GPIO number used for I2C0 master clock */
#define IIC1_SDA_GPIO_PIN               GPIO_NUM_5                                                      /* GPIO number used for I2C1 master data */
#define IIC1_SCL_GPIO_PIN               GPIO_NUM_4                                                      /* GPIO number used for I2C1 master clock */
#define IIC_FREQ                        400000                                                          /* I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE       0                                                               /* I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE       0                                                               /* I2C master doesn't need buffer */
#define ACK_CHECK_EN                    0x1                                                             /* Enable ACK signal */

/* function declaration */
i2c_obj_t iic_init(uint8_t iic_port);
esp_err_t i2c_transfer(i2c_obj_t *self, uint16_t addr, size_t n, i2c_buf_t *bufs, unsigned int flags);

#endif

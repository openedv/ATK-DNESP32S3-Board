/**
 ****************************************************************************************************
 * @file        myiic.h
 * @author      ALIENTEK
 * @brief       MYIIC code
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
 
#ifndef __MYIIC_H__
#define __MYIIC_H__

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/mphal.h"
#include "driver/i2c.h"
#include "esp_err.h"


#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define ACK_CHECK_EN                0x1
#define ACK_CHECK_DIS               0x0
#define ACK_VAL                     0x0
#define NACK_VAL                    0x1

typedef struct _hw_i2c_obj_t {
    mp_obj_base_t base;
    i2c_port_t port : 8;
    gpio_num_t scl : 8;
    gpio_num_t sda : 8;
} hw_i2c_obj_t;

#endif  /*  __MYIIC_H__ */

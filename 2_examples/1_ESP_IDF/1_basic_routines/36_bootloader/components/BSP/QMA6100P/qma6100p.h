/**
 ****************************************************************************************************
 * @file        qma6100p.h
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

#ifndef __QMA6100P_H__
#define __QMA6100P_H__

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "iic.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "qma6100p.h"


#ifdef __cplusplus
extern "C" {
#endif

/* qma6100p device address(7bits) */
#define QMA6100P_ADDR                     0x12

/* QMA6100P registers */
#define QMA6100P_REG_CHIP_ID              0x00  /* This register is used to identify the device */
/* These 6 registers store the 3-axes 14 bits data in two’s complement and the highest bit OUT[13] is a sign bit */
#define QMA6100P_REG_XOUTL                0x01
#define QMA6100P_REG_XOUTH                0x02
#define QMA6100P_REG_YOUTL                0x03
#define QMA6100P_REG_YOUTH                0x04
#define QMA6100P_REG_ZOUTL                0x05
#define QMA6100P_REG_ZOUTH                0x06

#define QMA6100P_REG_BW_ODR               0x10

#define QMA6100P_REG_POWER_MANAGE         0x11

#define QMA6100P_REG_RANGE                0x0f

#define QMA6100P_REG_RESET                0x36

#define QMA6100P_REG_ACC_VAL(lsb, msb)  ((int16_t)(((uint16_t)msb << 8) | ((uint16_t)lsb & 0xFC)) >> 2)

typedef struct {
    uint8_t data[2];
    float  acc_x;
    float  acc_y;
    float  acc_z;
    float  acc_g;
    float  pitch;
    float  roll;
}qma6100p_rawdata_t;

/* function declaration */
void qma6100p_init(i2c_obj_t self);
void qma6100p_read_rawdata(qma6100p_rawdata_t *rawdata);
esp_err_t qma6100p_register_read(const uint8_t reg, uint8_t *data, const size_t len);

typedef enum
{
    QMA6100P_BW_100 = 0,
    QMA6100P_BW_200 = 1,
    QMA6100P_BW_400 = 2,
    QMA6100P_BW_800 = 3,
    QMA6100P_BW_1600 = 4,
    QMA6100P_BW_50 = 5,
    QMA6100P_BW_25 = 6,
    QMA6100P_BW_12_5 = 7,
    QMA6100P_BW_OTHER = 8
}qma6100p_bw;

typedef enum
{
    QMA6100P_RANGE_2G = 0x01,
    QMA6100P_RANGE_4G = 0x02,
    QMA6100P_RANGE_8G = 0x04,
    QMA6100P_RANGE_16G = 0x08,
    QMA6100P_RANGE_32G = 0x0f
}qma6100p_range;

typedef enum
{
    QMA6100P_RESET = 0xB6,
    QMA6100P_RESET_END = 0x00,
}qma6100p_reset;

typedef enum
{
    QMA6100P_MAP_INT1,
    QMA6100P_MAP_INT2,
    QMA6100P_MAP_INT_NONE
}qma6100p_int_map;

typedef enum
{
    QMA6100P_ACTIVE = 0x80,
    QMA6100P_ACTIVE_DIGITAL = 0x84,
    QMA6100P_STANDBY = 0x00,
}qma6100p_power;

typedef enum
{
    QMA6100P_MCLK_102_4K = 0x03,
    QMA6100P_MCLK_51_2K = 0x04,
    QMA6100P_MCLK_25_6K = 0x05,
    QMA6100P_MCLK_12_8K = 0x06,
    QMA6100P_MCLK_6_4K = 0x07,
    QMA6100P_MCLK_RESERVED = 0xff
}qma6100p_mclk;

typedef enum
{
    QMA6100P_SENSITITY_2G = 244,
    QMA6100P_SENSITITY_4G = 488,
    QMA6100P_SENSITITY_8G = 977,
    QMA6100P_SENSITITY_16G = 1950,
    QMA6100P_SENSITITY_32G = 3910
}qma6100p_sensitity;

#ifdef  __cplusplus
}

#endif /*  __cplusplus */
#endif  /*  __QMA6100P_H__ */

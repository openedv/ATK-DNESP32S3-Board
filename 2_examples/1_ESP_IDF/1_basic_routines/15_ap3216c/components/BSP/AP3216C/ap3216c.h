/**
 ****************************************************************************************************
 * @file        ap3216c.h
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
 
#ifndef __AP3216C_H__
#define __AP3216C_H__

#include "xl9555.h"
#include "driver/gpio.h"


typedef struct _ap3216c_value_t
{
    uint16_t ir;
    uint16_t als;
    uint16_t ps;
} ap3216c_value_t;

#define AP3216C_INT     xl9555_pin_read(AP_INT_IO)
#define AP3216C_ADDR    0X1E                                        /* ap3216c device address(7bits) */

/* function declaration */
void ap3216c_init(i2c_obj_t self);
uint8_t ap3216c_comfig(void);
void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als);

#endif

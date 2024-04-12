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

#ifndef __AP3216C_H
#define __AP3216C_H

#include "Arduino.h"

/* pin definition */
#define IIC_SCL         42
#define IIC_SDA         41

#define AP3216C_ADDR    0X1E    /* ap3216c device address(7bits) */

/* function declaration */
uint8_t ap3216c_init(void);                                         
void ap3216c_write_one_byte(uint8_t reg, uint8_t data);             
uint8_t ap3216c_read_one_byte(uint8_t reg);                         
void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als);  

#endif

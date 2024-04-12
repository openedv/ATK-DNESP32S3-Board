/**
 ****************************************************************************************************
 * @file        24c02.h
 * @author      ALIENTEK
 * @brief       24CXX code
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
 
#ifndef __24CXX_H
#define __24CXX_H

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "iic.h"


/* device addr(7bit) */
#define AT_ADDR     (0x50)

#define AT24C01     127
#define AT24C02     255
#define AT24C04     511
#define AT24C08     1023
#define AT24C16     2047
#define AT24C32     4095
#define AT24C64     8191
#define AT24C128    16383
#define AT24C256    32767

#define EE_TYPE     AT24C02

/* function declaration */
void at24cxx_init();
uint8_t at24cxx_check(void);
uint8_t at24cxx_read_one_byte(uint16_t addr);
void at24cxx_write_one_byte(uint16_t addr,uint8_t data);
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen);
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen);

#endif

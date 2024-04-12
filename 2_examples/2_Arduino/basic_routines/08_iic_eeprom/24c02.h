/**
 ****************************************************************************************************
 * @file        24c02.h
 * @author      ALIENTEK
 * @brief       EEPROM code
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

#ifndef __24C02_H
#define __24C02_H

#include "Arduino.h"

/* pin definition */
#define IIC_SCL     42
#define IIC_SDA     41

#define EEPROM_ADDR 0x50    /* device addr(7bit) */

/* function declaration */
void at24c02_init(void);                                            
uint8_t at24c02_check(void);                                       
uint8_t at24c02_read_one_byte(uint8_t addr);                       
void at24c02_write_one_byte(uint8_t addr,uint8_t data);           
void at24c02_write(uint8_t addr, uint8_t *pbuf, uint8_t datalen);  
void at24c02_read(uint8_t addr, uint8_t *pbuf, uint8_t datalen);    

#endif

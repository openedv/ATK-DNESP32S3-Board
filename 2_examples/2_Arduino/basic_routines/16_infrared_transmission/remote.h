/**
 ****************************************************************************************************
 * @file        remote.h
 * @author      ALIENTEK
 * @brief       REMOTE code
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

#ifndef __REMOTE_H
#define __REMOTE_H

#include "Arduino.h"

/* pin definition */
#define RMT_RX_PIN 2

/* function declaration */
void remote_init(void);         
uint8_t remote_scan(void);     

#endif
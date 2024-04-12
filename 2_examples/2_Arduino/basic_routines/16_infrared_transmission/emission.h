/**
 ****************************************************************************************************
 * @file        emission.h
 * @author      ALIENTEK
 * @brief       EMISSION code
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

#ifndef __EMISSION_H
#define __EMISSION_H

#include "Arduino.h"

/* pin definition */
#define RMT_TX_PIN 8
 
/* function declaration */
void emission_init(void);
void emission_send(uint8_t data);

#endif

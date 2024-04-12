/**
 ****************************************************************************************************
 * @file        key.h
 * @author      ALIENTEK
 * @brief       KEY code
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

#ifndef __KEY_H
#define __KEY_H

#include "Arduino.h"

/* pin definition */
#define KEY_PIN       0   

/* key operate definition */
#define KEY           digitalRead(KEY_PIN)

/* function declaration */
void key_init(void);

#endif

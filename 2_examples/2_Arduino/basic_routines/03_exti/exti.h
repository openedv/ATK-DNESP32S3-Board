/**
 ****************************************************************************************************
 * @file        exti.h
 * @author      ALIENTEK
 * @brief       EXTI code
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

#ifndef __EXTI_H
#define __EXTI_H

#include "Arduino.h"

extern uint8_t led_state;

/* pin definition */
#define KEY_INT_PIN     0   

/* function declaration */
void exti_init(void);
void key_isr(void);

#endif

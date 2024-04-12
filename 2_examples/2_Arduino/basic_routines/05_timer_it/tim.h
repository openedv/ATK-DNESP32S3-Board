/**
 ****************************************************************************************************
 * @file        tim.h
 * @author      ALIENTEK
 * @brief       TIMER code
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

#ifndef __TIM_H
#define __TIM_H

#include "Arduino.h"

/* timer interrupt definition */
#define TIMx_INT        0
#define TIMx_ISR        tim0_ISR

/* function declaration */
void timx_int_init(uint16_t arr, uint16_t psc);
void TIMx_ISR(void); 

#endif

/**
 ****************************************************************************************************
 * @file        adc.h
 * @author      ALIENTEK
 * @brief       ADC code
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

#ifndef __ADC_H
#define __ADC_H

#include "Arduino.h"

/* pin definition */
#define ADC_IN_PIN      8        

/* function declaration */
uint16_t adc_get(uint8_t adc_pin);

#endif

/**
 ****************************************************************************************************
 * @file        adc.cpp
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

#include "adc.h"

/**
* @brief       get adc value function
* @param       adc_pin：adc pin
* @retval      adc value
*/
uint16_t adc_get(uint8_t adc_pin) 
{
    return analogRead(adc_pin);
}



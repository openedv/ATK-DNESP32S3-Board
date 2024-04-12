/**
 ****************************************************************************************************
 * @file        adc1.h
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

#ifndef __ADC_H_
#define __ADC_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"


#define ADC_ADCX_CHY   ADC1_CHANNEL_7 

/* function declaration */
void adc_init(void);
uint32_t adc_get_result_average(uint32_t ch, uint32_t times);

#endif

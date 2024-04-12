/**
 ****************************************************************************************************
 * @file        adc1.c
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

#include "adc1.h"

/**
 * @brief       ADC initialization
 * @param       None
 * @retval      None
 */
void adc_init(void)
{
    adc_digi_pattern_config_t adc1_digi_pattern_config;         /* List of configs for each ADC channel that will be used */
    adc_digi_configuration_t adc1_init_config;                  /* ADC digital controller settings */
    
    adc1_digi_pattern_config.atten = ADC_ATTEN_DB_11;           /* Attenuation of this ADC channel */
    adc1_digi_pattern_config.channel = ADC_ADCX_CHY;            /* ADC channel */
    adc1_digi_pattern_config.unit = ADC_UNIT_1;                 /* ADC unit */
    adc1_digi_pattern_config.bit_width = ADC_BITWIDTH_12;       /* ADC output bit width */
    adc1_init_config.adc_pattern = &adc1_digi_pattern_config;
    adc_digi_controller_configure(&adc1_init_config);           /* Setting the digital controller */
}

/**
 * @brief       Obtain the results of ADC conversion and mean filtering
 * @param       ch      : Channel number
 * @param       times   : number
 * @retval      average value
 */
uint32_t adc_get_result_average(uint32_t ch, uint32_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)
    {
        temp_val += adc1_get_raw(ch);
        vTaskDelay(5);
    }

    return temp_val / times;
}

/**
 ****************************************************************************************************
 * @file        sensor.c
 * @author      ALIENTEK
 * @brief       SENSOR code
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

#include "sensor.h"


esp_err_t rev_flag;
temperature_sensor_handle_t temp_handle = NULL;

/**
 * @brief       Initialize the temperature sensor
 * @param       None
 * @retval      None
 */
void temperature_sensor_init(void)
{
    /* Install temperature sensor, expected temp ranger range: 10~50 ℃ */
    temperature_sensor_config_t temp_sensor;
    
    temp_sensor.range_min = SENSOR_RANGE_MIN;
    temp_sensor.range_max = SENSOR_RANGE_MAX;

    rev_flag |= temperature_sensor_install(&temp_sensor, &temp_handle);
    ESP_ERROR_CHECK(rev_flag);
}

/**
 * @brief       get temperature
 * @param       None
 * @retval      temperature value
 */
float sensor_get_temperature(void)
{
    float temp;

    /* Enable the temperature sensor */
    rev_flag |= temperature_sensor_enable(temp_handle);

    /* Read temperature sensor data that is converted to degrees Celsius */ 
    rev_flag |= temperature_sensor_get_celsius(temp_handle, &temp);

    /* Disable temperature sensor */
    rev_flag |= temperature_sensor_disable(temp_handle);
    ESP_ERROR_CHECK(rev_flag);

    return temp;
}

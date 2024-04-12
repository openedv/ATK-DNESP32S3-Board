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

#define __SENSOR_VERSION__  "1.0"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sensor.h"


float sensor_value;
temperature_sensor_handle_t temp_sensor = NULL;

/**
 * @brief       read sensor
 * @param       None
 * @retval      None
 */
STATIC mp_obj_t internal_sensor_read(void)
{
    if (temp_sensor != NULL)
    {
        ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &sensor_value));
        return mp_obj_new_float(sensor_value);
    }
    else
    {
        mp_raise_ValueError(MP_ERROR_TEXT("sensor init ??......"));
        return 0;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sensor_read_obj, internal_sensor_read);

/**
 * @brief       sensor init
 * @param       None
 * @retval      None
 */
STATIC mp_obj_t sensor_init(void)
{
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(init_obj, sensor_init);


STATIC const mp_map_elem_t sensor_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_atk_sensor) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&init_obj) },
    { MP_ROM_QSTR(MP_QSTR_sensor_read), MP_ROM_PTR(&sensor_read_obj) },
};

STATIC MP_DEFINE_CONST_DICT (mp_module_sensor_globals, sensor_module_globals_table );

const mp_obj_module_t mp_module_sensor = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_sensor_globals,
};

MP_REGISTER_MODULE(MP_QSTR_atk_sensor, mp_module_sensor);

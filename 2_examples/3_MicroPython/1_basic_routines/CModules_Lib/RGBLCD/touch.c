/**
 ****************************************************************************************************
 * @file        touch.c
 * @author      ALIENTEK
 * @brief       TOUCH code
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

#include "touch.h"

_m_tp_dev tp_dev =
{{
    tp_init,
    0,
    0,
    0,
    0,
    0,
}};

/**
 * @brief       tp init
 * @param       None
 * @retval      0:Not calibrated
 *              1:calibrated
 */
uint8_t tp_init(void)
{
    tp_dev.touchtype = 0;                                   /* default setting*/
    tp_dev.touchtype |= ltdc_self->dir & 0X01;              /* Determine whether it is landscape or vertical based on LCD */

    if (ltdc_self->id == 0X4342 || ltdc_self->id == 0X4384)
    {
        gt9xxx_init();
        tp_dev.scan = gt9xxx_scan;                          /* Scan function points to GT9147 touch screen scan */
        tp_dev.touchtype |= 0X80;
        return 0;
    }

    return 1;
}

/**
 * @brief       Obtain X-axis coordinates
 * @param       None
 * @retval      None
 */
STATIC mp_obj_t mp_touch_get_x(void)
{
    return mp_obj_new_bytearray_by_ref(sizeof(uint16_t) * MP_ARRAY_SIZE(tp_dev.x), tp_dev.x);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(touch_get_x_obj, mp_touch_get_x);

/**
 * @brief       Obtain Y-axis coordinates
 * @param       None
 * @retval      None
 */
STATIC mp_obj_t mp_touch_get_y(void)
{
    return mp_obj_new_bytearray_by_ref(sizeof(uint16_t) * MP_ARRAY_SIZE(tp_dev.y), tp_dev.y);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(touch_get_y_obj, mp_touch_get_y);

uint16_t sta[2];

/**
 * @brief       Get touch status
 * @param       None
 * @retval      None
 */
STATIC mp_obj_t mp_touch_get_sta(void)
{
    sta[0] = tp_dev.sta;
    
    return mp_obj_new_bytearray_by_ref(sizeof(uint16_t) * MP_ARRAY_SIZE(sta), sta);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(touch_get_sta_obj, mp_touch_get_sta);

STATIC mp_obj_t mp_touch_scan(mp_obj_t mode)
{
    int ct_mode = mp_obj_get_int(mode);
    tp_dev.scan(ct_mode);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(touch_scan_obj, mp_touch_scan);

STATIC mp_obj_t mp_touch_init(void)
{
    tp_dev.init();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(touch_init_obj, mp_touch_init);

STATIC const mp_map_elem_t touch_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_atk_touch) },
    { MP_ROM_QSTR(MP_QSTR_init), (mp_obj_t)&touch_init_obj },
    { MP_ROM_QSTR(MP_QSTR_scan), (mp_obj_t)&touch_scan_obj },
    { MP_ROM_QSTR(MP_QSTR_get_x), (mp_obj_t)&touch_get_x_obj },
    { MP_ROM_QSTR(MP_QSTR_get_y), (mp_obj_t)&touch_get_y_obj },
    { MP_ROM_QSTR(MP_QSTR_get_sta), (mp_obj_t)&touch_get_sta_obj },
    
};

STATIC MP_DEFINE_CONST_DICT (mp_module_touch_globals, touch_module_globals_table );

const mp_obj_module_t mp_module_touch = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_touch_globals,
};

MP_REGISTER_MODULE(MP_QSTR_atk_touch, mp_module_touch);
/**
 ****************************************************************************************************
 * @file        esp_config.c
 * @author      ALIENTEK
 * @brief       esp_config code
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

#include "esp_config.h"

QueueHandle_t xQueueAIFrameO;

int ai_flag;

STATIC mp_obj_t mp_py_ai_strat(mp_obj_t flag)
{
    ai_flag = mp_obj_get_int(flag);
    
    if (ai_flag == FACE_DETECTION)
    {
        esp_face_detection_ai_strat();
    }
    else if (ai_flag == FACE_RECONGNITION)
    {
        esp_face_recognition_ai_strat();
    }
    else if (ai_flag == QR_RECONGNITION)
    {
        esp_qr_detection_ai_strat();
    }
    else if (ai_flag == COLOR_RECONGNITION)
    {
        esp_color_detection_ai_strat();
    }
    else if (ai_flag == CAT_DETECTION)
    {
        esp_cat_face_detection_ai_strat();
    }
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ai_strat, mp_py_ai_strat);

STATIC mp_obj_t mp_py_ai_deinit(mp_obj_t flag)
{
    if (ai_flag == FACE_DETECTION)
    {
        esp_face_detection_ai_deinit();
    }
    else if (ai_flag == FACE_RECONGNITION)
    {
        esp_face_recognition_ai_deinit();
    }
    else if (ai_flag == QR_RECONGNITION)
    {
        esp_qr_detection_ai_deinit();
    }
    else if (ai_flag == COLOR_RECONGNITION)
    {
        esp_color_detection_ai_deinit();
    }
    else if (ai_flag == CAT_DETECTION)
    {
        esp_cat_face_detection_ai_deinit();
    }
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ai_deinit, mp_py_ai_deinit);

STATIC const mp_rom_map_elem_t mp_module_ai_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_esp_who) },
    { MP_ROM_QSTR(MP_QSTR_ai_detection_strat), MP_ROM_PTR(&ai_strat) },
    { MP_ROM_QSTR(MP_QSTR_ai_detection_deinit), MP_ROM_PTR(&ai_deinit) },
    { MP_ROM_QSTR(MP_QSTR_FACE_DETECTION), MP_ROM_INT(FACE_DETECTION) },
    { MP_ROM_QSTR(MP_QSTR_FACE_RECONGNITION), MP_ROM_INT(FACE_RECONGNITION) },
    { MP_ROM_QSTR(MP_QSTR_QR_RECONGNITION), MP_ROM_INT(QR_RECONGNITION) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_RECONGNITION), MP_ROM_INT(COLOR_RECONGNITION) },
    { MP_ROM_QSTR(MP_QSTR_CAT_DETECTION), MP_ROM_INT(CAT_DETECTION) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_ai_globals, mp_module_ai_globals_table);


const mp_obj_module_t mp_module_ai = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_ai_globals,
};

MP_REGISTER_MODULE(MP_QSTR_esp_who, mp_module_ai);
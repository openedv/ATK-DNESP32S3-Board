/**
 ****************************************************************************************************
 * @file        qma6100p.c
 * @author      ALIENTEK
 * @brief       QMA6100P code
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

#define __IIC_VERSION__  "1.0"

#include "qma6100p.h"


mp_obj_t qma6100p_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args );

typedef struct _qma6100p_obj_t
{
    mp_obj_base_t   base;
    mp_obj_base_t   *iic_obj;
} qma6100p_obj_t;

qma6100p_obj_t *qma6100p_self = NULL;

/**
 * @brief       read bytes data from qma6100p
 * @param       reg   : register address
 * @param       data  : buffer
 * @param       len   : length
 * @retval      ESP_OK: None
 */
esp_err_t qma6100p_register_read(const uint8_t reg, uint8_t *data, const size_t len)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0]  = reg;
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(qma6100p_self->iic_obj);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);

    mp_machine_i2c_buf_t bufs[2] = {
        {.len = 1, .buf = memaddr_buf},
        {.len = len, .buf = data},
    };

    i2c_p->transfer(self, QMA6100P_ADDR, 2, bufs,MP_MACHINE_I2C_FLAG_WRITE1 | MP_MACHINE_I2C_FLAG_READ | MP_MACHINE_I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief       write data to qma6100p register
 * @param       reg   : register address
 * @param       data  : write data
 * @retval      ESP_OK: None
 */
static esp_err_t qma6100p_register_write_byte(uint8_t reg, uint8_t data)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0]  = reg;
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(qma6100p_self->iic_obj);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);

    mp_machine_i2c_buf_t bufs[2] = {
        {.len = 1, .buf = memaddr_buf},
        {.len = 1, .buf = &data},
    };

    i2c_p->transfer(self, QMA6100P_ADDR, 2, bufs,MP_MACHINE_I2C_FLAG_STOP);
    return ESP_OK;
}

uint8_t xyz_data[6] = {0};
short raw_data[3] = {0};
float accl_data[3];
float acc_normal;

/**
 * @brief       Read 3-axis data (raw data, acceleration, pitch angle, and roll angle)
 * @param       rawdata：qma6100p Data structure
 * @retval      None
 */
void qma6100p_read_rawdata(qma6100p_rawdata_t *rawdata)
{
    qma6100p_register_read(QMA6100P_REG_XOUTL, xyz_data, 6);
    
    raw_data[0] = (short)(((xyz_data[1] << 8)) | (xyz_data[0]));
    raw_data[1] = (short)(((xyz_data[3] << 8)) | (xyz_data[2]));
    raw_data[2] = (short)(((xyz_data[5] << 8)) | (xyz_data[4]));
    
    rawdata->acc_x = (float)((raw_data[0] >> 2) * M_G) / (1024);
    rawdata->acc_y = (float)((raw_data[1] >> 2) * M_G) / (1024);
    rawdata->acc_z = (float)((raw_data[2] >> 2) * M_G) / (1024);
    
    rawdata->acc_g = sqrt(rawdata->acc_x*rawdata->acc_x + rawdata->acc_y * rawdata->acc_y + rawdata->acc_z*rawdata->acc_z);
    
    acc_normal = sqrtf(rawdata->acc_x * rawdata->acc_x + rawdata->acc_y * rawdata->acc_y + rawdata->acc_z * rawdata->acc_z);
    accl_data[0] = rawdata->acc_x / acc_normal;
    accl_data[1] = rawdata->acc_y / acc_normal;
    accl_data[2] = rawdata->acc_z / acc_normal;
    
    rawdata->pitch = -atan2f(rawdata->acc_x,rawdata->acc_z) * RAD_TO_DEG;
    
    acc_normal = sqrtf(accl_data[0] * accl_data[0] + accl_data[1] * accl_data[1] + accl_data[2] * accl_data[2]);
    
    rawdata->roll = asinf((accl_data[1]/acc_normal)) * RAD_TO_DEG ;
}

qma6100p_rawdata_t xyz_rawdata;

STATIC mp_obj_t mp_qma6100p_read(size_t n_args, const mp_obj_t *args)
{
    qma6100p_obj_t *self    = MP_OBJ_TO_PTR(args[0]);
    
    qma6100p_read_rawdata(&xyz_rawdata);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qma6100p_read_obj, 1, 1, mp_qma6100p_read);

STATIC mp_obj_t mp_qma6100p_acc_x(size_t n_args, const mp_obj_t *args)
{
    qma6100p_obj_t *self    = MP_OBJ_TO_PTR(args[0]);
    
    return mp_obj_new_float(xyz_rawdata.acc_x);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qma6100p_acc_x_obj, 1, 1, mp_qma6100p_acc_x);

STATIC mp_obj_t mp_qma6100p_acc_y(size_t n_args, const mp_obj_t *args)
{
    qma6100p_obj_t *self    = MP_OBJ_TO_PTR(args[0]);
    
    return mp_obj_new_float(xyz_rawdata.acc_y);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qma6100p_acc_y_obj, 1, 1, mp_qma6100p_acc_y);

STATIC mp_obj_t mp_qma6100p_acc_z(size_t n_args, const mp_obj_t *args)
{
    qma6100p_obj_t *self    = MP_OBJ_TO_PTR(args[0]);
    
    return mp_obj_new_float(xyz_rawdata.acc_z);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qma6100p_acc_z_obj, 1, 1, mp_qma6100p_acc_z);

STATIC mp_obj_t mp_qma6100p_acc_g(size_t n_args, const mp_obj_t *args)
{
    qma6100p_obj_t *self    = MP_OBJ_TO_PTR(args[0]);
    
    return mp_obj_new_float(xyz_rawdata.acc_g);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qma6100p_acc_g_obj, 1, 1, mp_qma6100p_acc_g);

STATIC mp_obj_t mp_qma6100p_acc_pitch(size_t n_args, const mp_obj_t *args)
{
    qma6100p_obj_t *self    = MP_OBJ_TO_PTR(args[0]);
    
    return mp_obj_new_float(xyz_rawdata.pitch);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qma6100p_acc_pitch_obj, 1, 1, mp_qma6100p_acc_pitch);

STATIC mp_obj_t mp_qma6100p_acc_roll(size_t n_args, const mp_obj_t *args)
{
    qma6100p_obj_t *self    = MP_OBJ_TO_PTR(args[0]);
    
    return mp_obj_new_float(xyz_rawdata.roll);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qma6100p_acc_roll_obj, 1, 1, mp_qma6100p_acc_roll);

/**
 * @brief       Configure Interrupt
 * @param       int_map :Interrupt mode
 * @param       enable  :Enable
 * @retval      None
 */
void qma6100p_step_int_config(int int_map, int enable)
{
    uint8_t	reg_16 = 0;
    uint8_t	reg_19 = 0;
    uint8_t	reg_1b = 0;

    qma6100p_register_read(0x16, &reg_16, 1);
    qma6100p_register_read(0x19, &reg_19, 1);
    qma6100p_register_read(0x1b, &reg_1b, 1);
    
    if (enable)
    {
        reg_16 |= 0x08;
        reg_19 |= 0x08;
        reg_1b |= 0x08;
        qma6100p_register_write_byte(0x16, reg_16);
        
        if(int_map == QMA6100P_MAP_INT1)
            qma6100p_register_write_byte(0x19, reg_19);
        else if(int_map == QMA6100P_MAP_INT2)
            qma6100p_register_write_byte(0x1b, reg_1b);
    }
    else
    {
        reg_16 &= (~0x08);
        reg_19 &= (~0x08);
        reg_1b &= (~0x08);

        qma6100p_register_write_byte(0x16, reg_16);
        qma6100p_register_write_byte(0x19, reg_19);
        qma6100p_register_write_byte(0x1b, reg_1b);
    }
}

/**
* @brief       configure qma6100p
* @param       None
* @retval      0:succeed; !0:failed
*/
uint8_t qma6100p_comfig(void)
{
    static uint8_t id_data[2];
    qma6100p_register_read(QMA6100P_REG_CHIP_ID,id_data,1); /* read qma6100p ID */
    
    /* software reset */
    qma6100p_register_write_byte(QMA6100P_REG_RESET,QMA6100P_RESET);
    mp_hal_delay_ms(5);
    qma6100p_register_write_byte(QMA6100P_REG_RESET,QMA6100P_RESET_END);
    mp_hal_delay_ms(10);
    
    qma6100p_register_read(QMA6100P_REG_CHIP_ID,id_data,1); /* read qma6100p ID */
    
    qma6100p_register_write_byte(0x11, 0x80);
    qma6100p_register_write_byte(0x11, 0x84);
    qma6100p_register_write_byte(0x4a, 0x20);
    qma6100p_register_write_byte(0x56, 0x01);
    qma6100p_register_write_byte(0x5f, 0x80);
    mp_hal_delay_ms(1);
    qma6100p_register_write_byte(0x5f, 0x00);
    mp_hal_delay_ms(10);

    qma6100p_register_write_byte(QMA6100P_REG_RANGE,QMA6100P_RANGE_8G);
    qma6100p_register_write_byte(QMA6100P_REG_BW_ODR,QMA6100P_BW_100);
    qma6100p_register_write_byte(QMA6100P_REG_POWER_MANAGE,QMA6100P_MCLK_51_2K | 0x80);

    qma6100p_register_write_byte(0x21, 0x03);   /* default 0x1c, step latch mode */
    
    qma6100p_step_int_config(QMA6100P_MAP_INT1,1);
    
    if (id_data[0] == 0x90)
    {
        ESP_LOGE("qma6100p", "qma6100p success!!!");
        return 0;
    }
    else
    {
        ESP_LOGE("qma6100p", "qma6100p fail!!!");
        return 1;
    }

}

/**
* @brief       qma6100p init
* @param       None
* @retval      None
*/
void qma6100p_init(void)
{    
    while (qma6100p_comfig())
    {
        ESP_LOGE("qma6100p", "qma6100p init fail!!!");
        mp_hal_delay_ms(500);
    }
}

STATIC const mp_rom_map_elem_t qma6100p_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_qma6100p_read),       MP_ROM_PTR(&qma6100p_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_qma6100p_acc_x),      MP_ROM_PTR(&qma6100p_acc_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_qma6100p_acc_y),      MP_ROM_PTR(&qma6100p_acc_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_qma6100p_acc_z),      MP_ROM_PTR(&qma6100p_acc_z_obj) },
    { MP_ROM_QSTR(MP_QSTR_qma6100p_acc_g),      MP_ROM_PTR(&qma6100p_acc_g_obj) },
    { MP_ROM_QSTR(MP_QSTR_qma6100p_acc_pitch),  MP_ROM_PTR(&qma6100p_acc_pitch_obj) },
    { MP_ROM_QSTR(MP_QSTR_qma6100p_acc_roll),   MP_ROM_PTR(&qma6100p_acc_roll_obj) },
};

STATIC MP_DEFINE_CONST_DICT(qma6100p_locals_dict, qma6100p_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    qma6100p_type,
    MP_QSTR_init,
    MP_TYPE_FLAG_NONE,
    make_new, qma6100p_make_new,
    locals_dict, &qma6100p_locals_dict
);

mp_obj_t qma6100p_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args )
{
    enum
    {
        ARG_iic,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_iic, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    qma6100p_self = m_new_obj(qma6100p_obj_t);
    qma6100p_self->base.type = &qma6100p_type;

    mp_obj_base_t *qma6100p_obj   = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[ARG_iic].u_obj);
    qma6100p_self->iic_obj        = qma6100p_obj;

    qma6100p_init();
    
    return MP_OBJ_FROM_PTR(qma6100p_self);
}

STATIC const mp_map_elem_t qma6100p_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_atk_qma6100p) },
    { MP_ROM_QSTR(MP_QSTR_init), (mp_obj_t)&qma6100p_type },
};

STATIC MP_DEFINE_CONST_DICT (mp_module_qma6100p_globals, qma6100p_module_globals_table );

const mp_obj_module_t mp_module_qma6100p = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_qma6100p_globals,
};

MP_REGISTER_MODULE(MP_QSTR_atk_qma6100p, mp_module_qma6100p);

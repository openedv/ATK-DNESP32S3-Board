/**
 ****************************************************************************************************
 * @file        24c02.c
 * @author      ALIENTEK
 * @brief       EEPROM code
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

#include "esp_log.h"
#include "myiic.h"
#include "24cxx.h"
#include <stdlib.h>
#include <string.h>


mp_obj_t at24cx_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args );


typedef struct _at24cx_obj_t
{
    mp_obj_base_t       base;
    mp_obj_base_t       *iic_obj;
} at24cx_obj_t;

at24cx_obj_t *at24cx_self = NULL;

/**
 * @brief       read data from 24c02
 * @param       addr: read data address
 * @retval      data / 0xFF:no data
 */
static uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    esp_err_t ret = 0;
    uint8_t data = 0;
    hw_i2c_obj_t *self = MP_OBJ_TO_PTR(at24cx_self->iic_obj);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
 
    if (EE_TYPE > AT24C16)
    {
        i2c_master_write_byte(cmd, (AT_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);    /* i2c address of device */
        i2c_master_write_byte(cmd, addr >> 8, ACK_CHECK_EN);
    }
    else
    {
        i2c_master_write_byte(cmd, 0XA0 + ((addr / 256) << 1), ACK_CHECK_EN);           /* i2c address of device */
    }
    
    i2c_master_write_byte(cmd, addr % 256, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AT_ADDR << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &data, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(self->port, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    mp_hal_delay_ms(10);
    
    return data;
}

/**
 * @brief       write data to 24c02
 * @param       addr: write data address
 * @param       data: write data 
 * @retval      None
 */
static void at24cxx_write_one_byte(uint16_t addr, uint8_t data)
{
    esp_err_t ret = 0;
    hw_i2c_obj_t *self = MP_OBJ_TO_PTR(at24cx_self->iic_obj);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    
    if (EE_TYPE > AT24C16)
    {
        i2c_master_write_byte(cmd, (AT_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);    /* i2c address of device */
        i2c_master_write_byte(cmd, addr >> 8, ACK_CHECK_EN);
    }
    else
    {
        i2c_master_write_byte(cmd, 0XA0 + ((addr / 256) << 1), ACK_CHECK_EN);           /* i2c address of device */
    }
    i2c_master_write_byte(cmd, addr % 256, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(self->port, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    
    mp_hal_delay_ms(10);
}

/**
 * @brief       check AT24C02
 *   @note      Detection principle: Write the value 0X55 at the end address of the device, 
 *              and then read the value. If the read value is 0X55, the detection is normal. 
 *              Otherwise, the detection fails.
 * @param       None
 * @retval      0: succeed, 1:failed
 */
uint8_t at24cxx_check(void)
{
    uint8_t temp;
    uint16_t addr = EE_TYPE;
    temp = at24cxx_read_one_byte(addr);     /* read data from addr 255 */

    if (temp == 0X55)                       /* data is correct */
    {
        return 0;
    }
    else                                    /* first time */
    {
        at24cxx_write_one_byte(addr, 0X55); /* write data first */
        temp = at24cxx_read_one_byte(255);  /* then read data */

        if (temp == 0X55)return 0;
    }

    return 1;
}

STATIC mp_obj_t mp_at24cxx_check(size_t n_args, const mp_obj_t *args)
{
    at24cx_obj_t *self  = MP_OBJ_TO_PTR(args[0]);
    return mp_obj_new_int(at24cxx_check());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(at24cxx_check_obj, 1, 1, mp_at24cxx_check);

/**
 * @brief       read bytes data from 24c02
 * @param       addr: read data address
 * @param       pbuf: data buf
 * @param       datalen: read data length 
 * @retval      None
 */
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        *pbuf++ = at24cxx_read_one_byte(addr++);
    }
}

uint8_t *p_pbuf;

STATIC mp_obj_t mp_at24cxx_read(size_t n_args, const mp_obj_t *args)
{
    at24cx_obj_t *self  = MP_OBJ_TO_PTR(args[0]);
    uint16_t addr       = mp_obj_get_int(args[1]);
    uint16_t datalen    = mp_obj_get_int(args[2]);

    if (p_pbuf == NULL)
    {
        p_pbuf = m_malloc(datalen);
    }

    at24cxx_read(addr,p_pbuf,datalen);

    return mp_obj_new_str((const char *)p_pbuf, datalen);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(at24cxx_read_obj, 3, 3, mp_at24cxx_read);

/**
 * @brief       write bytes data to 24c02
 * @param       addr: write data address
 * @param       pbuf: data buf
 * @param       datalen: write data length 
 * @retval      None
 */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        at24cxx_write_one_byte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}
STATIC mp_obj_t mp_at24cxx_write(size_t n_args, const mp_obj_t *args)
{
    at24cx_obj_t *self  = MP_OBJ_TO_PTR(args[0]);
    uint16_t addr       = mp_obj_get_int(args[1]);
    const char *str     = mp_obj_str_get_str(args[2]);
    uint16_t datalen    = mp_obj_get_int(args[3]);

    at24cxx_write(addr,str,datalen);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(at24cxx_write_obj, 4, 4, mp_at24cxx_write);

STATIC const mp_map_elem_t at24cx_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_at24cxx_check),   MP_ROM_PTR(&at24cxx_check_obj) },
    { MP_ROM_QSTR(MP_QSTR_at24cxx_write),   MP_ROM_PTR(&at24cxx_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_at24cxx_read),    MP_ROM_PTR(&at24cxx_read_obj) },
};

STATIC MP_DEFINE_CONST_DICT (at24cx_locals_dict, at24cx_locals_dict_table );


MP_DEFINE_CONST_OBJ_TYPE(
    at24cx_type,
    MP_QSTR_init,
    MP_TYPE_FLAG_NONE,
    make_new, at24cx_make_new,
    locals_dict, &at24cx_locals_dict
);

mp_obj_t at24cx_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args )
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


    at24cx_self = m_new_obj(at24cx_obj_t);
    at24cx_self->base.type = &at24cx_type;

    mp_obj_base_t *at24cx_obj   = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[ARG_iic].u_obj);
    at24cx_self->iic_obj        = at24cx_obj;

    return MP_OBJ_FROM_PTR(at24cx_self);
}

STATIC const mp_map_elem_t at24cx_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_atk_at24cx) },
    { MP_ROM_QSTR(MP_QSTR_init), (mp_obj_t)&at24cx_type },
};

STATIC MP_DEFINE_CONST_DICT (mp_module_at24cx_globals, at24cx_module_globals_table );

const mp_obj_module_t mp_module_at24cx = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_at24cx_globals,
};

MP_REGISTER_MODULE(MP_QSTR_atk_at24cx, mp_module_at24cx);

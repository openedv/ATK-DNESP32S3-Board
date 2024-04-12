/**
 ****************************************************************************************************
 * @file        ap3216c.c
 * @author      ALIENTEK
 * @brief       AP3216C code
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

#include "ap3216c.h"


mp_obj_t ap3216c_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args );

typedef struct _ap3216c_obj_t
{
    mp_obj_base_t       base;
    mp_obj_base_t       *iic_obj;
} ap3216c_obj_t;

typedef struct _ap3216c_value_t
{
    uint16_t ir;
    uint16_t als;
    uint16_t ps;
} ap3216c_value_t;

ap3216c_obj_t *ap3216c_self = NULL;

/**
 * @brief       read data from ap3216c register
 * @param       reg: register
 * @retval      ESP_OK:none
 */
static esp_err_t ap3216c_read_one_byte(uint8_t* data,uint8_t reg)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0]  = reg;
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(ap3216c_self->iic_obj);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);

    mp_machine_i2c_buf_t bufs[2] = {
        {.len = 1, .buf = memaddr_buf},
        {.len = 1, .buf = data},
    };

    i2c_p->transfer(self, AP3216C_ADDR, 2, bufs,MP_MACHINE_I2C_FLAG_WRITE1 | MP_MACHINE_I2C_FLAG_READ | MP_MACHINE_I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief       write data to ap3216c register
 * @param       reg: register address
 * @param       data: write data 
 * @retval      ESP_OK:none
 */
static esp_err_t ap3216c_write_one_byte(uint8_t reg,uint8_t data)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0]  = reg;
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(ap3216c_self->iic_obj);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);

    mp_machine_i2c_buf_t bufs[2] = {
        {.len = 1, .buf = memaddr_buf},
        {.len = 1, .buf = &data},
    };

    i2c_p->transfer(self, AP3216C_ADDR, 2, bufs, MP_MACHINE_I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief       configure ap3216c 
 * @param       None
 * @retval      0:succeed;1:failed
 */
uint8_t ap3216c_comfig(void)
{
    uint8_t temp;
    esp_err_t err;
    int retry = 3;
    
    ap3216c_write_one_byte(0x00, 0X04); /* ap3216c reset */
    mp_hal_delay_ms(50);                /* at least 10ms */
    ap3216c_write_one_byte(0x00, 0X03); /* enable ALS and PS+IR */
    ap3216c_read_one_byte(&temp,0X00);  /* verify that the configuration is successful */

    if (temp == 0X03)
    {
        printf("AP3216C success!!!");
        return 0;                       /* succeed */
    }
    else
    {
        printf("AP3216C fail!!!");
        return 1;                       /* failed */
    }
} 

/**
 * @brief       read AP3216C data(ALS,PS+IR)
 * @note        The data read interval must be greater than 112.5ms
 * @param       ir      : IR data
 * @param       ps      : PS data
 * @param       als     : ALS data
 * @retval      None
 */
void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als)
{
    uint8_t buf[6] = {0};
    uint8_t i;

    for (i = 0; i < 6; i++)
    {
        ap3216c_read_one_byte(&buf[i],0X0A + i);           /* reads all sensor data */
    }

    if (buf[0] & 0X80)
    {
        *ir = 0;                                            /* IR_OF bit is 1,the data is invalid */
    }
    else 
    {
        *ir = ((uint16_t)buf[1] << 2) | (buf[0] & 0X03);    /* read IR sensor data */
    }

    *als = ((uint16_t)buf[3] << 8) | buf[2];                /* read ALS sensor data */

    if (buf[4] & 0x40) 
    {
        *ps = 0;                                            /* IR_OF bit is 1,the data is invalid */
    }
    else
    {
        *ps = ((uint16_t)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F);  /* read PS sensor data */
    }
}

uint16_t pbuf[3] = {0,0,0};
uint16_t *ptr = NULL;

STATIC mp_obj_t mp_ap3216c_read(size_t n_args, const mp_obj_t *args)
{
    ap3216c_obj_t *self 	= MP_OBJ_TO_PTR(args[0]);
    
    if (ptr != NULL)
    {
        m_free(ptr);
    }
    
    ptr = m_malloc(sizeof(uint16_t) * (MP_ARRAY_SIZE(pbuf)));
    
    ap3216c_read_data(&pbuf[0],&pbuf[1],&pbuf[2]);
    
    for (int i = 0; i < MP_ARRAY_SIZE(pbuf); i++)
    {
        ptr[i] = pbuf[i];
    }
    
    return mp_obj_new_bytearray_by_ref(sizeof(uint16_t) * MP_ARRAY_SIZE(pbuf), ptr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ap3216c_read_obj, 1, 1, mp_ap3216c_read);

/**
 * @brief       ap3216c init
 * @param       None
 * @retval      None
 */
void ap3216c_init(void)
{    
    while (ap3216c_comfig())
    {
        ESP_LOGE("ap3216c", "ap3216c init fail!!!");
        mp_hal_delay_ms(500);
    }
}

STATIC const mp_rom_map_elem_t ap3216c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_ap3216c_read),    MP_ROM_PTR(&ap3216c_read_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ap3216c_locals_dict, ap3216c_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    ap3216c_type,
    MP_QSTR_init,
    MP_TYPE_FLAG_NONE,
    make_new, ap3216c_make_new,
    locals_dict, &ap3216c_locals_dict
);

mp_obj_t ap3216c_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args )
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

    ap3216c_self = m_new_obj(ap3216c_obj_t);
    ap3216c_self->base.type = &ap3216c_type;
    mp_obj_base_t *ap3216c_obj   = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[ARG_iic].u_obj);

    if (ap3216c_obj == MP_OBJ_NULL)
    {
        mp_raise_ValueError(MP_ERROR_TEXT("I2C init ???"));
    }

    ap3216c_self->iic_obj        = ap3216c_obj;

    ap3216c_init();
    
    return MP_OBJ_FROM_PTR(ap3216c_self);
}

STATIC const mp_map_elem_t ap3216c_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_atk_ap3216) },
    { MP_ROM_QSTR(MP_QSTR_init), (mp_obj_t)&ap3216c_type },
};

STATIC MP_DEFINE_CONST_DICT (mp_module_ap3216c_globals, ap3216c_module_globals_table );

const mp_obj_module_t mp_module_ap3216c = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ap3216c_globals,
};

MP_REGISTER_MODULE(MP_QSTR_atk_ap3216, mp_module_ap3216c);

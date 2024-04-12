/**
 ****************************************************************************************************
 * @file        xl9555.c
 * @author      ALIENTEK
 * @brief       XL9555 code
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

#include "xl9555.h"


static uint16_t xl9555_value = 0x0000;
static uint16_t xl9555_readonly_value = 0x0000;
static uint16_t xl9555_failed = 0;

mp_obj_t xl9555_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args );

typedef struct _xl9555_obj_t
{
    mp_obj_base_t       base;
    mp_obj_base_t       *iic_obj;
} xl9555_obj_t;

xl9555_obj_t *xl9555_self = NULL;


/**
 * @brief       read data from xl9555 register
 * @param       data   : data
 * @param       len    : lenght
 * @retval      ESP_OK :Note
 */
esp_err_t xl9555_read_byte(uint8_t* data, size_t len)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0]  = XL9555_INPUT_PORT0_REG;
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(xl9555_self->iic_obj);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);

    mp_machine_i2c_buf_t bufs[2] = {
        {.len = 1, .buf = memaddr_buf},
        {.len = len, .buf = data},
    };

    i2c_p->transfer(self, XL9555_ADDR, 2, bufs,MP_MACHINE_I2C_FLAG_WRITE1 | MP_MACHINE_I2C_FLAG_READ | MP_MACHINE_I2C_FLAG_STOP);
    return ESP_OK;
}

 */
/**
 * @brief      write data to xl9555 register
 * @param      reg    : xl9555 register address
 * @param      data   : write to xl9555 register data
 * @retval     ESP_OK:None
 */
esp_err_t xl9555_write_byte(uint8_t reg,  uint8_t* data, size_t len)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0]  = reg;
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(xl9555_self->iic_obj);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);

    mp_machine_i2c_buf_t bufs[2] = {
        {.len = 1, .buf = memaddr_buf},
        {.len = len, .buf = data},
    };

    i2c_p->transfer(self, XL9555_ADDR, 2, bufs, MP_MACHINE_I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief       Control the level of all IO
 * @param       reg_value:xl9555 register value
 * @retval      Return 16 bit IO status
 */
uint16_t xl9555_multi_write_ex(uint16_t reg_value)
{
    uint8_t w_data[2];
    esp_err_t err;
    int retry = 3;

    w_data[0] = (uint8_t)(0xFF & reg_value);
    w_data[1] = (uint8_t)(0xFF & (reg_value >> 8));

    do
    {
        err = xl9555_write_byte(XL9555_OUTPUT_PORT0_REG, w_data, 2);
        
        if (err != ESP_OK)
        {
            retry--;
            mp_hal_delay_ms(100);
            ESP_LOGE("IIC", "%s failed, prev:%X, target: %X", __func__, xl9555_value, reg_value);
            xl9555_failed = 1;
        }
        else
        {
            xl9555_failed = 0;
            break;
        }
        
    } while (retry);

    if (xl9555_failed == 0)
    {
        xl9555_value = reg_value;
    }
    
    return xl9555_value;
}

/**
 * @brief       Control the level of a certain IO
 * @param       pin:Controlled IO
 * @param       val:status
 * @retval      Return 16 bit IO status
 */
uint16_t xl9555_pin_write(uint16_t pin, int val)
{
    uint16_t i;
    uint16_t wrt_data = xl9555_value;
    uint16_t bitval = (val > 0) ? 1 : 0;
    uint16_t temp;

    for (i = 0; i < 16; i++)
    {
        temp = 0x1 << i;
        if (pin & temp)
        {
            if (val) wrt_data |= temp;
            else wrt_data &= ~temp;
        }
    }

    return xl9555_multi_write_ex(wrt_data);
}
STATIC mp_obj_t mp_xl9555_write(size_t n_args, const mp_obj_t *args)
{
    xl9555_obj_t *self  = MP_OBJ_TO_PTR(args[0]);
    uint16_t bit        = mp_obj_get_int(args[1]);
    int sta             = mp_obj_get_int(args[2]);
    
    xl9555_pin_write(bit,sta);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(xl9555_write_obj, 3, 3, mp_xl9555_write);

/**
 * @brief       Read all IO statuses of XL9555
 * @param       Note
 * @retval      Return 16 bit IO status
 */
uint16_t xl9555_multi_read_ex(void)
{
    uint16_t ret;
    esp_err_t err;
    uint8_t r_data[2];
    int retry = 3;

    do
    {
        err = xl9555_read_byte(r_data, 2);
        
        if (err != ESP_OK)
        {
            retry--;
            mp_hal_delay_ms(100);
            ESP_LOGE("IIC", "%s failed, prev:%X, target: %X", __func__, xl9555_value, xl9555_readonly_value);
            xl9555_value = 1;
        }
        else
        {
            xl9555_value = 0;
            break;
        }
        
    } while (retry);

    ret = r_data[1] << 8 | r_data[0];

    xl9555_readonly_value = ret;
    return ret;
}

/**
 * @brief       Get a certain IO status
 * @param       pin:To obtain the IO status
 * @retval      Return the status of the IO port (0/1)
 */
int xl9555_pin_read(uint16_t pin)
{
    return (xl9555_multi_read_ex() & pin) ? 1 : 0;
}

STATIC mp_obj_t mp_xl9555_read(size_t n_args, const mp_obj_t *args)
{
    xl9555_obj_t *self  = MP_OBJ_TO_PTR(args[0]);
    uint16_t bit        = mp_obj_get_int(args[1]);
    int state           = 0;
    
    state = xl9555_pin_read(bit);
    
    return mp_obj_new_int(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(xl9555_read_obj, 2, 2, mp_xl9555_read);

/**
 * @brief       IO configuration of XL9555
 * @param       config_value:Configuration values
 * @retval      Configuration values
 */
static uint16_t xl9555_ioconfig(uint16_t config_value)
{
    /* addr + CMD + data1(P0) + data2(P1) */
    uint8_t data[2];
    esp_err_t err;
    int retry = 3;

    data[0] = (uint8_t)(0xFF & config_value);
    data[1] = (uint8_t)(0xFF & (config_value >> 8));

    do
    {
        err = xl9555_write_byte(XL9555_CONFIG_PORT0_REG, data, 2);
        
        if (err != ESP_OK)
        {
            retry--;
            mp_hal_delay_ms(100); 
            ESP_LOGE("IIC", "%s configure %X failed, ret: %d", __func__, config_value, err);
            xl9555_failed = 1;
            
            if ((retry <= 0) && xl9555_failed)
            {
                mp_hal_delay_ms(5000); 
                esp_restart();
            }
        }
        else
        {
            xl9555_failed = 0;
            break;
        }
        
    } while (retry);
    
    return config_value;
}

#define KEY0                            xl9555_pin_read(KEY0_IO)        /* read KEY0 IO */
#define KEY1                            xl9555_pin_read(KEY1_IO)        /* read KEY1 IO */
#define KEY2                            xl9555_pin_read(KEY2_IO)        /* read KEY2 IO */
#define KEY3                            xl9555_pin_read(KEY3_IO)        /* read KEY3 IO */

/**
 * @brief       Key scan function
 * @param       Note
 * @retval      KEY0_PRES, 1, KEY0
 *              KEY1_PRES, 2, KEY1
 *              KEY2_PRES, 3, KEY2
 *              KEY3_PRES, 4, KEY3
 */
uint8_t xl9555_key_scan(void)
{
    static uint8_t key_up = 1;
    uint8_t keyval = 0;

    if (key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0  || KEY3 == 0 ))
    {
        vTaskDelay(10);
        key_up = 0;

        if (KEY0 == 0)  keyval = KEY0_PRES;

        if (KEY1 == 0)  keyval = KEY1_PRES;

        if (KEY2 == 0)  keyval = KEY2_PRES;

        if (KEY3 == 0)  keyval = KEY3_PRES;
    }
    else if (KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && KEY3 == 1)
    {
        key_up = 1;
    }

    return keyval;
}
STATIC mp_obj_t mp_xl9555_key_scan(size_t n_args, const mp_obj_t *args)
{
    xl9555_obj_t *self  = MP_OBJ_TO_PTR(args[0]);
    int state           = 0;
    
    state = xl9555_key_scan();
    
    return mp_obj_new_int(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(xl9555_key_scan_obj, 1, 1, mp_xl9555_key_scan);

/**
 * @brief       XL9555 Init
 * @param       Note
 * @retval      Note
 */
void xl9555_init(void)
{
    uint8_t r_data[2];
    xl9555_read_byte(r_data, 2);
    
    /* addr + CMD + 6 input + 12 output */
    xl9555_ioconfig(0xF003);
    xl9555_pin_write(BEEP_IO,1);
    xl9555_pin_write(SPK_EN_IO,1);
}

STATIC const mp_rom_map_elem_t xl9555_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write_bit),   MP_ROM_PTR(&xl9555_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_bit),    MP_ROM_PTR(&xl9555_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_scan),    MP_ROM_PTR(&xl9555_key_scan_obj) },
};

STATIC MP_DEFINE_CONST_DICT(xl9555_locals_dict, xl9555_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    xl9555_type,
    MP_QSTR_init,
    MP_TYPE_FLAG_NONE,
    make_new, xl9555_make_new,
    locals_dict, &xl9555_locals_dict
);

mp_obj_t xl9555_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args )
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

    xl9555_self = m_new_obj(xl9555_obj_t);
    xl9555_self->base.type = &xl9555_type;

    mp_obj_base_t *xl9555_obj   = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[ARG_iic].u_obj);

    if (xl9555_obj == MP_OBJ_NULL)
    {
        mp_raise_ValueError(MP_ERROR_TEXT("I2C init ???"));
    }

    xl9555_self->iic_obj        = xl9555_obj;

    xl9555_init();
    
    return MP_OBJ_FROM_PTR(xl9555_self);
}

STATIC const mp_map_elem_t xl9555_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_atk_xl9555) },
    { MP_ROM_QSTR(MP_QSTR_init), (mp_obj_t)&xl9555_type },
    { MP_ROM_QSTR(MP_QSTR_AP_INT), MP_ROM_INT(AP_INT_IO) },
    { MP_ROM_QSTR(MP_QSTR_QMA_INT), MP_ROM_INT(QMA_INT_IO) },
    { MP_ROM_QSTR(MP_QSTR_SPK_EN), MP_ROM_INT(SPK_EN_IO) },
    { MP_ROM_QSTR(MP_QSTR_BEEP), MP_ROM_INT(BEEP_IO) },
    { MP_ROM_QSTR(MP_QSTR_OV_PWDN), MP_ROM_INT(OV_PWDN_IO) },
    { MP_ROM_QSTR(MP_QSTR_OV_RESET), MP_ROM_INT(OV_RESET_IO) },
    { MP_ROM_QSTR(MP_QSTR_GBC_LED), MP_ROM_INT(GBC_LED_IO) },
    { MP_ROM_QSTR(MP_QSTR_GBC_KEY), MP_ROM_INT(GBC_KEY_IO) },
    { MP_ROM_QSTR(MP_QSTR_LCD_BL), MP_ROM_INT(LCD_BL_IO) },
    { MP_ROM_QSTR(MP_QSTR_CT_RST), MP_ROM_INT(CT_RST_IO) },
    { MP_ROM_QSTR(MP_QSTR_SLCD_RST), MP_ROM_INT(SLCD_RST_IO) },
    { MP_ROM_QSTR(MP_QSTR_SLCD_PWR), MP_ROM_INT(SLCD_PWR_IO) },
    { MP_ROM_QSTR(MP_QSTR_KEY3_IO), MP_ROM_INT(KEY3_IO) },
    { MP_ROM_QSTR(MP_QSTR_KEY2_IO), MP_ROM_INT(KEY2_IO) },
    { MP_ROM_QSTR(MP_QSTR_KEY1_IO), MP_ROM_INT(KEY1_IO) },
    { MP_ROM_QSTR(MP_QSTR_KEY0_IO), MP_ROM_INT(KEY0_IO) },
    { MP_ROM_QSTR(MP_QSTR_KEY3), MP_ROM_INT(KEY3_PRES) },
    { MP_ROM_QSTR(MP_QSTR_KEY2), MP_ROM_INT(KEY2_PRES) },
    { MP_ROM_QSTR(MP_QSTR_KEY1), MP_ROM_INT(KEY1_PRES) },
    { MP_ROM_QSTR(MP_QSTR_KEY0), MP_ROM_INT(KEY0_PRES) },
};

STATIC MP_DEFINE_CONST_DICT (mp_module_xl9555_globals, xl9555_module_globals_table );

const mp_obj_module_t mp_module_xl9555 = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_xl9555_globals,
};

MP_REGISTER_MODULE(MP_QSTR_atk_xl9555, mp_module_xl9555);

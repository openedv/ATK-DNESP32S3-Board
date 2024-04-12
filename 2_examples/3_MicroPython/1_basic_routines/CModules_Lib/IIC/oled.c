/**
 ****************************************************************************************************
 * @file        oled.c
 * @author      ALIENTEK
 * @brief       OLED code
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

#define __OLED_VERSION__  "1.0"

#include "oled.h"
#include "oledfont.h"


uint8_t OLED_GRAM[128][8];

mp_obj_t oled_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args );

typedef struct _oled_obj_t
{
    mp_obj_base_t   base;
    mp_obj_base_t   *iic_obj;
} oled_obj_t;

oled_obj_t *oled_self = NULL;


/**
 * @brief       write data to oled register
 * @param       data: write data 
 * @param       len: len
 * @retval      ESP_OK:none
 */
esp_err_t oled_write(uint8_t* data, size_t len)
{
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(oled_self->iic_obj);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);

    mp_machine_i2c_buf_t bufs = {
        .len = len,
        .buf = data,
    };

    i2c_p->transfer(self, OLED_ADDR, 1, &bufs, MP_MACHINE_I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief       write command to oled
 * @param       tx_data: write data 
 * @param       command: command value
 * @retval      none
 */
void oled_write_Byte(unsigned char tx_data,unsigned char command)
{
    unsigned char data[2] = {command,tx_data};
    oled_write(data, sizeof(data));
}

/**
 * @brief       Update graphics memory to LCD
 * @param       None
 * @retval      None
 */
void oled_refresh_gram(void)
{
    uint8_t i,n;
    
    for (i = 0;i < 8;i++)
    {  
        oled_write_Byte(0xb0 + i,OLED_CMD);
        oled_write_Byte(0x00,OLED_CMD);
        oled_write_Byte(0x10,OLED_CMD);
        
        for (n = 0;n < 128;n ++)
        {
            oled_write_Byte(OLED_GRAM[n][i],OLED_DATA);
        }
    }   
}
STATIC mp_obj_t mp_oled_refresh_gram(mp_obj_t self_in)
{
    oled_refresh_gram();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(oled_refresh_gram_obj, mp_oled_refresh_gram);

/**
 * @brief       OLED module Init
 * @param       None
 * @retval      None
 */
void oled_init(void)
{
    /* init code */
    oled_write_Byte(0xAE,OLED_CMD);
    oled_write_Byte(0xD5,OLED_CMD);
    oled_write_Byte(80,OLED_CMD);
    oled_write_Byte(0xA8,OLED_CMD);
    oled_write_Byte(0X3F,OLED_CMD);
    oled_write_Byte(0xD3,OLED_CMD);
    oled_write_Byte(0X00,OLED_CMD);

    oled_write_Byte(0x40,OLED_CMD);

    oled_write_Byte(0x8D,OLED_CMD);
    oled_write_Byte(0x14,OLED_CMD);
    oled_write_Byte(0x20,OLED_CMD);
    oled_write_Byte(0x02,OLED_CMD);
    oled_write_Byte(0xA1,OLED_CMD);
    oled_write_Byte(0xC0,OLED_CMD);
    oled_write_Byte(0xDA,OLED_CMD);
    oled_write_Byte(0x12,OLED_CMD);
            
    oled_write_Byte(0x81,OLED_CMD);
    oled_write_Byte(0xEF,OLED_CMD);
    oled_write_Byte(0xD9,OLED_CMD);
    oled_write_Byte(0xf1,OLED_CMD);
    oled_write_Byte(0xDB,OLED_CMD);
    oled_write_Byte(0x30,OLED_CMD);

    oled_write_Byte(0xA4,OLED_CMD);
    oled_write_Byte(0xA6,OLED_CMD);
    oled_write_Byte(0xAF,OLED_CMD);

    oled_on();
    oled_clear();
}

/**
 * @brief       Open oled
 * @param       None
 * @retval      None
 */
void oled_on(void)
{
    oled_write_Byte(0X8D,OLED_CMD);
    oled_write_Byte(0X14,OLED_CMD);
    oled_write_Byte(0XAF,OLED_CMD);
}

STATIC mp_obj_t mp_oled_on(mp_obj_t self_in)
{
    oled_on();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(oled_on_obj, mp_oled_on);

/**
 * @brief       Close oled
 * @param       None
 * @retval      None
 */
void oled_off(void)
{
    oled_write_Byte(0X8D,OLED_CMD);
    oled_write_Byte(0X10,OLED_CMD);
    oled_write_Byte(0XAE,OLED_CMD);
}
STATIC mp_obj_t mp_oled_off(mp_obj_t self_in)
{
    oled_off();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(oled_off_obj, mp_oled_off);

/**
 * @brief       clear
 * @param       None
 * @retval      None
 */
void oled_clear(void)
{
    uint8_t i,n;

    for (i = 0;i < 8;i++)
    {
        for (n = 0;n < 128;n++)
        {
            OLED_GRAM[n][i] = 0X00;
        }
    }

    oled_refresh_gram();
}
STATIC mp_obj_t mp_oled_clear(mp_obj_t self_in)
{
    oled_clear();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(oled_clear_obj, mp_oled_clear);

/**
 * @brief       draw point
 * @param       x    : The x-coordinate of the point to be drawn
 *              y    : The y-coordinate of the point to be drawn
 *              t: The color of the dot to be painted
 * @retval      None
 */
void oled_draw_point(uint8_t x,uint8_t y,uint8_t t)
{
    uint8_t pos,bx,temp = 0;

    if (x > 127 || y > 63)
    {
        return;
    }

    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);

    if(t)
    {
        OLED_GRAM[x][pos] |= temp;
    }
    else
    {
        OLED_GRAM[x][pos] &= ~ temp;
    }
}
STATIC mp_obj_t mp_oled_pixel(size_t n_args, const mp_obj_t *args)
{
    oled_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t t = mp_obj_get_int(args[3]);
    
    oled_draw_point(x, y, t);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(oled_pixel_obj, 4, 4, mp_oled_pixel);

/**
 * @brief       area filling
 * @param       x1: region start x coordinate
 *              y1: region start y coordinate
 *              x2: region end x coordinate
 *              y2: region end y coordinate
 *              dot: The color of the area to be painted
 * @retval      None
 */
void oled_fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)
{
    uint8_t x,y;
    
    for (x = x1;x <= x2;x++)
    {
        for (y = y1;y <= y2;y++)
        {
            oled_draw_point(x,y,dot);
        }
    }

    oled_refresh_gram();
}
STATIC mp_obj_t mp_oled_fill(size_t n_args, const mp_obj_t *args)
{
    oled_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x0     = mp_obj_get_int(args[1]);
    mp_int_t y0     = mp_obj_get_int(args[2]);
    mp_int_t x1     = mp_obj_get_int(args[3]);
    mp_int_t y1     = mp_obj_get_int(args[3]);
    mp_int_t dot    = mp_obj_get_int(args[5]);
    
    oled_fill(x0,y0,x1,y1,dot);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(oled_fill_obj, 6, 6, mp_oled_fill);

/**
 * @brief       displays 1 character
 * @param       x    : The X coordinate of the character to be displayed
 *              y    : The Y coordinate of the character to be displayed
 *              chr  : Character to be displayed
 *              mode : 0:display in reverse.1:Normal display
 *              size : Font size
 * @retval      None
 */
void oled_show_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{
    uint8_t temp,t,t1;
    uint8_t y0=y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    chr = chr - ' ';                            /* The offset value is obtained(the ASCII library is modded from a space, so - "is the library of the corresponding character) */
    
    for (t = 0;t < csize;t ++)                  /* Gets the height and width of the font */
    {   
        if (size == 12)
        {
            temp = atk_asc2_1206[chr][t];
        }
        else if (size == 16)
        {
            temp = atk_asc2_1608[chr][t];
        }
        else if (size == 24)
        {
            temp = atk_asc2_2412[chr][t];
        }
        else
        {
            return;
        }

        for (t1 = 0;t1 < 8;t1++)
        {
            if (temp & 0x80)
            {
                oled_draw_point(x,y,mode);
            }
            else
            {
                oled_draw_point(x,y,!mode);
            }

            temp <<= 1;
            y++;

            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

STATIC mp_obj_t mp_oled_char(size_t n_args, const mp_obj_t *args)
{
    const char *chr;
    
    oled_obj_t *self    = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x          = mp_obj_get_int(args[1]);
    mp_int_t y          = mp_obj_get_int(args[2]);
    
    chr                 = mp_obj_str_get_str(args[3]);
    
    uint8_t size        = mp_obj_get_int(args[4]);
    uint8_t mode        = mp_obj_get_int(args[5]);
    
    oled_show_char(x,y,*chr,size,mode);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(oled_char_obj, 6, 6, mp_oled_char);

/**
 * @brief       Square function,x^y
 * @param       x: Base number
 *              y: index
 * @retval      x^y
 */
uint32_t mypow(uint8_t m,uint8_t n)
{
    uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/**
 * @brief       displays numbers and does not display high zeros
 * @param       x    : The X coordinate of the number to be displayed
 *              y    : The Y coordinate of the number to be displayed
 *              num  : To display numbers
 *              len  : The number of digits to be displayed
 *              size : Font size
 * @retval      None
 */
void oled_show_num(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{
    uint8_t t,temp;
    uint8_t enshow = 0;
    
    for (t = 0;t < len;t++)
    {
        temp = (num / mypow(10,len - t - 1))%10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                oled_show_char(x + (size / 2)*t,y,' ',size,1);
                continue;
            }
            else
            {
                enshow = 1;
            }
            
        }

        oled_show_char(x + (size / 2)*t,y,temp + '0',size,1); 
    }
} 
STATIC mp_obj_t mp_oled_num(size_t n_args, const mp_obj_t *args)
{
    oled_obj_t *self    = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x          = mp_obj_get_int(args[1]);
    mp_int_t y          = mp_obj_get_int(args[2]);
    uint32_t num        = mp_obj_get_int(args[3]);
    uint8_t len         = mp_obj_get_int(args[4]);
    uint8_t size        = mp_obj_get_int(args[5]);
    
    oled_show_num(x,y,num,len,size);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(oled_num_obj, 6, 6, mp_oled_num);

/**
 * @brief       show string
 * @param       x    : The x-coordinate of the string to be displayed
 *              y    : The y-coordinate of the string to be displayed
 *              *p   : To display string
 *              size : Font size
 * @retval      None
 */
void oled_show_string(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size)
{
    while ((*p <= '~') && (*p >= ' '))
    {       
        if (x > (128 - (size / 2)))
        {
            x=0;y+=size;
        }
        if (y > (64 - size))
        {
            y = x = 0;
            oled_clear();
        }

        oled_show_char(x,y,*p,size,1);
        
        x += size / 2;
        p++;
    }
}

STATIC mp_obj_t mp_oled_string(size_t n_args, const mp_obj_t *args)
{
    oled_obj_t *self    = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x          = mp_obj_get_int(args[1]);
    mp_int_t y          = mp_obj_get_int(args[2]);
    uint8_t *p          = mp_obj_str_get_str(args[3]);
    uint16_t size       = mp_obj_get_int(args[4]);
    
    oled_show_string(x,y,p,size);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(oled_string_obj, 5, 5, mp_oled_string);


STATIC const mp_rom_map_elem_t oled_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&oled_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&oled_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear),   MP_ROM_PTR(&oled_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel),   MP_ROM_PTR(&oled_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill),    MP_ROM_PTR(&oled_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_char),    MP_ROM_PTR(&oled_char_obj) },
    { MP_ROM_QSTR(MP_QSTR_num),     MP_ROM_PTR(&oled_num_obj) },
    { MP_ROM_QSTR(MP_QSTR_string),  MP_ROM_PTR(&oled_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh_gram),    MP_ROM_PTR(&oled_refresh_gram_obj) },
};

STATIC MP_DEFINE_CONST_DICT(oled_locals_dict, oled_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    oled_type,
    MP_QSTR_init,
    MP_TYPE_FLAG_NONE,
    make_new, oled_make_new,
    locals_dict, &oled_locals_dict
);

mp_obj_t oled_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args )
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
    
    oled_self = m_new_obj(oled_obj_t);
    oled_self->base.type = &oled_type;
    mp_obj_base_t *oled_obj   = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[ARG_iic].u_obj);

    if (oled_obj == MP_OBJ_NULL)
    {
        mp_raise_ValueError(MP_ERROR_TEXT("I2C init ???"));
    }

    oled_self->iic_obj        = oled_obj;

    oled_init();
    
    return MP_OBJ_FROM_PTR(oled_self);
}

STATIC const mp_map_elem_t oled_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_atk_oled) },
    { MP_ROM_QSTR(MP_QSTR_init), (mp_obj_t)&oled_type },
};

STATIC MP_DEFINE_CONST_DICT (mp_module_oled_globals, oled_module_globals_table );

const mp_obj_module_t mp_module_oled = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_oled_globals,
};

MP_REGISTER_MODULE(MP_QSTR_atk_oled, mp_module_oled);

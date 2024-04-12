/**
 ****************************************************************************************************
 * @file        ltdc.c
 * @author      ALIENTEK
 * @brief       LTDC code
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

#define __LTDC_VERSION__  "1.0"

#include "ltdc.h"
#include "ltdcfont.h"


mp_obj_t ltdc_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args );

_ltdc_dev *ltdc_self = NULL ;
esp_lcd_panel_handle_t panel_handle = NULL;
static const char *TAG = "ltdc";

/**
 * @brief       read ID
 * @note        Using the highest bit (R7, G7, B7) of LCD RGB lines to identify panel IDs
 *              PG6 = R7(M0); PI2 = G7(M1); PI7 = B7(M2);
 *              M2:M1:M0
 *              0 :0 :0     4.3 寸480*272  RGB,ID = 0X4342
 *              1 :0 :0     4.3 寸800*480  RGB,ID = 0X4384
 * @param       None
 * @retval      LCD ID
 */
uint16_t ltdc_panelid_read(void)
{
    uint8_t idx = 0;
    gpio_config_t gpio_init_struct;

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;
    gpio_init_struct.mode = GPIO_MODE_INPUT;
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_init_struct.pin_bit_mask = 1ull << GPIO_NUM_6;
    gpio_config(&gpio_init_struct);                         /* Configure GPIO */

    gpio_init_struct.pin_bit_mask = 1ull << GPIO_NUM_14 || 1ull << GPIO_NUM_18;
    gpio_config(&gpio_init_struct);                         /* Configure GPIO */

    idx = (uint8_t)gpio_get_level(GPIO_NUM_14);             /* read M0 */
    idx|= (uint8_t)gpio_get_level(GPIO_NUM_18) << 1;        /* read M1 */
    idx|= (uint8_t)gpio_get_level(GPIO_NUM_6) << 2;         /* read M2 */

    switch (idx)
    {
        case 0 : 
            return 0X4342;
        case 4 : 
            return 0X4384;
        default : 
            return 0;
    }
}

/**
 * @brief       LCD clear
 * @param       color: LCD color
 * @retval      None
 */
void ltdc_clear(uint16_t color)
{
    uint16_t *buffer = heap_caps_malloc(ltdc_self->width * sizeof(uint16_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    if (NULL == buffer)
    {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
    }
    else
    {
        for (uint16_t i = 0; i < ltdc_self->width; i++)
        {
            buffer[i] = color;
        }
        
        for (int y = 0; y < ltdc_self->height; y++)
        {
            esp_lcd_panel_draw_bitmap(panel_handle, 0, y, ltdc_self->width, y+1, buffer);
        }
        
        heap_caps_free(buffer);
    }
}
STATIC mp_obj_t mp_ltdc_fast_clear(mp_obj_t self_in, mp_obj_t _color)
{
    _ltdc_dev *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t color = mp_obj_get_int(_color);
    
    ltdc_clear(color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ltdc_clear_obj, mp_ltdc_fast_clear);

/**
 * @brief       Set the LCD display direction
 * @param       dir:0,portrait; 1,landscape
 * @retval      None
 */
void ltdc_display_dir(uint8_t dir)
{
    ltdc_self->dir = dir;

    if (dir == 0)
    {
        ltdc_self->width = ltdc_self->pheight;
        ltdc_self->height = ltdc_self->pwidth;
        esp_lcd_panel_swap_xy(panel_handle, true);
        esp_lcd_panel_mirror(panel_handle, false, true);
    }
    else if (dir == 1)
    {
        ltdc_self->width = ltdc_self->pwidth;
        ltdc_self->height = ltdc_self->pheight;
        esp_lcd_panel_swap_xy(panel_handle, false);
        esp_lcd_panel_mirror(panel_handle, false, false);
    }
}

/**
 * @brief       LCD draw point
 * @param       x    : The x-coordinate of the point to be drawn
 *              y    : The y-coordinate of the point to be drawn
 *              color: The color of the dot to be painted
 * @retval      None
 */
void ltdc_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    uint16_t at_color[1] = {0};
    at_color[0] = color;
    esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 1, y + 1, at_color);
}

STATIC mp_obj_t mp_ltdc_pixel(size_t n_args, const mp_obj_t *args)
{
    _ltdc_dev *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t color = mp_obj_get_int(args[3]);
    
    ltdc_draw_point( x, y, color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ltdc_pixel_obj, 4, 4, mp_ltdc_pixel);

/**
 * @brief       LCD draw line
 * @param       x1   : The x-coordinate of end point 1 of the line to be drawn
 * @param       y1   : The y-coordinate of end point 1 of the line to be drawn
 * @param       x2   : The x-coordinate of end point 2 of the line to be drawn
 * @param       y2   : The y-coordinate of end point 2 of the line to be drawn
 * @param       color: The color of the line to be painted
 * @retval      None
 */
void ltdc_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)
    {
        incx = 1;
    }
    else if (delta_x == 0)
    {
        incx = 0;
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)
    {
        distance = delta_x;
    }
    else
    {
        distance = delta_y;
    }

    for (t = 0; t <= distance + 1; t++ )
    {
        ltdc_draw_point(row, col, color);
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

STATIC mp_obj_t mp_ltdc_line(size_t n_args, const mp_obj_t *args)
{
    _ltdc_dev *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x0     = mp_obj_get_int(args[1]);
    mp_int_t y0     = mp_obj_get_int(args[2]);
    mp_int_t x1     = mp_obj_get_int(args[3]);
    mp_int_t y1     = mp_obj_get_int(args[3]);
    mp_int_t color  = mp_obj_get_int(args[5]);
    
    ltdc_draw_line(x0,y0,x1,y1,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ltdc_line_obj, 6, 6, mp_ltdc_line);

/**
 * @brief       LCD draw rectangle
 * @param       x1   : The x-coordinate of end point 1 of the rectangle to be drawn
 *              y1   : The y-coordinate of end point 1 of the rectangle to be drawn
 *              x2   : The x-coordinate of end point 2 of the rectangle to be drawn
 *              y2   : The y-coordinate of end point 2 of the rectangle to be drawn
 *              color: The color of the rectangle to be painted
 * @retval      None
 */
void ltdc_draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color)
{
    ltdc_draw_line(x0, y0, x1, y0,color);
    ltdc_draw_line(x0, y0, x0, y1,color);
    ltdc_draw_line(x0, y1, x1, y1,color);
    ltdc_draw_line(x1, y0, x1, y1,color);
}

STATIC mp_obj_t mp_ltdc_rectangle(size_t n_args, const mp_obj_t *args)
{
    _ltdc_dev *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x0     = mp_obj_get_int(args[1]);
    mp_int_t y0     = mp_obj_get_int(args[2]);
    mp_int_t x1     = mp_obj_get_int(args[3]);
    mp_int_t y1     = mp_obj_get_int(args[3]);
    mp_int_t color  = mp_obj_get_int(args[5]);
    
    ltdc_draw_rectangle(x0,y0,x1,y1,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ltdc_rectangle_obj, 6, 6, mp_ltdc_rectangle);

/**
 * @brief       LCD draw circle
 * @param       x    : The x-coordinate of the origin of the circle to be drawn
 *              y    : The Y coordinate of the origin of the circle to be drawn
 *              r    : The radius of the circle to be drawn
 *              color: The color of the circle to be painted
 * @retval      None
 */
void ltdc_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);

    while (a <= b)
    {
        ltdc_draw_point(x0 + a, y0 - b, color);  /* 5 */
        ltdc_draw_point(x0 + b, y0 - a, color);  /* 0 */
        ltdc_draw_point(x0 + b, y0 + a, color);  /* 4 */
        ltdc_draw_point(x0 + a, y0 + b, color);  /* 6 */
        ltdc_draw_point(x0 - a, y0 + b, color);  /* 1 */
        ltdc_draw_point(x0 - b, y0 + a, color);
        ltdc_draw_point(x0 - a, y0 - b, color);  /* 2 */
        ltdc_draw_point(x0 - b, y0 - a, color);  /* 7 */
        a++;

        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

STATIC mp_obj_t mp_ltdc_circle(size_t n_args, const mp_obj_t *args)
{
    _ltdc_dev *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x      = mp_obj_get_int(args[1]);
    mp_int_t y      = mp_obj_get_int(args[2]);
    mp_int_t r      = mp_obj_get_int(args[3]);
    mp_int_t color  = mp_obj_get_int(args[4]);
    
    ltdc_draw_circle(x,y,r,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ltdc_circle_obj, 5, 5, mp_ltdc_circle);

/**
 * @brief       The LCD displays 1 character
 * @param       x    : The X coordinate of the character to be displayed
 *              y    : The Y coordinate of the character to be displayed
 *              chr  : Character to be displayed
 *              mode : stacking(1); non-stacking(0)
 *              color: The color of the character to be displayed
 * @retval      None
 */
void ltdc_show_char(uint16_t x, uint16_t y, uint8_t chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    chr = chr - ' ';       /* The offset value is obtained(the ASCII library is modded from a space, so - "is the library of the corresponding character) */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)ltdc_asc2_1206[chr];
            break;

        case 16:
            pfont = (uint8_t *)ltdc_asc2_1608[chr];
            break;

        case 24:
            pfont = (uint8_t *)ltdc_asc2_2412[chr];
            break;

        case 32:
            pfont = (uint8_t *)ltdc_asc2_3216[chr];
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
            {
                ltdc_draw_point(x, y, color);
            }
            else if (mode == 0)
            {
                ltdc_draw_point(x, y, 0xffff);
            }

            temp <<= 1;
            y++;

            if (y >= ltdc_self->height)return;

            if ((y - y0) == size)
            {
                y = y0;
                x++;

                if (x >= ltdc_self->width)
                {
                    return;
                }

                break;
            }
        }
    }
}

STATIC mp_obj_t mp_ltdc_char(size_t n_args, const mp_obj_t *args)
{
    const char *chr;
    
    _ltdc_dev *self     = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x          = mp_obj_get_int(args[1]);
    mp_int_t y          = mp_obj_get_int(args[2]);
    
    chr                 = mp_obj_str_get_str(args[3]);
    
    uint8_t size        = mp_obj_get_int(args[4]);
    uint8_t mode        = mp_obj_get_int(args[5]);
    uint16_t color      = mp_obj_get_int(args[6]);
    
    ltdc_show_char(x,y,*chr,size,mode,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ltdc_char_obj, 7, 7, mp_ltdc_char);

/**
 * @brief       Square function,x^y
 * @param       x: Base number
 *              y: index
 * @retval      x^y
 */
static uint32_t ltdc_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/**
 * @brief       LCD displays numbers and does not display high zeros
 * @param       x    : The X coordinate of the number to be displayed
 *              y    : The Y coordinate of the number to be displayed
 *              num  : To display numbers
 *              len  : The number of digits to be displayed
 *              color: The color of the numbers to be displayed
 * @retval      None
 */
void ltdc_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / ltdc_pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                ltdc_show_char(x + (size / 2) * t, y, ' ', size, 0, color);
                continue;
            }
            else
            {
                enshow = 1;
            }
        }

        ltdc_show_char(x + (size / 2) * t, y, temp + '0', size, 0, color);
    }
}

STATIC mp_obj_t mp_ltdc_num(size_t n_args, const mp_obj_t *args)
{
    _ltdc_dev *self     = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x          = mp_obj_get_int(args[1]);
    mp_int_t y          = mp_obj_get_int(args[2]);
    uint32_t num        = mp_obj_get_int(args[3]);
    uint8_t len         = mp_obj_get_int(args[4]);
    uint8_t size        = mp_obj_get_int(args[5]);
    uint16_t color      = mp_obj_get_int(args[6]);
    
    ltdc_show_num(x,y,num,len,size,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ltdc_num_obj, 7, 7, mp_ltdc_num);

/**
 * @brief       LCD show string
 * @param       x    : The x-coordinate of the string to be displayed
 *              y    : The y-coordinate of the string to be displayed
 *              str  : To display string
 *              color: The color of the string to be displayed
 * @retval      None
 */
void ltdc_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' ')) 
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)
        {
            break;
        }

        ltdc_show_char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}

STATIC mp_obj_t mp_ltdc_string(size_t n_args, const mp_obj_t *args)
{
    _ltdc_dev *self     = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x          = mp_obj_get_int(args[1]);
    mp_int_t y          = mp_obj_get_int(args[2]);
    uint16_t width      = mp_obj_get_int(args[3]);
    uint16_t height     = mp_obj_get_int(args[4]);
    uint8_t size        = mp_obj_get_int(args[5]);
    char *p             = mp_obj_str_get_str(args[6]);
    uint16_t color      = mp_obj_get_int(args[7]);
    
    ltdc_show_string(x,y,width,height,size,p,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ltdc_string_obj, 8, 8, mp_ltdc_string);

/**
 * @brief       LCD area filling
 * @param       xs: region start x coordinate
 *              ys: region start y coordinate
 *              xe: region end x coordinate
 *              ye: region end y coordinate
 *              color: The color of the area to be painted
 * @retval      None
 */
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    esp_lcd_panel_draw_bitmap(panel_handle, sx, sy, ex, ey, (uint16_t *)color);
}
STATIC mp_obj_t mp_ltdc_fill(size_t n_args, const mp_obj_t *args)
{
    _ltdc_dev *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x0     = mp_obj_get_int(args[1]);
    mp_int_t y0     = mp_obj_get_int(args[2]);
    mp_int_t x1     = mp_obj_get_int(args[3]);
    mp_int_t y1     = mp_obj_get_int(args[3]);
    mp_int_t color  = mp_obj_get_int(args[5]);
    
    ltdc_color_fill(x0,y0,x1,y1,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ltdc_fill_obj, 6, 6, mp_ltdc_fill);

/**
 * @brief       ltdc init
 * @param       None
 * @retval      None
 */
void ltdc_init(void)
{
    panel_handle = NULL;
    ltdc_self->id = ltdc_panelid_read();        /* read ID */

    if (ltdc_self->id == 0X4342)                   /* 480*272 */
    {
        ltdc_self->pwidth = 480;
        ltdc_self->pheight = 272;
        ltdc_self->hsw = 1;                        /* Horizontal rear corridor */
        ltdc_self->vsw = 1;                        /* Horizontal front porch */
        ltdc_self->hbp = 40;                       /* Horizontal synchronization width */
        ltdc_self->vbp = 8;                        /* Vertical rear corridor */
        ltdc_self->hfp = 5;                        /* Vertical front porch */
        ltdc_self->vfp = 8;                        /* Vertical synchronization width */
        ltdc_self->pclk_hz = 9 * 1000 * 1000;      /* Set pixel clock 15Mhz */
    }
    else if (ltdc_self->id == 0X4384)
    {
        ltdc_self->pwidth = 800;
        ltdc_self->pheight = 480;
        ltdc_self->hbp = 88;                       /* Horizontal rear corridor */
        ltdc_self->hfp = 40;                       /* Horizontal front porch */
        ltdc_self->hsw = 48;                       /* Horizontal synchronization width */
        ltdc_self->vbp = 32;                       /* Vertical rear corridor */
        ltdc_self->vfp = 13;                       /* Vertical front porch */
        ltdc_self->vsw = 3;                        /* Vertical synchronization width */
        ltdc_self->pclk_hz = 15 * 1000 * 1000;     /* Set pixel clock 15Mhz */
    }

    /* Configure parameters */
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,
        .psram_trans_align = 64,
        .clk_src = LCD_CLK_SRC_PLL160M,
        .disp_gpio_num = GPIO_NUM_NC,
        .pclk_gpio_num = GPIO_LCD_PCLK,
        .de_gpio_num = GPIO_LCD_DE,
        .data_gpio_nums = {
            GPIO_LCD_B3, GPIO_LCD_B4, GPIO_LCD_B5, GPIO_LCD_B6, GPIO_LCD_B7,
            GPIO_LCD_G2, GPIO_LCD_G3, GPIO_LCD_G4, GPIO_LCD_G5, GPIO_LCD_G6, GPIO_LCD_G7,
            GPIO_LCD_R3, GPIO_LCD_R4, GPIO_LCD_R5, GPIO_LCD_R6, GPIO_LCD_R7,
        },
        .timings = {
            .pclk_hz = ltdc_self->pclk_hz,
            .h_res = ltdc_self->pwidth,
            .v_res = ltdc_self->pheight,
            .hsync_back_porch = ltdc_self->hbp,
            .hsync_front_porch = ltdc_self->hfp,
            .hsync_pulse_width = ltdc_self->vsw,
            .vsync_back_porch = ltdc_self->vbp,
            .vsync_front_porch = ltdc_self->vfp,
            .vsync_pulse_width = ltdc_self->hsw,
            .flags.pclk_active_neg = true,
        },
        .flags.fb_in_psram = true,
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ltdc_display_dir(ltdc_self->dir);
    ltdc_clear(WHITE);
}

STATIC const mp_rom_map_elem_t ltdc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_pixel),       MP_ROM_PTR(&ltdc_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_line),        MP_ROM_PTR(&ltdc_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle),   MP_ROM_PTR(&ltdc_rectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle),      MP_ROM_PTR(&ltdc_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_char),        MP_ROM_PTR(&ltdc_char_obj) },
    { MP_ROM_QSTR(MP_QSTR_num),         MP_ROM_PTR(&ltdc_num_obj) },
    { MP_ROM_QSTR(MP_QSTR_string),      MP_ROM_PTR(&ltdc_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear),       MP_ROM_PTR(&ltdc_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill),        MP_ROM_PTR(&ltdc_fill_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ltdc_locals_dict, ltdc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    ltdc_type,
    MP_QSTR_init,
    MP_TYPE_FLAG_NONE,
    make_new, ltdc_make_new,
    locals_dict, &ltdc_locals_dict
    );

mp_obj_t ltdc_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args )
{
    enum
    {
        ARG_dir,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_dir, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ltdc_self = m_new_obj(_ltdc_dev);
    ltdc_self->base.type = &ltdc_type;

    ltdc_self->dir      = args[ARG_dir].u_int;
    
    if (ltdc_self->dir != 1 && ltdc_self->dir != 0)
    {
        mp_raise_ValueError(MP_ERROR_TEXT("Not horizontal and vertical screens"));
    }

    ltdc_init();
    
    return MP_OBJ_FROM_PTR(ltdc_self);
}

/**
 * @brief       RGB888 to RGB565
 * @param       r:red
 * @param       g:Green
 * @param       b:Blue
 * @retval      Return RGB565 color value
 */
uint16_t ltdc_rgb888_to_565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

STATIC mp_obj_t mp_ltdc_rgb888_to_565(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
    return MP_OBJ_NEW_SMALL_INT(ltdc_rgb888_to_565(
        (uint8_t)mp_obj_get_int(r),
        (uint8_t)mp_obj_get_int(g),
        (uint8_t)mp_obj_get_int(b)
    ));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ltdc_color565_obj, mp_ltdc_rgb888_to_565);

STATIC const mp_map_elem_t ltdc_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_atk_ltdc) },
    { MP_ROM_QSTR(MP_QSTR_rgb888_to_565), (mp_obj_t)&ltdc_color565_obj },
    { MP_ROM_QSTR(MP_QSTR_init), (mp_obj_t)&ltdc_type },
    { MP_ROM_QSTR(MP_QSTR_BLACK), MP_ROM_INT(BLACK) },
    { MP_ROM_QSTR(MP_QSTR_BLUE), MP_ROM_INT(BLUE) },
    { MP_ROM_QSTR(MP_QSTR_RED), MP_ROM_INT(RED) },
    { MP_ROM_QSTR(MP_QSTR_GREEN), MP_ROM_INT(GREEN) },
    { MP_ROM_QSTR(MP_QSTR_CYAN), MP_ROM_INT(CYAN) },
    { MP_ROM_QSTR(MP_QSTR_MAGENTA), MP_ROM_INT(MAGENTA) },
    { MP_ROM_QSTR(MP_QSTR_YELLOW), MP_ROM_INT(YELLOW) },
    { MP_ROM_QSTR(MP_QSTR_WHITE), MP_ROM_INT(WHITE) },
    { MP_ROM_QSTR(MP_QSTR_BROWN), MP_ROM_INT(BROWN) },
    { MP_ROM_QSTR(MP_QSTR_BRRED), MP_ROM_INT(BRRED) },
    { MP_ROM_QSTR(MP_QSTR_GRAY), MP_ROM_INT(GRAY) },
    { MP_ROM_QSTR(MP_QSTR_DARKBLUE), MP_ROM_INT(DARKBLUE) },
    { MP_ROM_QSTR(MP_QSTR_LIGHTBLUE), MP_ROM_INT(LIGHTBLUE) },
    { MP_ROM_QSTR(MP_QSTR_GRAYBLUE), MP_ROM_INT(GRAYBLUE) },
    { MP_ROM_QSTR(MP_QSTR_LIGHTGREEN), MP_ROM_INT(LIGHTGREEN) },
    { MP_ROM_QSTR(MP_QSTR_LGRAY), MP_ROM_INT(LGRAY) },
    { MP_ROM_QSTR(MP_QSTR_LGRAYBLUE), MP_ROM_INT(LGRAYBLUE) },
    { MP_ROM_QSTR(MP_QSTR_LBBLUE), MP_ROM_INT(LBBLUE) },	
};

STATIC MP_DEFINE_CONST_DICT (mp_module_ltdc_globals, ltdc_module_globals_table );

const mp_obj_module_t mp_module_ltdc = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ltdc_globals,
};

MP_REGISTER_MODULE(MP_QSTR_atk_ltdc, mp_module_ltdc);

/**
 ****************************************************************************************************
 * @file        lcd.cpp
 * @author      ALIENTEK
 * @brief       LCD code
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

#define __LCD_VERSION__  "1.0"

#include "lcd.h"
#include "lcdfont.h"


#ifndef MP_ERROR_TEXT
#define MP_ERROR_TEXT(a) a
#endif

#define _swap_bytes(val) ((((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00))
#define mp_hal_delay_ms(delay)  (mp_hal_delay_us(delay * 1000))

/* Store a frame of image data */
#define LCD_TOTAL_BUF_SIZE      (320 * 240 * 2)
#define LCD_BUF_SIZE            15360
#define PARALLEL_LINES          16
static uint8_t lcd_buf[LCD_TOTAL_BUF_SIZE];

uint16_t    g_back_color = WHITE;                                           /* 背景颜色 默认为白色 */

mp_obj_t lcd_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args );


typedef struct _lcd_obj_t
{
    mp_obj_base_t       base;
    mp_obj_base_t       *spi_obj;
    uint16_t            width;
    uint16_t            height;
    uint8_t             dir;
    uint8_t             lcd;
    uint16_t            wramcmd;
    uint16_t            setxcmd;
    uint16_t            setycmd;
    mp_hal_pin_obj_t    reset;
    mp_hal_pin_obj_t    dc;
    mp_hal_pin_obj_t    cs;
    mp_hal_pin_obj_t    backlight;
} lcd_obj_t;

static lcd_obj_t *lcd_self = NULL ;

#define CS_LOW()     mp_hal_pin_write(lcd_self->cs, 0)
#define CS_HIGH()    mp_hal_pin_write(lcd_self->cs, 1)
#define DC_LOW()     mp_hal_pin_write(lcd_self->dc, 0)
#define DC_HIGH()    mp_hal_pin_write(lcd_self->dc, 1)

typedef struct
{
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes;
} lcd_init_cmd_t;


/**
 * @brief       SPI Send data
 * @param       buf：       data
 * @param       len：       lenght
 * @retval      None
 */
STATIC void lcd_write_cmd(const uint8_t cmd)
{
    CS_LOW();
    DC_LOW();
    mp_obj_base_t *s = (mp_obj_base_t *)MP_OBJ_TO_PTR(lcd_self->spi_obj);
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t *)MP_OBJ_TYPE_GET_SLOT(s->type, protocol);
    spi_p->transfer(s, 1, &cmd, NULL);
    CS_HIGH();
}

/**
 * @brief       write command to LCD
 * @param       cmd:command
 * @retval      None
 */
STATIC void lcd_write_data(const uint8_t *data, int len)
{
    CS_LOW();
    DC_HIGH();
    mp_obj_base_t *s = (mp_obj_base_t *)MP_OBJ_TO_PTR(lcd_self->spi_obj);
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t *)MP_OBJ_TYPE_GET_SLOT(s->type, protocol);
    spi_p->transfer(s, len, data, NULL);
    CS_HIGH();
}

/**
 * @brief       write data to LCD
 * @param       data:8 bit data
 * @param       len :lengit
 * @retval      None
 */
STATIC void lcd_write_data16(uint16_t data)
{
    uint8_t dataBuf[2] = {0,0};
    dataBuf[0] = data >> 8;
    dataBuf[1] = data & 0xFF;
    lcd_write_data(&dataBuf,sizeof(dataBuf));
}

/**
 * @brief       Set the LCD row and column address
 * @param       xs: column start address
 *              ys: row start address
 *              xe: column end address
 *              ye: row end address
 * @retval      None
 */
void lcd_set_window(uint16_t xstar, uint16_t ystar,uint16_t xend,uint16_t yend)
{	
    uint8_t databuf[4] = {0,0,0,0};
    databuf[0] = xstar >> 8;
    databuf[1] = 0xFF & xstar;	
    databuf[2] = xend >> 8;
    databuf[3] = 0xFF & xend;
    lcd_write_cmd(lcd_self->setxcmd);
    lcd_write_data(databuf,4);

    databuf[0] = ystar >> 8;
    databuf[1] = 0xFF & ystar;	
    databuf[2] = yend >> 8;
    databuf[3] = 0xFF & yend;
    lcd_write_cmd(lcd_self->setycmd);
    lcd_write_data(databuf,4);

    lcd_write_cmd(lcd_self->wramcmd);
}   

/**
 * @brief       LCD clear
 * @param       color: LCD color
 * @retval      None
 */
void lcd_clear(uint16_t color)
{
    uint16_t i, j;
    uint8_t data[2] = {0};

    data[0] = color >> 8;
    data[1] = color;

    lcd_set_window(0, 0, lcd_self->width - 1, lcd_self->height - 1);

    for(j = 0; j < LCD_BUF_SIZE / 2; j++)
    {
        lcd_buf[j * 2] =  data[0];
        lcd_buf[j * 2 + 1] =  data[1];
    }

    for(i = 0; i < (LCD_TOTAL_BUF_SIZE / LCD_BUF_SIZE); i++)
    {
        lcd_write_data(lcd_buf, LCD_BUF_SIZE);
    }
}
STATIC mp_obj_t lcd_fast_clear(mp_obj_t self_in, mp_obj_t _color)
{
    lcd_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t color = mp_obj_get_int(_color);
    
    lcd_clear(color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_clear_obj, lcd_fast_clear);

/**
 * @brief       Set the position of the cursor
 * @param       Xpos：region start x coordinate
 * @param       Ypos：region start y coordinate
 * @retval      None
 */
void lcd_set_cursor(uint16_t xpos, uint16_t ypos)
{
    lcd_set_window(xpos,ypos,xpos,ypos);	
} 

/**
 * @brief       Set the automatic LCD scanning direction
 * @note        Generally, this parameter is set to L2R_U2D. 
 *              If this parameter is set to other scanning modes, the display may be abnormal.
 * @param       dir:0~7,8 directions(lcd.h)
 * @retval      None
 */
STATIC void lcd_scan_dir(uint8_t dir)
{
    uint8_t regval = 0;
    uint8_t dirreg = 0;
    uint16_t temp;
    uint8_t databuf[4] = {0,0,0,0};

    /* In landscape mode, change the scanning direction */
    if (self->dir == 1)
    {
        dir = 5;
    }

    /* Set the 0x36 register bit 5,6, and 7 values according to the scan mode */
    switch (dir)
    {
        case L2R_U2D:/* From left to right, top to bottom */
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U:/* From left to right, from bottom to top */
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D:/* Right to left, top to bottom */
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U:/* Right to left, bottom to top */
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R:/* From top to bottom, left to right */
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L:/* From top to bottom, right to left */
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R:/* From bottom to top, left to right */
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L:/* From bottom to top, right to left */
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    dirreg = 0x36;

    uint8_t date_send[1] = {regval};
    
    lcd_write_cmd(dirreg);
    lcd_write_data(date_send,1);
    
    if (regval & 0x20)
    {
        if (self->width < self->height)   /* Switch X,Y */
        {
            temp = self->width;
            self->width = self->height;
            self->height = temp;
        }
    }
    else
    {
        if (self->width > self->height)   /* Switch X,Y */
        {
            temp = self->width;
            self->width = self->height;
            self->height = temp;
        }
    }
    
    lcd_set_window(0, 0, self->width, self->height);
}

/**
 * @brief       Set the LCD display direction
 * @param       dir:0,portrait; 1,landscape
 * @retval      None
 */
STATIC void lcd_display_dir(uint8_t dir)
{
    self->dir = dir;
    
    if (self->dir == 0)             /* portrait */
    {
        if (self->lcd == LCD_320X240)
        {
            self->width     = 240;
            self->height    = 320;
        }
        else
        {
            self->width     = 240;
            self->height    = 240;
        }

        self->wramcmd   = 0X2C;
        self->setxcmd   = 0X2A;
        self->setycmd   = 0X2B;
    }
    else                            /* landscape */
    {
        if (self->lcd == LCD_320X240)
        {
            self->width     = 320;
            self->height    = 240;
        }
        else
        {
            self->width     = 240;
            self->height    = 240;
        }

        self->wramcmd   = 0X2C;
        self->setxcmd   = 0X2A;
        self->setycmd   = 0X2B;
    }

    lcd_scan_dir(DFT_SCAN_DIR);
}

STATIC mp_obj_t lcd_hard_reset(void)
{
    if (lcd_self->reset != MP_OBJ_NULL)
    {
        /* 复位显示屏 */
        mp_hal_pin_write(lcd_self->reset, 0);
        mp_hal_delay_ms(100);
        mp_hal_pin_write(lcd_self->reset, 1);
        mp_hal_delay_ms(100);
    }
    
    return mp_const_none;
}

/**
 * @brief       LCD draw point
 * @param       x    : The x-coordinate of the point to be drawn
 *              y    : The y-coordinate of the point to be drawn
 *              color: The color of the dot to be painted
 * @retval      None
 */
STATIC void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_set_cursor(x, y);
    lcd_write_data16(color);
}

STATIC mp_obj_t lcd_pixel(size_t n_args, const mp_obj_t *args)
{
    lcd_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t color = mp_obj_get_int(args[3]);
    
    lcd_draw_pixel( x, y, color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_pixel_obj, 4, 4, lcd_pixel);

/**
 * @brief       LCD draw line
 * @param       x1   : The x-coordinate of end point 1 of the line to be drawn
 *              y1   : The y-coordinate of end point 1 of the line to be drawn
 *              x2   : The x-coordinate of end point 2 of the line to be drawn
 *              y2   : The y-coordinate of end point 2 of the line to be drawn
 *              color: The color of the line to be painted
 * @retval      None
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    uint16_t t; 
    int xerr = 0,yerr = 0,delta_x,delta_y,distance; 
    
    int incx,incy,urow,ucol; 

    delta_x = x2 - x1;
    delta_y = y2 - y1; 
    urow = x1; 
    ucol = y1; 
    
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
        incx =-1;delta_x =-delta_x;
    } 
    if(delta_y > 0)
    {
        incy = 1; 
    }
    else if(delta_y == 0)
    {
        incy = 0;
    }
    else
    {
        incy =-1;
        delta_y=-delta_y;
    } 
    
    if( delta_x>delta_y)
    {
        distance = delta_x;
    }
    else
    {
        distance = delta_y; 
    }
    
    for (t = 0;t <= distance + 1;t++ )
    {
        lcd_draw_pixel(urow,ucol,color);
        xerr += delta_x ; 
        yerr += delta_y ; 
        
        if(xerr>distance)
        { 
            xerr -= distance; 
            urow += incx; 
        } 
        
        if (yerr > distance)
        { 
            yerr -= distance; 
            ucol += incy; 
        } 
    } 
}

STATIC mp_obj_t lcd_line(size_t n_args, const mp_obj_t *args)
{
    lcd_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x0     = mp_obj_get_int(args[1]);
    mp_int_t y0     = mp_obj_get_int(args[2]);
    mp_int_t x1     = mp_obj_get_int(args[3]);
    mp_int_t y1     = mp_obj_get_int(args[3]);
    mp_int_t color  = mp_obj_get_int(args[5]);
    
    lcd_draw_line(x0,y0,x1,y1,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_line_obj, 6, 6, lcd_line);

/**
 * @brief       LCD draw rectangle
 * @param       x1   : The x-coordinate of end point 1 of the rectangle to be drawn
 *              y1   : The y-coordinate of end point 1 of the rectangle to be drawn
 *              x2   : The x-coordinate of end point 2 of the rectangle to be drawn
 *              y2   : The y-coordinate of end point 2 of the rectangle to be drawn
 *              color: The color of the rectangle to be painted
 * @retval      None
 */
void lcd_draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color)
{
    lcd_draw_line(x0, y0, x1, y0,color);
    lcd_draw_line(x0, y0, x0, y1,color);
    lcd_draw_line(x0, y1, x1, y1,color);
    lcd_draw_line(x1, y0, x1, y1,color);
}

STATIC mp_obj_t lcd_rectangle(size_t n_args, const mp_obj_t *args)
{
    lcd_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x0     = mp_obj_get_int(args[1]);
    mp_int_t y0     = mp_obj_get_int(args[2]);
    mp_int_t x1     = mp_obj_get_int(args[3]);
    mp_int_t y1     = mp_obj_get_int(args[3]);
    mp_int_t color  = mp_obj_get_int(args[5]);
    
    lcd_draw_rectangle(x0,y0,x1,y1,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_rectangle_obj, 6, 6, lcd_rectangle);

/**
 * @brief       LCD draw circle
 * @param       x    : The x-coordinate of the origin of the circle to be drawn
 *              y    : The Y coordinate of the origin of the circle to be drawn
 *              r    : The radius of the circle to be drawn
 *              color: The color of the circle to be painted
 * @retval      None
 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);

    while (a <= b)
    {
        lcd_draw_pixel(x0 - b, y0 - a, color);
        lcd_draw_pixel(x0 + b, y0 - a, color);
        lcd_draw_pixel(x0 - a, y0 + b, color);
        lcd_draw_pixel(x0 - b, y0 - a, color);
        lcd_draw_pixel(x0 - a, y0 - b, color);
        lcd_draw_pixel(x0 + b, y0 + a, color);
        lcd_draw_pixel(x0 + a, y0 - b, color);
        lcd_draw_pixel(x0 + a, y0 + b, color);
        lcd_draw_pixel(x0 - b, y0 + a, color);
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

        lcd_draw_pixel(x0 + a, y0 + b, color);
    }
}

STATIC mp_obj_t lcd_circle(size_t n_args, const mp_obj_t *args)
{
    lcd_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x      = mp_obj_get_int(args[1]);
    mp_int_t y      = mp_obj_get_int(args[2]);
    mp_int_t r      = mp_obj_get_int(args[3]);
    mp_int_t color  = mp_obj_get_int(args[4]);
    
    lcd_draw_circle(x,y,r,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_circle_obj, 5, 5, lcd_circle);

/**
 * @brief       The LCD displays 1 character
 * @param       x    : The X coordinate of the character to be displayed
 *              y    : The Y coordinate of the character to be displayed
 *              chr  : Character to be displayed
 *              mode : stacking(1); non-stacking(0)
 *              color: The color of the character to be displayed
 * @retval      None
 */
void lcd_show_char(uint16_t x, uint16_t y, uint8_t chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp = 0,t1 = 0, t = 0;
    uint8_t *pfont = 0;
    uint8_t csize = 0;
    uint16_t colortemp = 0;
    uint8_t sta = 0;
    
    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    chr = chr - ' ';    /* The offset value is obtained(the ASCII library is modded from a space, so - "is the library of the corresponding character) */

    if ((x > (lcd_self->width - size / 2)) || (y > (lcd_self->height - size)))
    {
        return;
    }

    lcd_set_window(x, y, x + size / 2 - 1, y + size - 1);          /* (x,y,x+8-1,y+16-1) */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[chr];
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[chr];
            break;

        default:
            return ;
    }

    if (size != 24)
    {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
        
        for (t = 0; t < csize; t++)
        {
            temp = pfont[t];

            for (t1 = 0; t1 < 8; t1++)
            {
                    if (temp & 0x80)
                    {
                        colortemp = color;
                    }
                    else if (mode == 0)
                    {
                        colortemp = g_back_color;
                    }

                    lcd_write_data16(colortemp);
                    temp <<= 1;
            }
        }
    }
    else
    {
        csize = (size * 16) / 8;
        
        for (t = 0; t < csize; t++)
        {
            temp = asc2_2412[chr][t];

            if (t % 2 == 0)
            {
                sta = 8;
            }
            else
            {
                sta = 4;
            }

            for (t1 = 0; t1 < sta; t1++)
            {
                if(temp & 0x80)
                {
                    colortemp = color;
                }
                else if (mode == 0)
                {
                    colortemp = g_back_color;
                }

                lcd_write_data16(colortemp);
                temp <<= 1;
            }
        }
    }
}

STATIC mp_obj_t lcd_char(size_t n_args, const mp_obj_t *args)
{
    const char *chr;
    
    lcd_obj_t *self     = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x          = mp_obj_get_int(args[1]);
    mp_int_t y          = mp_obj_get_int(args[2]);
    
    chr                 = mp_obj_str_get_str(args[3]);
    
    uint8_t size        = mp_obj_get_int(args[4]);
    uint8_t mode        = mp_obj_get_int(args[5]);
    uint16_t color      = mp_obj_get_int(args[6]);
    
    lcd_show_char(x,y,*chr,size,mode,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_char_obj, 7, 7, lcd_char);

/**
 * @brief       Square function,x^y
 * @param       x: Base number
 *              y: index
 * @retval      x^y
 */
STATIC uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while(n--)result *= m;

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
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2)*t, y, ' ', size, 0, color);
                continue;
            }
            else
            {
                enshow = 1;
            }

        }

        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, 0, color);
    }
}

STATIC mp_obj_t lcd_num(size_t n_args, const mp_obj_t *args)
{
    lcd_obj_t *self     = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x          = mp_obj_get_int(args[1]);
    mp_int_t y          = mp_obj_get_int(args[2]);
    uint32_t num        = mp_obj_get_int(args[3]);
    uint8_t len         = mp_obj_get_int(args[4]);
    uint8_t size        = mp_obj_get_int(args[5]);
    uint16_t color      = mp_obj_get_int(args[6]);
    
    lcd_show_num(x,y,num,len,size,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_num_obj, 7, 7, lcd_num);

/**
 * @brief       LCD show string
 * @param       x    : The x-coordinate of the string to be displayed
 *              y    : The y-coordinate of the string to be displayed
 *              str  : To display string
 *              color: The color of the string to be displayed
 * @retval      None
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
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

        if (y >= height)break;  /* 退出 */

        lcd_show_char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}

STATIC mp_obj_t lcd_string(size_t n_args, const mp_obj_t *args)
{
    lcd_obj_t *self     = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x          = mp_obj_get_int(args[1]);
    mp_int_t y          = mp_obj_get_int(args[2]);
    uint16_t width      = mp_obj_get_int(args[3]);
    uint16_t height     = mp_obj_get_int(args[4]);
    uint8_t size        = mp_obj_get_int(args[5]);
    char *p             = mp_obj_str_get_str(args[6]);
    uint16_t color      = mp_obj_get_int(args[7]);
    
    lcd_show_string(x,y,width,height,size,p,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_string_obj, 8, 8, lcd_string);

/**
 * @brief       LCD area filling
 * @param       xs: region start x coordinate
 *              ys: region start y coordinate
 *              xe: region end x coordinate
 *              ye: region end y coordinate
 *              color: The color of the area to be painted
 * @retval      None
 */
void lcd_fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint16_t i = 0;
    uint32_t size = 0, size_remain = 0;

    size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2;

    if(size > LCD_BUF_SIZE)
    {
        size_remain = size - LCD_BUF_SIZE;
        size = LCD_BUF_SIZE;
    }

    lcd_set_window(x_start, y_start, x_end, y_end);

    while(1)
    {
        for(i = 0; i < size / 2; i++)
        {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color;
        }

        lcd_write_data(lcd_buf, size);

        if(size_remain == 0)
            break;

        if(size_remain > LCD_BUF_SIZE)
        {
            size_remain = size_remain - LCD_BUF_SIZE;
        }

        else
        {
            size = size_remain;
            size_remain = 0;
        }
    }
}
STATIC mp_obj_t mp_lcd_fill(size_t n_args, const mp_obj_t *args)
{
    lcd_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x0     = mp_obj_get_int(args[1]);
    mp_int_t y0     = mp_obj_get_int(args[2]);
    mp_int_t x1     = mp_obj_get_int(args[3]);
    mp_int_t y1     = mp_obj_get_int(args[3]);
    mp_int_t color  = mp_obj_get_int(args[5]);
    
    lcd_fill(x0,y0,x1,y1,color);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_fill_obj, 6, 6, mp_lcd_fill);

/**
 * @brief       Display camera data (RGB565)
 * @param       x:region start x coordinate
 * @param       y:region start y coordinate
 * @retval      None
 */
static void lcd_camera(uint16_t x, uint16_t y)
{
    unsigned long i = 0;
    unsigned long j = 0;
    camera_fb_t *fb = NULL;
    
    while (1)
    {
        fb = esp_camera_fb_get();
        
        lcd_set_window(x, y, x + fb->width - 1, y + fb->height - 1);

        for (j = 0; j < fb->width * fb->height; j++)
        {
            lcd_buf[2 * j] = (fb->buf[2 * i]) ;
            lcd_buf[2 * j + 1] =  (fb->buf[2 * i + 1]);
            i ++;
        }
        
        for(j = 0; j < (fb->width * fb->height * 2 / LCD_BUF_SIZE); j++)
        {
            lcd_write_data(&lcd_buf[j * LCD_BUF_SIZE] , LCD_BUF_SIZE);
        }
        esp_camera_fb_return(fb);
        i = 0;
        fb = NULL;
    }
}
STATIC mp_obj_t mp_lcd_camera(size_t n_args, const mp_obj_t *args)
{
    lcd_obj_t *self     = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x0         = mp_obj_get_int(args[1]);
    mp_int_t y0         = mp_obj_get_int(args[2]);
    lcd_camera(x0,y0);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_camera_obj, 3, 3, mp_lcd_camera);

extern QueueHandle_t xQueueAIFrameO;
camera_fb_t *face_ai_frameO;

/**
    * @brief       Face detection (RGB565)
    * @param       None
    * @retval      None
    */
STATIC void lcd_ai_identify_camera(uint16_t x, uint16_t y)
{
    unsigned long i = 0;
    unsigned long j = 0;
    
    while (1)
    {
        if (xQueueReceive(xQueueAIFrameO, &face_ai_frameO, portMAX_DELAY))
        {
            lcd_set_window(x, y, x + face_ai_frameO->width - 1, y + face_ai_frameO->height - 1);
            
            for (j = 0; j < face_ai_frameO->width * face_ai_frameO->height; j++)
            {
                lcd_buf[2 * j] = (face_ai_frameO->buf[2 * i]) ;
                lcd_buf[2 * j + 1] =  (face_ai_frameO->buf[2 * i + 1]);
                i ++;
            }
            
            for(j = 0; j < (face_ai_frameO->width * face_ai_frameO->height * 2 / LCD_BUF_SIZE); j++)
            {
                lcd_write_data(&lcd_buf[j * LCD_BUF_SIZE] , LCD_BUF_SIZE);
            }

            esp_camera_fb_return(face_ai_frameO);
            i = 0;
            face_ai_frameO = NULL;
        }
    }
}

STATIC mp_obj_t mp_lcd_ai_identify_camera(size_t n_args, const mp_obj_t *args)
{
    lcd_obj_t *self     = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x0         = mp_obj_get_int(args[1]);
    mp_int_t y0         = mp_obj_get_int(args[2]);
    lcd_ai_identify_camera(x0,y0);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_ai_identify_camera_obj, 3, 3, mp_lcd_ai_identify_camera);

/**
 * @brief       Turn on the LCD backlight
 * @param       None
 * @retval      None
 */
STATIC mp_obj_t lcd_on(mp_obj_t self_in)
{
    if (lcd_self->backlight != MP_OBJ_NULL)
    {
        gpio_set_level(lcd_self->backlight, 1);
        mp_hal_delay_ms(10);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(lcd_on_obj, lcd_on);

/**
 * @brief       Turn off the LCD backlight
 * @param       None
 * @retval      None
 */
STATIC mp_obj_t lcd_off(mp_obj_t self_in)
{
    if (lcd_self->backlight != MP_OBJ_NULL)
    {
        mp_hal_pin_write(lcd_self->backlight, 0);
        mp_hal_delay_ms(10);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(lcd_off_obj, lcd_off);


STATIC void lcd_dv_init(void)
{
    /* reset */
    lcd_hard_reset();
    mp_hal_delay_ms(10);

    if (self->lcd == LCD_320X240)
    {
        lcd_write_cmd(0x11);
        lcd_write_cmd(0x36);
        lcd_spi_write_data(0x00);
        lcd_write_cmd(0x3A);
        lcd_spi_write_data(0x65);
        lcd_write_cmd(0xB2);
        lcd_spi_write_data(0x0C);
        lcd_spi_write_data(0x0C);
        lcd_spi_write_data(0x00);
        lcd_spi_write_data(0x33);
        lcd_spi_write_data(0x33);
        lcd_write_cmd(0xB7);
        lcd_spi_write_data(0x75);
        lcd_write_cmd(0xBB);
        lcd_spi_write_data(0x1C);
        lcd_write_cmd(0xC0);
        lcd_spi_write_data(0x2c);
        lcd_write_cmd(0xC2);
        lcd_spi_write_data(0x01);
        lcd_write_cmd(0xC3);
        lcd_spi_write_data(0x0F);
        lcd_write_cmd(0xC4);
        lcd_spi_write_data(0x20);
        lcd_write_cmd(0xC6);
        lcd_spi_write_data(0X01);
        lcd_write_cmd(0xD0);
        lcd_spi_write_data(0xA4);
        lcd_spi_write_data(0xA1);
        lcd_write_cmd(0xE0);
        lcd_spi_write_data(0xD0);
        lcd_spi_write_data(0x04);
        lcd_spi_write_data(0x0D);
        lcd_spi_write_data(0x11);
        lcd_spi_write_data(0x13);
        lcd_spi_write_data(0x2B);
        lcd_spi_write_data(0x3F);
        lcd_spi_write_data(0x54);
        lcd_spi_write_data(0x4C);
        lcd_spi_write_data(0x18);
        lcd_spi_write_data(0x0D);
        lcd_spi_write_data(0x0B);
        lcd_spi_write_data(0x1F);
        lcd_spi_write_data(0x23);
        lcd_write_cmd(0XE1);
        lcd_spi_write_data(0xD0);
        lcd_spi_write_data(0x04);
        lcd_spi_write_data(0x0C);
        lcd_spi_write_data(0x11);
        lcd_spi_write_data(0x13);
        lcd_spi_write_data(0x2C);
        lcd_spi_write_data(0x3F);
        lcd_spi_write_data(0x44);
        lcd_spi_write_data(0x51);
        lcd_spi_write_data(0x2F);
        lcd_spi_write_data(0x1F);
        lcd_spi_write_data(0x1F);
        lcd_spi_write_data(0x20);
        lcd_spi_write_data(0x23);
        lcd_write_cmd(0x20);
        mp_hal_delay_ms(120);
        lcd_write_cmd(0x29);
    }
    else
    {
        /* Sleep Out */
        lcd_write_cmd(0x11);
        /* wait for power stability */
        mp_hal_delay_ms(120);

        /* Memory Data Access Control */
        lcd_write_cmd(0x36);
        lcd_spi_write_data(0x00);

        /* RGB 5-6-5-bit  */
        lcd_write_cmd(0x3A);
        lcd_spi_write_data(0x65);

        /* Porch Setting */
        lcd_write_cmd(0xB2);
        lcd_spi_write_data(0x0C);
        lcd_spi_write_data(0x0C);
        lcd_spi_write_data(0x00);
        lcd_spi_write_data(0x33);
        lcd_spi_write_data(0x33);

        /*  Gate Control */
        lcd_write_cmd(0xB7);
        lcd_spi_write_data(0x72);

        /* VCOM Setting */
        lcd_write_cmd(0xBB);
        lcd_spi_write_data(0x3D);

        /* LCM Control */
        lcd_write_cmd(0xC0);
        lcd_spi_write_data(0x2C);

        /* VDV and VRH Command Enable */
        lcd_write_cmd(0xC2);
        lcd_spi_write_data(0x01);

        /* VRH Set */
        lcd_write_cmd(0xC3);
        lcd_spi_write_data(0x19);

        /* VDV Set */
        lcd_write_cmd(0xC4);
        lcd_spi_write_data(0x20);

        /* Frame Rate Control in Normal Mode */
        lcd_write_cmd(0xC6);
        lcd_spi_write_data(0x0F);

        /* Power Control 1 */
        lcd_write_cmd(0xD0);
        lcd_spi_write_data(0xA4);
        lcd_spi_write_data(0xA1);

        /* Positive Voltage Gamma Control */
        lcd_write_cmd(0xE0);
        lcd_spi_write_data(0xD0);
        lcd_spi_write_data(0x04);
        lcd_spi_write_data(0x0D);
        lcd_spi_write_data(0x11);
        lcd_spi_write_data(0x13);
        lcd_spi_write_data(0x2B);
        lcd_spi_write_data(0x3F);
        lcd_spi_write_data(0x54);
        lcd_spi_write_data(0x4C);
        lcd_spi_write_data(0x18);
        lcd_spi_write_data(0x0D);
        lcd_spi_write_data(0x0B);
        lcd_spi_write_data(0x1F);
        lcd_spi_write_data(0x23);

        /* Negative Voltage Gamma Control */
        lcd_write_cmd(0xE1);
        lcd_spi_write_data(0xD0);
        lcd_spi_write_data(0x04);
        lcd_spi_write_data(0x0C);
        lcd_spi_write_data(0x11);
        lcd_spi_write_data(0x13);
        lcd_spi_write_data(0x2C);
        lcd_spi_write_data(0x3F);
        lcd_spi_write_data(0x44);
        lcd_spi_write_data(0x51);
        lcd_spi_write_data(0x2F);
        lcd_spi_write_data(0x1F);
        lcd_spi_write_data(0x1F);
        lcd_spi_write_data(0x20);
        lcd_spi_write_data(0x23);

        /* Display Inversion On */
        lcd_write_cmd(0x21);

        lcd_write_cmd(0x29);
    }

    lcd_display_dir(self->dir);

    lcd_clear(WHITE);
    
    if (lcd_self->backlight != MP_OBJ_NULL)
    {
        mp_hal_pin_write(lcd_self->backlight, 1);
    }
}

/* LCD函数声明表 */
STATIC const mp_rom_map_elem_t lcd_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on),          MP_ROM_PTR(&lcd_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),         MP_ROM_PTR(&lcd_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel),       MP_ROM_PTR(&lcd_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_line),        MP_ROM_PTR(&lcd_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_rectangle),   MP_ROM_PTR(&lcd_rectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle),      MP_ROM_PTR(&lcd_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_char),        MP_ROM_PTR(&lcd_char_obj) },
    { MP_ROM_QSTR(MP_QSTR_num),         MP_ROM_PTR(&lcd_num_obj) },
    { MP_ROM_QSTR(MP_QSTR_string),      MP_ROM_PTR(&lcd_string_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear),       MP_ROM_PTR(&lcd_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill),        MP_ROM_PTR(&lcd_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_camera),      MP_ROM_PTR(&lcd_camera_obj) },
    { MP_ROM_QSTR(MP_QSTR_ai_identify_camerashow), MP_ROM_PTR(&lcd_ai_identify_camera_obj) },
};

STATIC MP_DEFINE_CONST_DICT(lcd_locals_dict, lcd_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    lcd_type,
    MP_QSTR_init,
    MP_TYPE_FLAG_NONE,
    make_new, lcd_make_new,
    locals_dict, &lcd_locals_dict
    );

mp_obj_t lcd_make_new(const mp_obj_type_t *type,size_t n_args,size_t n_kw,const mp_obj_t *all_args )
{
    enum
    {
        ARG_spi, ARG_reset, ARG_dc, ARG_cs,
        ARG_backlight, ARG_dir,ARG_lcd,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_reset, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_dc, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_cs, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_backlight, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_dir, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_lcd, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
        
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(lcd_obj_t);
    self->base.type = &lcd_type;

    mp_obj_base_t *spi_obj 	= (mp_obj_base_t*)MP_OBJ_TO_PTR(args[ARG_spi].u_obj);
    self->spi_obj           = spi_obj;
    self->dir               = args[ARG_dir].u_int;
    self->lcd               = args[ARG_lcd].u_int;
    
    if (self->dir != 1 && self->dir != 0)
    {
        mp_raise_ValueError(MP_ERROR_TEXT("Not horizontal and vertical screens"));
    }

    if (self->lcd != 1 && self->lcd != 0)
    {
        mp_raise_ValueError(MP_ERROR_TEXT("Only supports 320x240(0) and 240x240(1)"));
    }

    if (args[ARG_dc].u_obj == MP_OBJ_NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("must specify dc pin"));
    }

    if (args[ARG_backlight].u_obj != MP_OBJ_NULL) {
        self->backlight = mp_hal_get_pin_obj(args[ARG_backlight].u_obj);
    }

    if (args[ARG_reset].u_obj != MP_OBJ_NULL) {
        self->reset = mp_hal_get_pin_obj(args[ARG_reset].u_obj);
    }

    self->dc = mp_hal_get_pin_obj(args[ARG_dc].u_obj);

    if (args[ARG_cs].u_obj != MP_OBJ_NULL) {
        self->cs = mp_hal_get_pin_obj(args[ARG_cs].u_obj);
    }

    if (args[ARG_backlight].u_obj != MP_OBJ_NULL) {
        self->backlight = mp_hal_get_pin_obj(args[ARG_backlight].u_obj);
    }

    lcd_dv_init();
    
    return MP_OBJ_FROM_PTR(self);
}

STATIC uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

STATIC mp_obj_t lcd_color565(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
    return MP_OBJ_NEW_SMALL_INT(color565(
        (uint8_t)mp_obj_get_int(r),
        (uint8_t)mp_obj_get_int(g),
        (uint8_t)mp_obj_get_int(b)
    ));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(lcd_color565_obj, lcd_color565);

STATIC const mp_map_elem_t lcd_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_atk_lcd) },
    { MP_ROM_QSTR(MP_QSTR_color565), (mp_obj_t)&lcd_color565_obj },
    { MP_ROM_QSTR(MP_QSTR_init), (mp_obj_t)&lcd_type },
    { MP_ROM_QSTR(MP_QSTR_BLACK), MP_ROM_INT(BLACK) },
    { MP_ROM_QSTR(MP_QSTR_BLUE), MP_ROM_INT(BLUE) },
    { MP_ROM_QSTR(MP_QSTR_RED), MP_ROM_INT(RED) },
    { MP_ROM_QSTR(MP_QSTR_GREEN), MP_ROM_INT(GREEN) },
    { MP_ROM_QSTR(MP_QSTR_CYAN), MP_ROM_INT(CYAN) },
    { MP_ROM_QSTR(MP_QSTR_MAGENTA), MP_ROM_INT(MAGENTA) },
    { MP_ROM_QSTR(MP_QSTR_YELLOW), MP_ROM_INT(YELLOW) },
    { MP_ROM_QSTR(MP_QSTR_WHITE), MP_ROM_INT(WHITE) },
};

STATIC MP_DEFINE_CONST_DICT (mp_module_lcd_globals, lcd_module_globals_table );

const mp_obj_module_t mp_module_lcd = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_lcd_globals,
};

MP_REGISTER_MODULE(MP_QSTR_atk_lcd, mp_module_lcd);

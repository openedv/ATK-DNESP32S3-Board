/**
 ****************************************************************************************************
 * @file        text.c
 * @author      ALIENTEK
 * @brief       TEXT code
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

#include "text.h"


/**
 * @brief       Obtain Chinese character lattice data
 * @param       code  : Current Chinese character encoding (GBK code)
 * @param       mat   : Current Chinese character dot matrix data storage address
 * @param       size  : Font size
 * @retval      None
 */
static void text_get_hz_mat(unsigned char *code, unsigned char *mat, uint8_t size)
{
    unsigned char qh, ql;
    unsigned char i;
    unsigned long foffset;
    uint8_t csize;
    
    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size);
    qh = *code;
    ql = *(++code);
    if ((qh < 0x81) || (ql < 0x40) || (ql == 0xFF) || (qh == 0xFF))
    {
        for (i = 0; i < csize; i++)
        {
            *mat++ = 0x00;
        }
        return;
    }
    
    if (ql < 0x7F)
    {
        ql -= 0x40;
    }
    else
    {
        ql -= 0x41;
    }
    
    qh -= 0x81;
    foffset = ((unsigned long)190 * qh + ql) * csize;
    
    switch (size)
    {
        case 12:
        {
            fonts_partition_read(mat, foffset + ftinfo.f12addr, csize);
            break;
        }
        case 16:
        {
            fonts_partition_read(mat, foffset + ftinfo.f16addr, csize);
            break;
        }
        case 24:
        {
            fonts_partition_read(mat, foffset + ftinfo.f24addr, csize);
            break;
        }
    }
}

/**
 * @brief       Display a Chinese character of a specified size
 * @param       x       : The x-coordinate of the string to be displayed
 * @param       y       : The y-coordinate of the string to be displayed
 * @param       font    : Chinese GBK code
 * @param       size    : Font size
 * @param       mode    : Display mode
 *   @note              0:Normal display
 *   @note              1:Overlay display
 * @param       color   : Font Color
 * @retval      None
 */
void text_show_font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t, t1;
    uint16_t y0 = y;
    uint8_t *dzk;
    uint8_t csize;
    uint8_t font_size = size;
    
    csize = (font_size / 8 + ((font_size % 8) ? 1 : 0)) * (font_size);
    
    if ((font_size != 12) && (font_size != 16) && (font_size != 24))
    {
        return;
    }
    
    dzk = (uint8_t *)malloc(font_size * 5);
    
    if (dzk == NULL)
    {
        return;
    }
    
    text_get_hz_mat(font, dzk, font_size);
    
    for (t = 0; t < csize; t++)
    {
        temp = dzk[t];
        
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
            {
                lcd_draw_pixel(x, y, color);
            }
            else if (mode == 0)
            {
                lcd_draw_pixel(x, y, 0xffff);
            }
            
            temp <<= 1;
            y++;
            if ((y - y0) == font_size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
    
    free(dzk);
}

/**
 * @brief       Start displaying a string at the specified location
 * @param       x       : The x-coordinate of the string to be displayed
 * @param       y       : The y-coordinate of the string to be displayed
 * @param       width   : Display width
 * @param       height  : Display height
 * @param       str     : String
 * @param       size    : Font size
 * @param       mode    : Display mode
 *   @note              0:Normal display
 *   @note              1:Overlay display
 * @param       color : 字体颜色
 * @retval      None
 */
void text_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, uint8_t size, uint8_t mode, uint16_t color)
{
    uint16_t x0 = x;
    uint16_t y0 = y;
    uint8_t bHz = 0;
    char in[100];
    char *pout;
    strcpy(in, str);
    convet_utf8_to_gbk(&pout, in, strlen(in));                      /* utf8 to gbk */
    uint8_t *pstr = (uint8_t *)pout;

    while (*pstr != 0)
    {
        if (!bHz)
        {
            if (*pstr > 0x80)
            {
                bHz = 1;
            }
            else
            {
                if (x > (x0 + width - size / 2))
                {
                    y += size;
                    x = x0;
                }
                
                if (y > (y0 + height - size))
                {
                    break;
                }
                
                if (*pstr == 13)
                {
                    y += size;
                    x = x0;
                    pstr++;
                }
                else
                {
                    lcd_show_char(x, y, *pstr, size, mode, color);
                }
                
                pstr++;
                x += size / 2;
            }
        }
        else
        {
            bHz = 0;
            
            if (x > (x0 + width - size))
            {
                y += size;
                x = x0;
            }
            
            if (y > (y0 + height - size))
            {
                break;
            }
            
            text_show_font(x, y, pstr, size, mode, color);
            pstr += 2;
            x += size;
        }
    }

    free(pout);
}

/**
 * @brief       Display a string in the middle of the specified width
 * @param       x       : The x-coordinate of the string to be displayed
 * @param       y       : The y-coordinate of the string to be displayed
 * @param       str     : String
 * @param       size    : Font size
 * @param       width   : Display width
 * @param       color   : Font color
 * @retval      None
 */
void text_show_string_middle(uint16_t x, uint16_t y, char *str, uint8_t size, uint16_t width, uint16_t color)
{
    uint16_t strlenth = 0;
    strlenth = strlen((const char *)str);
    strlenth *= size / 2;

    if (strlenth > width)
    {
        text_show_string(x, y, lcd_self.width, lcd_self.height, str, size, 1, color);
    }
    else
    {
        strlenth = (width - strlenth) / 2;
        text_show_string(strlenth + x, y, lcd_self.width, lcd_self.height, str, size, 1, color);
    }
}

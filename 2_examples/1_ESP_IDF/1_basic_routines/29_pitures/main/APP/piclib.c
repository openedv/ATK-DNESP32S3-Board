/**
 ****************************************************************************************************
 * @file        piclib.c
 * @author      ALIENTEK
 * @brief       PICLIB code
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

#include "piclib.h"


_pic_info picinfo;
_pic_phy pic_phy;


/**
 * @brief       Multi point filling
 * @param       x       : The x-coordinate of the string to be displayed
 * @param       y       : The y-coordinate of the string to be displayed
 * @param       width         : Display width
 * @param       height        : Display height
 * @param       color         : point color
 * @retval      None
 */
static void piclib_multi_color(uint16_t x, uint16_t y, uint16_t size, uint16_t *color)
{
    static uint8_t Byte[1024];
    int index = 0;
    uint16_t _x1 = x;
    uint16_t _x2 = _x1 + (size - 1);
    uint16_t _y1 = y;
    uint16_t _y2 = _y1;

    lcd_set_window(_x1, _y1, _x2, _y2);

    for (int i = 0;i < size;i++)
    {
        Byte[index++] = (color[i] >> 8) & 0xFF;
        Byte[index++] = color[i] & 0xFF;
    }

    lcd_write_data(Byte, size * 2);
}

/**
 * @brief       Initialize piclib
 * @param       None
 * @retval      None
 */
void piclib_init(void)
{
    pic_phy.draw_point = lcd_draw_pixel;
    pic_phy.fill = lcd_fill;
    pic_phy.draw_hline = lcd_draw_hline;
    pic_phy.multicolor = piclib_multi_color;

    picinfo.lcdwidth = lcd_self.width;
    picinfo.lcdheight = lcd_self.height;
}

/**
 * @brief       Draw a picture
 *
 * @param       filename      : Image file path(.bmp/.jpg/.jpeg/.gif/.png etc)
 * @param       x       : The x-coordinate of the string to be displayed
 * @param       y       : The y-coordinate of the string to be displayed
 * @param       width         : Display width
 * @param       height        : Display height
 * @retval      None
 */
uint8_t piclib_ai_load_picfile(char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint8_t	res = 0;
    uint8_t temp;

    if ((x + width) > picinfo.lcdwidth)return PIC_WINDOW_ERR;

    if ((y + height) > picinfo.lcdheight)return PIC_WINDOW_ERR;

    if (width == 0 || height == 0)return PIC_WINDOW_ERR;

    /* Detect the type of file */
    temp = exfuns_file_type(filename);

    switch (temp)
    {
        case T_BMP:
            res = bmp_decode(filename,width, height);           /* Decoding BMP */
            break;

        case T_JPG:
        case T_JPEG:
            res = jpeg_decode(filename,width, height);          /* Decoding JPG/JPEG */
            break;

        case T_GIF:
            res = gif_decode(filename, x, y, width, height);    /* Decoding gif */
            break;

        case T_PNG:
            res = png_decode(filename, width, height);          /* Decoding PNG */
            break;

        default:
            res = PIC_FORMAT_ERR;                               /* Non image format */
            break;
    }

    return res;
}

/**
 * @brief       Dynamically allocate memory
 * @param       size          : Memory size to be applied for (in bytes)
 * @retval      The first address of memory allocated
 */
void *piclib_mem_malloc (uint32_t size)
{
    return (void *)malloc(size);
}

/**
 * @brief       Release memory
 * @param       paddr         : Memory First Address
 * @retval      None
 */
void piclib_mem_free (void *paddr)
{
    free(paddr);
}

/**
 ****************************************************************************************************
 * @file        lcd.c
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


#define SPI_LCD_TYPE    1

spi_device_handle_t MY_LCD_Handle;
uint8_t lcd_buf[LCD_TOTAL_BUF_SIZE];
lcd_obj_t lcd_self;


/*
 The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
typedef struct
{
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes;
} lcd_init_cmd_t;

/**
 * @brief       write command to LCD
 * @param       cmd:command
 * @retval      None
 */
void lcd_write_cmd(const uint8_t cmd)
{
    LCD_WR(0);
    spi2_write_cmd(MY_LCD_Handle, cmd);
}

/**
 * @brief       write data to LCD
 * @param       data:8 bit data
 * @param       len :lengit
 * @retval      None
 */
void lcd_write_data(const uint8_t *data, int len)
{
    LCD_WR(1);
    spi2_write_data(MY_LCD_Handle, data, len);
}

/**
 * @brief       write a data to LCD
 * @param       data:16 bit data
 * @retval      None
 */
void lcd_write_data16(uint16_t data)
{
    uint8_t dataBuf[2] = {0,0};
    dataBuf[0] = data >> 8;
    dataBuf[1] = data & 0xFF;
    LCD_WR(1);
    spi2_write_data(MY_LCD_Handle, dataBuf,2);
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
    lcd_write_cmd(lcd_self.setxcmd);
    lcd_write_data(databuf,4);

    databuf[0] = ystar >> 8;
    databuf[1] = 0xFF & ystar;
    databuf[2] = yend >> 8;
    databuf[3] = 0xFF & yend;
    lcd_write_cmd(lcd_self.setycmd);
    lcd_write_data(databuf,4);

    lcd_write_cmd(lcd_self.wramcmd);
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
    
    lcd_set_window(0, 0, lcd_self.width - 1, lcd_self.height - 1);

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

/**
 * @brief       LCD area filling
 * @param       xs: region start x coordinate
 *              ys: region start y coordinate
 *              xe: region end x coordinate
 *              ye: region end y coordinate
 *              color: The color of the area to be painted
 * @retval      None
 */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i;
    uint16_t j;
    uint16_t width;
    uint16_t height;

    width = ex - sx + 1;
    height = ey - sy + 1;
    lcd_set_window(sx, sy, ex, ey);

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            lcd_write_data16(color);
        }
    }
    lcd_set_window(sx, sy, ex, ey);
}

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
void lcd_scan_dir(uint8_t dir)
{
    uint8_t regval = 0;
    uint8_t dirreg = 0;
    uint16_t temp;

    if (lcd_self.dir == 1)
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
        if (lcd_self.width < lcd_self.height)   /* Switch X,Y */
        {
            temp = lcd_self.width;
            lcd_self.width = lcd_self.height;
            lcd_self.height = temp;
        }
    }
    else
    {
        if (lcd_self.width > lcd_self.height)   /* Switch X,Y */
        {
            temp = lcd_self.width;
            lcd_self.width = lcd_self.height;
            lcd_self.height = temp;
        }
    }
    
    lcd_set_window(0, 0, lcd_self.width,lcd_self.height);
}

/**
 * @brief       Set the LCD display direction
 * @param       dir:0,portrait; 1,landscape
 * @retval      None
 */
void lcd_display_dir(uint8_t dir)
{
    lcd_self.dir = dir;
    
    if (lcd_self.dir == 0)                  /* portrait */
    {
        lcd_self.width      = 240;
        lcd_self.height     = 320;
        lcd_self.wramcmd    = 0X2C;
        lcd_self.setxcmd    = 0X2A;
        lcd_self.setycmd    = 0X2B;
    }
    else                                    /* landscape */
    {
        lcd_self.width      = 320;
        lcd_self.height     = 240;
        lcd_self.wramcmd    = 0X2C;
        lcd_self.setxcmd    = 0X2A;
        lcd_self.setycmd    = 0X2B;
    }

    lcd_scan_dir(DFT_SCAN_DIR);
}

/**
 * @brief       HW reset
 * @param       None
 * @retval      None
 */
void lcd_hard_reset(void)
{
    LCD_RST(0);
    vTaskDelay(100);
    LCD_RST(1);
    vTaskDelay(100);
}

/**
 * @brief       LCD draw point
 * @param       x    : The x-coordinate of the point to be drawn
 * @param       y    : The y-coordinate of the point to be drawn
 * @param       color: The color of the dot to be painted
 * @retval      None
 */
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_set_cursor(x, y);
    lcd_write_data16(color);
}

/**
 * @brief       LCD draw line
 * @param       x1   : The x-coordinate of end point 1 of the line to be drawn
 * @param       y1   : The y-coordinate of end point 1 of the line to be drawn
 * @param       x2   : The x-coordinate of end point 2 of the line to be drawn
 * @param       y2   : The y-coordinate of end point 2 of the line to be drawn
 * @param       color: The color of the line to be painted
 * @retval      None
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t; 
    int xerr = 0, yerr = 0, delta_x, delta_y, distance; 
    
    int incx, incy, urow, ucol; 

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
        incx =-1;
        delta_x =-delta_x;
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

/**
 * @brief       Draw a horizontal line
 * @param       x0   : The x-coordinate of end point 1 of the rectangle to be drawn
 * @param       y0   : The y-coordinate of end point 1 of the rectangle to be drawn
 * @param       len  : lenght
 * @param       color: color
 * @retval      None
 */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > lcd_self.width) || (y > lcd_self.height))return;

    lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * @brief       LCD draw rectangle
 * @param       x1   : The x-coordinate of end point 1 of the rectangle to be drawn
 * @param       y1   : The y-coordinate of end point 1 of the rectangle to be drawn
 * @param       x2   : The x-coordinate of end point 2 of the rectangle to be drawn
 * @param       y2   : The y-coordinate of end point 2 of the rectangle to be drawn
 * @param       color: The color of the rectangle to be painted
 * @retval      None
 */
void lcd_draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color)
{
    lcd_draw_line(x0, y0, x1, y0,color);
    lcd_draw_line(x0, y0, x0, y1,color);
    lcd_draw_line(x0, y1, x1, y1,color);
    lcd_draw_line(x1, y0, x1, y1,color);
}

/**
 * @brief       LCD draw circle
 * @param       x    : The x-coordinate of the origin of the circle to be drawn
 * @param       y    : The Y coordinate of the origin of the circle to be drawn
 * @param       r    : The radius of the circle to be drawn
 * @param       color: The color of the circle to be painted
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

/**
 * @brief       The LCD displays 1 character
 * @param       x    : The X coordinate of the character to be displayed
 * @param       y    : The Y coordinate of the character to be displayed
 * @param       chr  : Character to be displayed
 * @param       size : Font size
 * @param       mode : stacking(1); non-stacking(0)
 * @param       color: The color of the character to be displayed
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
    chr = chr - ' ';

    if ((x > (lcd_self.width - size / 2)) || (y > (lcd_self.height - size)))
    {
        return;
    }

    lcd_set_window(x, y, x + size / 2 - 1, y + size - 1);   /* (x,y,x+8-1,y+16-1) */

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
                        colortemp = BLACK;
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
                    colortemp = BLACK;
                }

                lcd_write_data16(colortemp);
                temp <<= 1;
            }
        }
    }
}

/**
 * @brief       Square function,x^y
 * @param       x: Base number
 * @param       y: index
 * @retval      x^y
 */
uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while(n--)result *= m;

    return result;
}

/**
 * @brief       LCD displays numbers and does not display high zeros
 * @param       x    : The X coordinate of the number to be displayed
 * @param       y    : The Y coordinate of the number to be displayed
 * @param       num  : To display numbers
 * @param       len  : The number of digits to be displayed
 * @param       color: The color of the numbers to be displayed
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

/**
 * @brief       LCD displays numbers and can control the display of high 0
 * @param       x    : The X coordinate of the number to be displayed
 * @param       y    : The Y coordinate of the number to be displayed
 * @param       num  : To display numbers
 * @param       len  : The number of digits to be displayed
 * @param       size : Font size
 * @param       mode : NUM_SHOW_NOZERO: The high digit 0 is not displayed
 *                     NUM_SHOW_ZERO  : the high digit 0 display
 * @param       color: The color of the numbers to be displayed
 * @retval      None
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
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
                if (mode & 0X80)
                {
                    lcd_show_char(x + (size / 2)*t, y, '0', size, mode & 0X01, color);
                }
                else
                {
                    lcd_show_char(x + (size / 2)*t, y, ' ', size, mode & 0X01, color);
                }
                continue;
            }
            else
            {
                enshow = 1;
            }
        }
        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, mode & 0X01, color);
    }
}

/**
 * @brief       LCD show string
 * @param       x      : The x-coordinate of the string to be displayed
 * @param       y      : The y-coordinate of the string to be displayed
 * @param       width  : Font width
 * @param       height : Font height
 * @param       size   : Font size
 * @param       *p     : To display string
 * @param       color  : The color of the string to be displayed
 * @retval      None
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* 判断是不是非法字符! */
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

/**
 * @brief       color fill
 * @param       sx   : The x-coordinate of end point 1 of the rectangle to be drawn
 * @param       sy   : The y-coordinate of end point 1 of the rectangle to be drawn
 * @param       ex   : The x-coordinate of end point 2 of the rectangle to be drawn
 * @param       ey   : The y-coordinate of end point 2 of the rectangle to be drawn
 * @param       color: The color of the rectangle to be painted
 * @retval      None
 */
void lcd_color_fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
    uint16_t i = 0;
    uint32_t size = 0, size_remain = 0;

    size = (ex - sx + 1) * (ey - sy + 1) * 2;

    if(size > LCD_BUF_SIZE)
    {
        size_remain = size - LCD_BUF_SIZE;
        size = LCD_BUF_SIZE;
    }

    lcd_set_window(sx, sy, ex, ey);

    while(1)
    {
        for (i = 0; i < size / 2; i++)
        {
            lcd_buf[2 * i] = color[i] >> 8;
            lcd_buf[2 * i + 1] = color[i];
        }

        lcd_write_data(lcd_buf, size);

        if (size_remain == 0)
            break;

        if (size_remain > LCD_BUF_SIZE)
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

/**
 * @brief       show icos
 * @param       x       : The x-coordinate of the string to be displayed
 * @param       y       : The y-coordinate of the string to be displayed
 * @param       width   : icos width
 * @param       height  : icos height
 * @param       icosbase:icos base
 * @param       color   : icos color
 * @param       bkcolor :icos bk color
 * @retval      None
 */
void lcd_app_show_mono_icos(uint16_t x,uint16_t y,uint8_t width,uint8_t height,uint8_t *icosbase,uint16_t color,uint16_t bkcolor)
{
    uint16_t rsize;
    uint16_t i,j;
    uint8_t temp;
    uint8_t t = 0;
    uint16_t x0 = x;
    rsize = width / 8 + ((width % 8) ? 1 : 0);

    for (i = 0;i < rsize * height;i++)
    {
        temp = icosbase[i];

        for(j = 0;j < 8;j++)
        {
            if (temp & 0x80)
            {
                lcd_draw_pixel(x,y,color);
            }
            else
            {
                lcd_draw_pixel(x,y,bkcolor);
            }

            temp <<= 1;
            x++;
            t++;

            if (t == width)
            {
                t=0;
                x=x0;
                y++;
                break;
            }
        }
    }
}

/**
 * @brief       Enable backlight
 * @param       None
 * @retval      None
 */
void lcd_on(void)
{
    LCD_PWR(1);
    vTaskDelay(10);
}

/**
 * @brief       Disable backlight
 * @param       None
 * @retval      None
 */
void lcd_off(void)
{
    LCD_PWR(0);
    vTaskDelay(10);
}

/**
 * @brief       Initialize the display
 * @param       None
 * @retval      None
 */
void lcd_init(void)
{
    int cmd = 0;
    esp_err_t ret = 0;
    
    lcd_self.dir = 0;
    lcd_self.wr = LCD_NUM_WR;
    lcd_self.cs = LCD_NUM_CS;
    
    gpio_config_t gpio_init_struct;

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 60 * 1000 * 1000,                         /* clock out at 60 MHz */
        .mode = 0,                                                  /* SPI mode 0 */
        .spics_io_num = lcd_self.cs,                                /* CS pin */
        .queue_size = 7,                                            /* we want to be able to queue 7 transactions at a time */
    };
    
    /* attach the LCD to the SPI bus */
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &MY_LCD_Handle);
    ESP_ERROR_CHECK(ret);

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;                 /* disable interrupt */
    gpio_init_struct.mode = GPIO_MODE_OUTPUT;                       /* set as output mode */
    gpio_init_struct.pin_bit_mask = 1ull << lcd_self.wr;            /* bit mask of the pins that you want to set,e.g.GPIO1/2 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;          /* disable pull-down mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;               /* enable pull-up mode */
    gpio_config(&gpio_init_struct);

    lcd_hard_reset();

#if SPI_LCD_TYPE
    lcd_init_cmd_t ili_init_cmds[] =
    {
        {0x11, {0}, 0x80},
        {0x36, {0x00}, 1},
        {0x3A, {0x65}, 1},
        {0X21, {0}, 0x80},
        {0x29, {0}, 0x80},
        {0, {0}, 0xff},
    };

#else
    lcd_init_cmd_t ili_init_cmds[] =
    {
        {0x11, {0}, 0x80},
        {0x36, {0x00}, 1},
        {0x3A, {0x65}, 1},
        {0xB2, {0x0C, 0x0C, 0x00, 0x33,0x33}, 5},
        {0xB7, {0x75}, 1},
        {0xBB, {0x1C}, 1},
        {0xC0, {0x2c}, 1},
        {0xC2, {0x01}, 1},
        {0xC3, {0x0F}, 1},
        {0xC4, {0x20}, 1},
        {0xC6, {0X01}, 1},
        {0xD0, {0xA4,0xA1}, 2},
        {0xE0, {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23}, 14},
        {0xE1, {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23}, 14},
        {0X21, {0}, 0x80},
        {0x29, {0}, 0x80},
        {0, {0}, 0xff},
    };
#endif

    /* send all the commands */
    while (ili_init_cmds[cmd].databytes != 0xff)
    {
        lcd_write_cmd(ili_init_cmds[cmd].cmd);
        lcd_write_data(ili_init_cmds[cmd].data, ili_init_cmds[cmd].databytes & 0x1F);
        
        if (ili_init_cmds[cmd].databytes & 0x80)
        {
            vTaskDelay(120);
        }
        
        cmd++;
    }

    lcd_display_dir(1);
    /* enable backlight */
    LCD_PWR(1);
    lcd_clear(WHITE);
}

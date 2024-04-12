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

#include "ltdc.h"
#include "ltdcfont.h"


static const char *TAG = "ltdc";
esp_lcd_panel_handle_t panel_handle = NULL;                         /* type of LCD panel handle */
static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;
uint32_t g_back_color  = 0xFFFF;                                    /* background color */
_ltdc_dev ltdcdev;

/**
 * @brief       read ID
 * @note        using the highest bit (R7, G7, B7) of LCD RGB lines to identify panel IDs
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
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* disable interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT;                /* set as intput/output mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* enable pull-up mode */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* disable pull-down mode */
    gpio_init_struct.pin_bit_mask = 1ull << GPIO_LCD_B7;    /* bit mask of the pins that you want to set,e.g.GPIO1/2 */
    gpio_config(&gpio_init_struct);

    gpio_init_struct.pin_bit_mask = 1ull << GPIO_LCD_R7 || 1ull << GPIO_LCD_G7;
    gpio_config(&gpio_init_struct);

    idx  = (uint8_t)gpio_get_level(GPIO_LCD_R7);            /* read M0 */
    idx |= (uint8_t)gpio_get_level(GPIO_LCD_G7) << 1;       /* read M1 */
    idx |= (uint8_t)gpio_get_level(GPIO_LCD_B7) << 2;       /* read M2 */

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
 * @brief       configure rgblcd
 * @param       None
 * @retval      None
 */
void ltdc_init(void)
{
    panel_handle = NULL;
    ltdcdev.id = ltdc_panelid_read();

    /* The following parameters should refer to LCD spec */
    if (ltdcdev.id == 0X4342)
    {
        ltdcdev.pwidth = 480;
        ltdcdev.pheight = 272;
        ltdcdev.hsw = 1;
        ltdcdev.vsw = 1;
        ltdcdev.hbp = 40;
        ltdcdev.vbp = 8;
        ltdcdev.hfp = 5;
        ltdcdev.vfp = 8;
        ltdcdev.pclk_hz = 9 * 1000 * 1000;
    }
    else if (ltdcdev.id == 0X4384)
    {
        ltdcdev.pwidth = 800;
        ltdcdev.pheight = 480;
        ltdcdev.hbp = 88;
        ltdcdev.hfp = 40;
        ltdcdev.hsw = 48;
        ltdcdev.vbp = 32;
        ltdcdev.vfp = 13;
        ltdcdev.vsw = 3;
        ltdcdev.pclk_hz = 18 * 1000 * 1000;
    }

    /* LCD RGB panel configuration structure */
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,                       /* RGB565 in parallel mode, thus 16bit in width */
        .psram_trans_align = 64,                /* Alignment of buffers (frame buffer) that allocated in PSRAM */
        .clk_src = LCD_CLK_SRC_PLL160M,         /* Clock source for the RGB LCD peripheral */
        .disp_gpio_num = GPIO_NUM_NC,           /* GPIO used for display control signal, set to -1 if it's not used */
        .pclk_gpio_num = GPIO_LCD_PCLK,         /* GPIO used for PCLK signal */
        .hsync_gpio_num = GPIO_NUM_NC,          /* GPIO used for HSYNC signal */
        .vsync_gpio_num = GPIO_NUM_NC,          /* GPIO used for VSYNC signal */
        .de_gpio_num = GPIO_LCD_DE,             /* GPIO used for DE signal, set to -1 if it's not used */
        .data_gpio_nums = {                     /* GPIOs used for data lines */
            GPIO_LCD_B3, GPIO_LCD_B4, GPIO_LCD_B5, GPIO_LCD_B6, GPIO_LCD_B7,
            GPIO_LCD_G2, GPIO_LCD_G3, GPIO_LCD_G4, GPIO_LCD_G5, GPIO_LCD_G6, GPIO_LCD_G7,
            GPIO_LCD_R3, GPIO_LCD_R4, GPIO_LCD_R5, GPIO_LCD_R6, GPIO_LCD_R7,
        },
        .timings = {                            /* RGB timing parameters, including the screen resolution */
            .pclk_hz = ltdcdev.pclk_hz,         /* Frequency of pixel clock */
            .h_res = ltdcdev.pwidth,            /* Horizontal resolution, i.e. the number of pixels in a line */
            .v_res = ltdcdev.pheight,           /* Vertical resolution, i.e. the number of lines in the frame */
            .hsync_back_porch = ltdcdev.hbp,    /* Horizontal back porch, number of PCLK between hsync and start of line active data */
            .hsync_front_porch = ltdcdev.hfp,   /* Horizontal front porch, number of PCLK between the end of active data and the next hsync */
            .hsync_pulse_width = ltdcdev.vsw,   /* Horizontal sync width, unit: PCLK period */
            .vsync_back_porch = ltdcdev.vbp,    /* Vertical back porch, number of invalid lines between vsync and start of frame */
            .vsync_front_porch = ltdcdev.vfp,   /* Vertical front porch, number of invalid lines between the end of frame and the next vsync */
            .vsync_pulse_width = ltdcdev.hsw,   /* Vertical sync width, unit: number of lines */
            .flags.pclk_active_neg = true,      /* Whether the display data is clocked out on the falling edge of PCLK */
        },
        .flags.fb_in_psram = true,              /* allocate frame buffer in PSRAM */
        /* If it's non-zero, the driver allocates two DRAM bounce buffers for DMA use.
           DMA fetching from DRAM bounce buffer is much faster than PSRAM frame buffer */
        .bounce_buffer_size_px = (ltdcdev.id == 0X4384) ? 480 * 10 : 272 * 10,
    };

    /* Create RGB LCD panel */
    esp_lcd_new_rgb_panel(&panel_config, &panel_handle);
    /* Reset LCD panel */
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    /* Initialize LCD panel */
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    ltdc_display_dir(1);
    ltdc_clear(WHITE);
    LCD_BL(1);
}


/**
 * @brief       LCD clear
 * @param       color: LCD color
 * @retval      None
 */
void ltdc_clear(uint16_t color)
{
    uint16_t *buffer = heap_caps_malloc(ltdcdev.width * sizeof(uint16_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    if (NULL == buffer)
    {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
    }
    else
    {
        for (uint16_t i = 0; i < ltdcdev.width; i++)
        {
            buffer[i] = color;
        }
        
        for (uint16_t y = 0; y < ltdcdev.height; y++)
        {
            taskENTER_CRITICAL(&my_spinlock);
            esp_lcd_panel_draw_bitmap(panel_handle, 0, y, ltdcdev.width, y + 1, buffer);
            taskEXIT_CRITICAL(&my_spinlock);
        }
        
        heap_caps_free(buffer);
    }
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

/**
 * @brief       Set the LCD display direction
 * @param       dir:0,portrait; 1,landscape
 * @retval      None
 */
void ltdc_display_dir(uint8_t dir)
{
    ltdcdev.dir = dir;

    if (ltdcdev.dir == 0)
    {
        ltdcdev.width = ltdcdev.pheight;
        ltdcdev.height = ltdcdev.pwidth;
        esp_lcd_panel_swap_xy(panel_handle, true);
        esp_lcd_panel_mirror(panel_handle, false, true);
    }
    else if (ltdcdev.dir == 1)
    {
        ltdcdev.width = ltdcdev.pwidth;
        ltdcdev.height = ltdcdev.pheight;
        esp_lcd_panel_swap_xy(panel_handle, false);
        esp_lcd_panel_mirror(panel_handle, false, false);
    }
}

/**
 * @brief       LCD draw point
 * @param       x    : The x-coordinate of the point to be drawn
 * @param       y    : The y-coordinate of the point to be drawn
 * @param       color: The color of the dot to be painted
 * @retval      None
 */
void ltdc_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    taskENTER_CRITICAL(&my_spinlock);
    esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 1, y + 1, &color);
    taskEXIT_CRITICAL(&my_spinlock);
}

/**
 * @brief       LCD area filling
 * @param       xs: region start x coordinate
 * @param       ys: region start y coordinate
 * @param       xe: region end x coordinate
 * @param       ye: region end y coordinate
 * @param       color: The color of the area to be painted
 * @retval      None
 */
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    if (sx < 0 || sy < 0 || ex > ltdcdev.width || ey > ltdcdev.height)
    {
        return;
    }  
      
    if (sx > ex || sy > ey)
    {
        return;
    }  
      
    sx = fmax(0, sx);
    sy = fmax(0, sy);
    ex = fmin(ltdcdev.width - 1, ex);
    ey = fmin(ltdcdev.height - 1, ey);
      
    for (int i = sx; i <= ex; i++)
    {
        for (int j = sy; j <= ey; j++)
        {
            ltdc_draw_point(i, j, color);
        }
    }
}

/**
 * @brief       Display monochrome icons
 * @param       x       : region start x coordinate
 * @param       y       : region start y coordinate
 * @param       width   : ico width
 * @param       height  : ico height
 * @param       icosbase: data
 * @param       color   : draw color
 * @param       bkcolor : background color
 * @retval      None
 */
void ltdc_app_show_mono_icos(uint16_t x,uint16_t y,uint8_t width,uint8_t height,uint8_t *icosbase,uint16_t color,uint16_t bkcolor)
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
                ltdc_draw_point(x,y,color);
            }
            else
            {
                ltdc_draw_point(x,y,bkcolor);
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

    for (t = 0; t <= distance + 1; t++)
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

/**
 * @brief       LCD draw rectangle
 * @param       x1   : The x-coordinate of end point 1 of the rectangle to be drawn
 * @param       y1   : The y-coordinate of end point 1 of the rectangle to be drawn
 * @param       x2   : The x-coordinate of end point 2 of the rectangle to be drawn
 * @param       y2   : The y-coordinate of end point 2 of the rectangle to be drawn
 * @param       color: The color of the rectangle to be painted
 * @retval      None
 */
void ltdc_draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color)
{
    ltdc_draw_line(x0, y0, x1, y0,color);
    ltdc_draw_line(x0, y0, x0, y1,color);
    ltdc_draw_line(x0, y1, x1, y1,color);
    ltdc_draw_line(x1, y0, x1, y1,color);
}

/**
 * @brief       LCD draw circle
 * @param       x    : The x-coordinate of the origin of the circle to be drawn
 * @param       y    : The Y coordinate of the origin of the circle to be drawn
 * @param       r    : The radius of the circle to be drawn
 * @param       color: The color of the circle to be painted
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

/**
 * @brief       The LCD displays 1 character
 * @param       x    : The X coordinate of the character to be displayed
 * @param       y    : The Y coordinate of the character to be displayed
 * @param       chr  : Character to be displayed
 * @param       mode : stacking(1); non-stacking(0)
 * @param       color: The color of the character to be displayed
 * @retval      None
 */
void ltdc_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    chr = (char)chr - ' ';

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[(uint8_t)chr];
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[(uint8_t)chr];
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[(uint8_t)chr];
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[(uint8_t)chr];
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
                ltdc_draw_point(x, y, g_back_color);
            }

            temp <<= 1;
            y++;

            if (y >= ltdcdev.height)return;

            if ((y - y0) == size)
            {
                y = y0;
                x++;
                
                if (x >= ltdcdev.width)
                {
                    return;
                }

                break;
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
 * @param       y    : The Y coordinate of the number to be displayed
 * @param       num  : To display numbers
 * @param       len  : The number of digits to be displayed
 * @param       color: The color of the numbers to be displayed
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
void ltdc_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
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
                if (mode & 0x80)
                {
                    ltdc_show_char(x + (size / 2) * t, y, '0', size, mode & 0x01, color);
                }
                else
                {
                    ltdc_show_char(x + (size / 2) * t, y, ' ', size, mode & 0x01, color);
                }

                continue;
            }
            else
            {
                enshow = 1;
            }

        }

        ltdc_show_char(x + (size / 2) * t, y, temp + '0', size, mode & 0x01, color);
    }
}

/**
 * @brief       LCD show string
 * @param       x    : The x-coordinate of the string to be displayed
 * @param       y    : The y-coordinate of the string to be displayed
 * @param       str  : To display string
 * @param       color: The color of the string to be displayed
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

        ltdc_show_char(x, y, *p, size, 1, color);
        x += size / 2;
        p++;
    }
}

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


i2c_obj_t oled_master;

uint8_t OLED_GRAM[128][8];

/**
 * @brief       write data to oled register
 * @param       data: write data 
 * @param       len: len
 * @retval      ESP_OK:none
 */
esp_err_t oled_write(uint8_t* data_wr, size_t size)
{
    i2c_buf_t bufs = {
        .len = size,
        .buf = data_wr,
    };

    i2c_transfer(&oled_master, OLED_ADDR, 1, &bufs, I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief       write command to oled
 * @param       tx_data: write data 
 * @param       command: command value
 * @retval      none
 */
void oled_write_Byte(unsigned char tx_data, unsigned char command)
{
    unsigned char data[2] = {command, tx_data};
    oled_write(data, sizeof(data));
}

/**
 * @brief       Update graphics memory to LCD
 * @param       None
 * @retval      None
 */
void oled_refresh_gram(void)
{
    uint8_t i, n;
    
    for (i = 0; i < 8; i++)
    {  
        oled_write_Byte(0xb0 + i, OLED_CMD);
        oled_write_Byte(0x00, OLED_CMD);
        oled_write_Byte(0x10, OLED_CMD);
        
        for (n = 0; n < 128; n++)
        {
            oled_write_Byte(OLED_GRAM[n][i], OLED_DATA);
        }
    }
}

/**
 * @brief       OLED module initialization
 * @param       None
 * @retval      None
 */
void oled_init(i2c_obj_t self)
{
    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_1);                                /* Initialize IIC1 */
    }

    oled_master = self;
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* disable interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT;                /* set as intput mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* enable pull-up mode */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* disable pull-down mode */
    gpio_init_struct.pin_bit_mask = 1ULL << OLED_D2_PIN;    /* bit mask of the pins that you want to set,e.g.GPIO1/2 */
    gpio_config(&gpio_init_struct);

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* disable interrupt */
    gpio_init_struct.mode = GPIO_MODE_OUTPUT;               /* set as intput mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* enable pull-up mode */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* disable pull-down mode */
    gpio_init_struct.pin_bit_mask = 1ULL << OLED_DC_PIN;    /* bit mask of the pins that you want to set,e.g.GPIO1/2 */
    gpio_config(&gpio_init_struct);
    
    OLED_DC(0);
    
    xl9555_pin_write(OV_RESET_IO, 1);

    /*reset oled*/
    OLED_RST(0);
    vTaskDelay(100);
    OLED_RST(1);
    vTaskDelay(100);

    /* code initialization */
    oled_write_Byte(0xAE, OLED_CMD);
    oled_write_Byte(0xD5, OLED_CMD);
    oled_write_Byte(80, OLED_CMD);
    oled_write_Byte(0xA8, OLED_CMD);
    oled_write_Byte(0X3F, OLED_CMD);
    oled_write_Byte(0xD3, OLED_CMD);
    oled_write_Byte(0X00, OLED_CMD);

    oled_write_Byte(0x40, OLED_CMD);

    oled_write_Byte(0x8D, OLED_CMD);
    oled_write_Byte(0x14, OLED_CMD);
    oled_write_Byte(0x20, OLED_CMD);
    oled_write_Byte(0x02, OLED_CMD);
    oled_write_Byte(0xA1, OLED_CMD);
    oled_write_Byte(0xC0, OLED_CMD);
    oled_write_Byte(0xDA, OLED_CMD);
    oled_write_Byte(0x12, OLED_CMD);
            
    oled_write_Byte(0x81, OLED_CMD);
    oled_write_Byte(0xEF, OLED_CMD);
    oled_write_Byte(0xD9, OLED_CMD);
    oled_write_Byte(0xf1, OLED_CMD);
    oled_write_Byte(0xDB, OLED_CMD);
    oled_write_Byte(0x30, OLED_CMD);

    oled_write_Byte(0xA4, OLED_CMD);
    oled_write_Byte(0xA6, OLED_CMD);
    oled_write_Byte(0xAF, OLED_CMD);
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
    oled_write_Byte(0X8D, OLED_CMD);
    oled_write_Byte(0X14, OLED_CMD);
    oled_write_Byte(0XAF, OLED_CMD);
}

/**
 * @brief       Close oled
 * @param       None
 * @retval      None
 */
void oled_off(void)
{
    oled_write_Byte(0X8D, OLED_CMD);
    oled_write_Byte(0X10, OLED_CMD);
    oled_write_Byte(0XAE, OLED_CMD);
}

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

/**
 * @brief       draw point
 * @param       x    : The x-coordinate of the point to be drawn
 *              y    : The y-coordinate of the point to be drawn
 *              t: The color of the dot to be painted
 * @retval      None
 */
void oled_draw_point(uint8_t x, uint8_t y, uint8_t dot)
{
    uint8_t pos, bx, temp = 0;

    if (x > 127 || y > 63)
    {
        return;
    }

    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);

    if(dot)
    {
        OLED_GRAM[x][pos] |= temp;  
    }
    else
    {
        OLED_GRAM[x][pos] &= ~ temp;
    }
}

/**
 * @brief       area filling
 * @param       x1: region start x coordinate
 *              y1: region start y coordinate
 *              x2: region end x coordinate
 *              y2: region end y coordinate
 *              dot: The color of the area to be painted
 * @retval      None
 */
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
    uint8_t x, y;
    
    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)
        {
            oled_draw_point(x, y, dot);
        }
    }

    oled_refresh_gram();
}

/**
 * @brief       displays 1 character
 * @param       x    : The X coordinate of the character to be displayed
 *              y    : The Y coordinate of the character to be displayed
 *              chr  : Character to be displayed
 *              mode : 0:display in reverse.1:Normal display
 *              size : Font size
 * @retval      None
 */
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    chr = chr - ' ';
    
    for (t = 0; t < csize; t ++)
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

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
            {
                oled_draw_point(x, y, mode);
            }
            else
            {
                oled_draw_point(x, y, !mode);
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

/**
 * @brief       Square function,x^y
 * @param       x: Base number
 *              y: index
 * @retval      x^y
 */
uint32_t oled_pow(uint8_t m, uint8_t n)
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
void oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    
    for (t = 0; t < len; t++) 
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                oled_show_char(x + (size / 2)*t, y, ' ', size, 1);
                continue;
            }
            else
            {
                enshow = 1;
            }
        }

        oled_show_char(x + (size / 2)*t, y, temp + '0', size, 1);
    }
} 

/**
 * @brief       show string
 * @param       x    : The x-coordinate of the string to be displayed
 *              y    : The y-coordinate of the string to be displayed
 *              *p   : To display string
 *              size : Font size
 * @retval      None
 */
void oled_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size)
{
    while ((*p <= '~') && (*p >= ' '))
    {       
        if (x > (128 - (size / 2)))
        {
            x = 0;
            y += size;
        }
        if (y > (64 - size))
        {
            y = x = 0;
            oled_clear();
        }

        oled_show_char(x, y, *p, size, 1);
        x += size / 2;
        p++;
    }
}

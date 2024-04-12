/**
 ****************************************************************************************************
 * @file        spilcd.cpp
 * @author      ALIENTEK
 * @brief       SPILCD code
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

#include "spilcd.h"
#include "font.h"
#include <SPI.h>
#include "xl9555.h"

#define SPI_LCD_TYPE    1           /* LCD_Type(1:2.4 inch SPILCD  0:1.3 inch SPILCD) */  

/* LCD width and LCD Height Definition */
#if SPI_LCD_TYPE                    /* 2.4 inch SPILCD */
uint16_t spilcd_width  = 240;       /* LCD width 240(portrait) */
uint16_t spilcd_height = 320;       /* LCD Height 320(portrait) */
#else
uint16_t spilcd_width  = 240;       /* LCD width 240(portrait) */
uint16_t spilcd_height = 240;       /* LCD Height 240(portrait) */
#endif                              /* 1.3 inch SPILCD */

uint8_t spilcd_dir = 1;             /* default landscape screen(1)、portrait screen(0) */

#define USE_LCD_BUF    1            /* default use lcd_buf,use lcd_buf make screen speed faster,sacrifice space to improve speed,If the memory is insufficient, set the value to 0 */
#if USE_LCD_BUF
    #if SPI_LCD_TYPE
    uint16_t lcd_buf[320 * 240];    /* Store a frame of image data */
    #else
    uint16_t lcd_buf[240 * 240];    /* Store a frame of image data */
    #endif
#endif

/* LCD brush color and background color */
uint32_t g_point_color = 0XF800;    /* brush color */
uint32_t g_back_color  = 0XFFFF;    /* background color */

static const int SPICLK = 80000000; /* SPI communication rate(If the display is abnormal, turn it down) */

SPIClass* spi_lcd = NULL;           /* uninitalised pointers to SPI objects */

/**
 * @brief       write command to LCD
 * @param       cmd:command
 * @retval      None
 */
static void lcd_write_cmd(uint8_t cmd)
{
    LCD_WR(0);
    spi_lcd->transfer(cmd);
}

/**
 * @brief       write data to LCD
 * @param       data:data
 * @retval      None
 */
static void lcd_write_data(uint8_t data)
{
    LCD_WR(1);
    spi_lcd->transfer(data);
}

/**
 * @brief	    write bytes data to LCD
 * @param       data:the start address of the data
 * @param       size:data length
 * @return      None
 */
static void lcd_write_bytes(uint8_t *data, uint32_t size)
{
    LCD_WR(1);
    spi_lcd->transfer(data, size);
}

/**
 * @brief       write pixel data to the LCD
 * @param       data:pixel data
 * @retval      None
 */
static void lcd_write_pixeldata(uint16_t data)
{
    LCD_WR(1);
    spi_lcd->transfer16(data);
}

/**
 * @brief       Set the LCD row and column address
 * @param       xs: column start address
 *              ys: row start address
 *              xe: column end address
 *              ye: row end address
 * @retval      None
 */
void lcd_set_address(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye) 
{
    lcd_write_cmd(0x2A);
    lcd_write_data((uint8_t)(xs >> 8) & 0xFF);
    lcd_write_data((uint8_t)xs & 0xFF);
    lcd_write_data((uint8_t)(xe >> 8) & 0xFF);
    lcd_write_data((uint8_t)xe & 0xFF);
    lcd_write_cmd(0x2B);
    lcd_write_data((uint8_t)(ys >> 8) & 0xFF);
    lcd_write_data((uint8_t)ys & 0xFF);
    lcd_write_data((uint8_t)(ye >> 8) & 0xFF);
    lcd_write_data((uint8_t)ye & 0xFF);
    lcd_write_cmd(0x2C);
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
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t temp;

    if (spilcd_dir == 1)  /* In landscape mode, change the scanning direction */
    {
        switch (dir)      /* Direction change */
        {
            case 0:
                dir = 5;
                break;

            case 1:
                dir = 7;
                break;

            case 2:
                dir = 4;
                break;

            case 3:
                dir = 6;
                break;

            case 4:
                dir = 1;
                break;

            case 5:
                dir = 0;
                break;

            case 6:
                dir = 3;
                break;

            case 7:
                dir = 2;
                break;
        }
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

    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3)); 

    LCD_CS(0);  
    lcd_write_cmd(dirreg);
    lcd_write_data(regval);

    if (regval & 0x20)
    {
        if (spilcd_width < spilcd_height)   /* Switch X,Y */
        {
            temp = spilcd_width;
            spilcd_width = spilcd_height;
            spilcd_height = temp;
        }
    }
    else
    {
        if (spilcd_width > spilcd_height)   /* Switch X,Y */
        {
            temp = spilcd_width;
            spilcd_width = spilcd_height;
            spilcd_height = temp;
        }
    }

    lcd_set_address(0, 0, spilcd_width - 1, spilcd_height - 1);

    LCD_CS(1);                    
    spi_lcd->endTransaction();
}

/**
 * @brief       Set the LCD display direction
 * @param       dir:0,portrait; 1,landscape
 * @retval      None
 */
void lcd_display_dir(uint8_t dir)
{
    spilcd_dir = dir;

    if (SPI_LCD_TYPE)   /* 2.4 inch need to be processed，1.3 inch not need to be processed */
    {
        if (dir == 0)   /* portrait */
        {
            spilcd_width = 240;
            spilcd_height = 320;   
        }
        else            /* landscape */
        {
            spilcd_width = 320; 
            spilcd_height = 240; 
        }
    }

    lcd_scan_dir(DFT_SCAN_DIR); 
}

/**
 * @brief       LCD clear
 * @param       color: LCD color
 * @retval      None
 */
void lcd_clear(uint16_t color) 
{
    uint32_t index = 0;
    uint32_t totalpoint = spilcd_width * spilcd_height;

    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3));
    LCD_CS(0);

    lcd_set_address(0, 0, spilcd_width - 1 ,spilcd_height - 1);

    LCD_WR(1);

#if USE_LCD_BUF 
    uint16_t color_tmp = ((color & 0x00FF) << 8) | ((color & 0xFF00) >> 8);   /* need to convert the color values */
    for (uint32_t i = 0; i < spilcd_width * spilcd_height; i++)               /* fill with lcd_buf */
    {
        lcd_buf[i] = color_tmp;
    }
    spi_lcd->transfer(lcd_buf, spilcd_width * spilcd_height * 2);             /* send a frame of image data(Byte) */
#else
    for (index = 0; index < totalpoint; index++)
    {
        spi_lcd->transfer16(color);
    }
#endif

    LCD_CS(1);
    spi_lcd->endTransaction();
}

/**
* @brief       lcd init
* @param       None
* @retval      None
*/
void lcd_init(void)
{
    /* pins needed to initialize the LCD screen */
    xl9555_io_config(SLCD_PWR, IO_SET_OUTPUT);
    xl9555_io_config(SLCD_RST, IO_SET_OUTPUT);
    pinMode(SLCD_WR_PIN, OUTPUT);

    /* The default pin status for the LCD screen is high */
    xl9555_pin_set(SLCD_PWR, IO_SET_HIGH);
    xl9555_pin_set(SLCD_RST, IO_SET_HIGH);
    digitalWrite(SLCD_WR_PIN, HIGH);
  
    /* configure SPI */
    spi_lcd = new SPIClass(HSPI);   /* initialise instances of the SPIClass attached to hspi respectively */
    spi_lcd->begin(SLCD_SCK_PIN, SLCD_SDI_PIN, SLCD_SDA_PIN, SLCD_CS_PIN);  /* Set the communication line for the SPI */
    pinMode(SLCD_CS_PIN, OUTPUT);   /* Set the CS pin to output mode */

    /* hardware reset */
    LCD_RST(1);
    delay(10);
    LCD_RST(0);
    delay(10);
    LCD_RST(1);
    delay(120);

    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3));    /* initialise hspi with SPISettings Configuration */
    
    LCD_CS(0);                  /* Pull down the piece selection and select the device */

    /* Configure the LCD register */
#if SPI_LCD_TYPE                /* Set the 2.4 inch LCD register */
    lcd_write_cmd(0x11);        /* Sleep Out */
    delay(120);                 /* wait for power stability */

    lcd_write_cmd(0x3A);        /* 65k mode */
    lcd_write_data(0x05);

    lcd_write_cmd(0xC5);        /* VCOM */
    lcd_write_data(0x1A);

    lcd_write_cmd(0x36);        /* displays the orientation Settings */
    lcd_write_data(0x00);

    /*-------------ST7789V Frame rate setting-----------*/
    lcd_write_cmd(0xB2);        /* Porch Setting */
    lcd_write_data(0x05);
    lcd_write_data(0x05);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);

    lcd_write_cmd(0xB7);        /* Gate Control */
    lcd_write_data(0x05);       /* 12.2v   -10.43v */

    /*--------------ST7789V Power setting---------------*/
    lcd_write_cmd(0xBB);        /* VCOM */
    lcd_write_data(0x3F);

    lcd_write_cmd(0xC0);        /* Power control */
    lcd_write_data(0x2c);

    lcd_write_cmd(0xC2);	    /* VDV and VRH Command Enable */
    lcd_write_data(0x01);

    lcd_write_cmd(0xC3);        /* VRH Set */
    lcd_write_data(0x0F);       /* 4.3+( vcom+vcom offset+vdv) */

    lcd_write_cmd(0xC4);        /* VDV Set */
    lcd_write_data(0x20);       /* 0v */

    lcd_write_cmd(0xC6);        /* Frame Rate Control in Normal Mode */
    lcd_write_data(0X01);       /* 111Hz */

    lcd_write_cmd(0xD0);        /* Power Control 1 */
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);

    lcd_write_cmd(0xE8);        /* Power Control 1 */
    lcd_write_data(0x03);

    lcd_write_cmd(0xE9);        /* Equalize time control */
    lcd_write_data(0x09);
    lcd_write_data(0x09);
    lcd_write_data(0x08);

    /*---------------ST7789V gamma setting-------------*/
    lcd_write_cmd(0xE0);        /* Set Gamma */
    lcd_write_data(0xD0);
    lcd_write_data(0x05);
    lcd_write_data(0x09);
    lcd_write_data(0x09);
    lcd_write_data(0x08);
    lcd_write_data(0x14);
    lcd_write_data(0x28);
    lcd_write_data(0x33);
    lcd_write_data(0x3F);
    lcd_write_data(0x07);
    lcd_write_data(0x13);
    lcd_write_data(0x14);
    lcd_write_data(0x28);
    lcd_write_data(0x30);

    lcd_write_cmd(0XE1);        /* Set Gamma */
    lcd_write_data(0xD0);
    lcd_write_data(0x05);
    lcd_write_data(0x09);
    lcd_write_data(0x09);
    lcd_write_data(0x08);
    lcd_write_data(0x03);
    lcd_write_data(0x24);
    lcd_write_data(0x32);
    lcd_write_data(0x32);
    lcd_write_data(0x3B);
    lcd_write_data(0x14);
    lcd_write_data(0x13);
    lcd_write_data(0x28);
    lcd_write_data(0x2F);

    lcd_write_cmd(0x21);        /* Inverted Display */
    lcd_write_cmd(0x29);        /* Open display */
#else                           /* Set the 1.3 inch LCD register */
    lcd_write_cmd(0x11);        /* Sleep Out */
    delay(120);                 /* wait for power stability */

    lcd_write_cmd(0x36);        /* Memory Data Access Control */
    lcd_write_data(0x00);

    lcd_write_cmd(0x3A);        /* RGB 5-6-5-bit  */
    lcd_write_data(0x65);

    lcd_write_cmd(0xB2);        /* Porch Setting */
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);

    lcd_write_cmd(0xB7);        /*  Gate Control */
    lcd_write_data(0x72);

    lcd_write_cmd(0xBB);        /* VCOM Setting */
    lcd_write_data(0x3D);

    lcd_write_cmd(0xC0);        /* LCM Control */
    lcd_write_data(0x2C);

    lcd_write_cmd(0xC2);        /* VDV and VRH Command Enable */
    lcd_write_data(0x01);

    lcd_write_cmd(0xC3);        /* VRH Set */
    lcd_write_data(0x19);

    lcd_write_cmd(0xC4);        /* VDV Set */
    lcd_write_data(0x20);

    lcd_write_cmd(0xC6);        /* Frame Rate Control in Normal Mode */
    lcd_write_data(0x0F);

    lcd_write_cmd(0xD0);        /* Power Control 1 */
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);

    lcd_write_cmd(0xE0);        /* Positive Voltage Gamma Control */
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0D);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2B);
    lcd_write_data(0x3F);
    lcd_write_data(0x54);
    lcd_write_data(0x4C);
    lcd_write_data(0x18);
    lcd_write_data(0x0D);
    lcd_write_data(0x0B);
    lcd_write_data(0x1F);
    lcd_write_data(0x23);

    lcd_write_cmd(0xE1);        /* Negative Voltage Gamma Control */
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0C);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2C);
    lcd_write_data(0x3F);
    lcd_write_data(0x44);
    lcd_write_data(0x51);
    lcd_write_data(0x2F);
    lcd_write_data(0x1F);
    lcd_write_data(0x1F);
    lcd_write_data(0x20);
    lcd_write_data(0x23);

    lcd_write_cmd(0x21);        /* Display Inversion On */
    lcd_write_cmd(0x29);
#endif
    LCD_CS(1);                  /* Pull up the line selection and uncheck it */  
    spi_lcd->endTransaction();  /* end SPI transaction */

    lcd_display_dir(1);         /* default landscape */
    lcd_display_on();           /* Turn on the LCD backlight */
    lcd_clear(WHITE);           /* lcd clear */
}

/**
 * @brief       Turn on the LCD backlight
 * @param       None
 * @retval      None
 */
void lcd_display_on(void)
{
    LCD_PWR(1);
}

/**
 * @brief       Turn off the LCD backlight
 * @param       None
 * @retval      None
 */
void lcd_display_off(void)
{
    LCD_PWR(0);
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
void lcd_fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
    uint32_t area_size;
    uint16_t buf_index;
    
    area_size = (xe - xs + 1) * (ye - ys + 1) * sizeof(uint16_t);   /* the number of bytes of the area */

    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3));
    LCD_CS(0);

    lcd_set_address(xs, ys, xe, ye);
    LCD_WR(1);
    
#if USE_LCD_BUF
    uint16_t color_tmp = ((color & 0x00FF) << 8) | ((color & 0xFF00) >> 8);
    for (uint32_t i = 0; i < area_size / sizeof(uint16_t); i++)
    {
        lcd_buf[i] = color_tmp;
    }
    spi_lcd->transfer(lcd_buf, area_size);
#else
    for (buf_index = 0; buf_index < area_size / sizeof(uint16_t); buf_index++)
    {
        spi_lcd->transfer16(color);
    }
#endif

    LCD_CS(1);
    spi_lcd->endTransaction();
}

/**
 * @brief       LCD draw point
 * @param       x    : The x-coordinate of the point to be drawn
 *              y    : The y-coordinate of the point to be drawn
 *              color: The color of the dot to be painted
 * @retval      None
 */
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3));
    LCD_CS(0);

    lcd_set_address(x, y, x, y);

    lcd_write_pixeldata(color);

    LCD_CS(1);
    spi_lcd->endTransaction();
}

/**
 * @brief       LCD draw line
 * @param       x1   : The x-coordinate of end point 1 of the line to be drawn
 *              y1   : The y-coordinate of end point 1 of the line to be drawn
 *              x2   : The x-coordinate of end point 2 of the line to be drawn
 *              y2   : The y-coordinate of end point 2 of the line to be drawn
 *              color: The color of the line to be painted
 * @retval      None
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t x_delta;
    uint16_t y_delta;
    int16_t x_sign;
    int16_t y_sign;
    int16_t error;
    int16_t error2;
    
    x_delta = (x1 < x2) ? (x2 - x1) : (x1 - x2);
    y_delta = (y1 < y2) ? (y2 - y1) : (y1 - y2);
    x_sign = (x1 < x2) ? 1 : -1;
    y_sign = (y1 < y2) ? 1 : -1;
    error = x_delta - y_delta;
    
    lcd_draw_point(x2, y2, color);
    
    while ((x1 != x2) || (y1 != y2))
    {
        lcd_draw_point(x1, y1, color);
        
        error2 = error << 1;
        if (error2 > -y_delta)
        {
            error -= y_delta;
            x1 += x_sign;
        }
      
        if (error2 < x_delta)
        {
            error += x_delta;
            y1 += y_sign;
        }
    }
}

/**
 * @brief       LCD draw horizontal line
 * @param       x,y   : Origin coordinate
 * @param       len   : line length
 * @param       color : The color of the horizontal line to be painted
 * @retval      None
 */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > spilcd_width) || (y > spilcd_height))
    {
        return;
    }

    lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * @brief       LCD draw rectangle
 * @param       x1   : The x-coordinate of end point 1 of the rectangle to be drawn
 *              y1   : The y-coordinate of end point 1 of the rectangle to be drawn
 *              x2   : The x-coordinate of end point 2 of the rectangle to be drawn
 *              y2   : The y-coordinate of end point 2 of the rectangle to be drawn
 *              color: The color of the rectangle to be painted
 * @retval      None
 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * @brief       LCD draw circle
 * @param       x    : The x-coordinate of the origin of the circle to be drawn
 *              y    : The Y coordinate of the origin of the circle to be drawn
 *              r    : The radius of the circle to be drawn
 *              color: The color of the circle to be painted
 * @retval      None
 */
void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    int32_t x_t;
    int32_t y_t;
    int32_t error;
    int32_t error2;
    
    x_t = -r;
    y_t = 0;
    error = 2 - 2 * r;
    
    do 
    {
        lcd_draw_point(x - x_t, y + y_t, color);
        lcd_draw_point(x + x_t, y + y_t, color);
        lcd_draw_point(x + x_t, y - y_t, color);
        lcd_draw_point(x - x_t, y - y_t, color);
        
        error2 = error;
        if (error2 <= y_t)
        {
            y_t++;
            error = error + (y_t * 2 + 1);
            if ((-x_t == y_t) && (error2 <= x_t))
            {
                error2 = 0;
            }
        }
        
        if (error2 > x_t)
        {
            x_t++;
            error = error + (x_t * 2 + 1);
        }
    } while (x_t <= 0);
}

/**
 * @brief       Fill the solid circle
 * @param       x,y  : Center of circle coordinates
 * @param       r    : radius
 * @param       color: Color of circle
 * @retval      None
 */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    lcd_draw_hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
      if ((i * i + xr * xr) > sqmax)
      {
          /* draw lines from outside */
          if (xr > imax)
          {
            lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
            lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
          }

          xr--;
      }

      /* draw lines from inside (center) */
      lcd_draw_hline(x - xr, y + i, 2 * xr, color);
      lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}



/**
 * @brief       The LCD displays 1 character
 * @param       x    : The X coordinate of the character to be displayed
 *              y    : The Y coordinate of the character to be displayed
 *              ch   : Character to be displayed
 *              font : The font of the character to be displayed
 *              mode : stacking(1); non-stacking(0)
 *              color: The color of the character to be displayed
 * @retval      None
 */
void lcd_show_char(uint16_t x, uint16_t y, char ch, lcd_font_t font, uint8_t mode, uint16_t color)
{
    const uint8_t *ch_code;
    uint8_t ch_width;
    uint8_t ch_height;
    uint8_t ch_size;
    uint8_t ch_offset;
    uint8_t byte_index;
    uint8_t byte_code;
    uint8_t bit_index;
    uint8_t width_index = 0;
    uint8_t height_index = 0;
    
    ch_offset = ch - ' ';   /* The offset value is obtained(the ASCII library is modded from a space, so - "is the library of the corresponding character) */
    
    switch (font)           /* Gets the height and width of the font */
    {
#if (FONT_12 != 0)
        case LCD_FONT_12:
        {
            ch_code = font_1206[ch_offset];
            ch_width = FONT_12_CHAR_WIDTH;
            ch_height = FONT_12_CHAR_HEIGHT;
            ch_size = FONT_12_CHAR_SIZE;
            break;
        }
#endif
#if (FONT_16 != 0)
        case LCD_FONT_16:
        {
            ch_code = font_1608[ch_offset];
            ch_width = FONT_16_CHAR_WIDTH;
            ch_height = FONT_16_CHAR_HEIGHT;
            ch_size = FONT_16_CHAR_SIZE;
            break;
        }
#endif
#if (FONT_24 != 0)
        case LCD_FONT_24:
        {
            ch_code = font_2412[ch_offset];
            ch_width = FONT_24_CHAR_WIDTH;
            ch_height = FONT_24_CHAR_HEIGHT;
            ch_size = FONT_24_CHAR_SIZE;
            break;
        }
#endif
#if (FONT_32 != 0)
        case LCD_FONT_32:
        {
            ch_code = font_3216[ch_offset];
            ch_width = FONT_32_CHAR_WIDTH;
            ch_height = FONT_32_CHAR_HEIGHT;
            ch_size = FONT_32_CHAR_SIZE;
            break;
        }
#endif
        default:
        {
            return;
        }
    }
    
    if ((x + ch_width > spilcd_width) || (y + ch_height > spilcd_height))
    {
        return;
    }
    
    for (byte_index = 0; byte_index < ch_size; byte_index++)
    {
        byte_code = ch_code[byte_index];                  /* Gets the character's dot matrix data */

        for (bit_index = 0; bit_index < 8; bit_index++)   /* One byte has eight dots */
        {
            if ((byte_code & 0x80) != 0)                  /* Valid point, need to display */
            {
                lcd_draw_point(x + width_index, y + height_index, color);           /* Draw a dot */
            }
            else if (mode == 0)
            {
                lcd_draw_point(x + width_index, y + height_index, g_back_color);    /* Draw background color */
            }

            height_index++;

            if (height_index == ch_height)    /* Have you finished showing a column? */
            {
                height_index = 0;             /* Y-coordinate reset */
                width_index++;                /* X-coordinate increment */
                break;
            }

            byte_code <<= 1;                  /* Shift to get the state of the next bit */
        }
    }
}

/**
 * @brief       LCD show string
 * @param       x    : The x-coordinate of the string to be displayed
 *              y    : The y-coordinate of the string to be displayed
 *              str  : To display string
 *              font : The font of the string to be displayed
 *              color: The color of the string to be displayed
 * @retval      None
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, lcd_font_t font, char *str,  uint16_t color)
{
    uint8_t x0 = x;
    uint8_t ch_width;
    uint8_t ch_height;
    
    width += x;
    height += y;

    switch (font)   /* Obtain the height and width of the font */
    {
#if (FONT_12 != 0)
        case LCD_FONT_12:
        {
            ch_width = FONT_12_CHAR_WIDTH;
            ch_height = FONT_12_CHAR_HEIGHT;
            break;
        }
#endif
#if (FONT_16 != 0)
        case LCD_FONT_16:
        {
            ch_width = FONT_16_CHAR_WIDTH;
            ch_height = FONT_16_CHAR_HEIGHT;
            break;
        }
#endif
#if (FONT_24 != 0)
        case LCD_FONT_24:
        {
            ch_width = FONT_24_CHAR_WIDTH;
            ch_height = FONT_24_CHAR_HEIGHT;
            break;
        }
#endif
#if (FONT_32 != 0)
        case LCD_FONT_32:
        {
            ch_width = FONT_32_CHAR_WIDTH;
            ch_height = FONT_32_CHAR_HEIGHT;
            break;
        }
#endif
        default:
        {
            return;
        }
    }
    
    while ((*str >= ' ') && (*str <= '~'))   /* Determine if it is an illegal character */
    {
        if (x >= width)
        {
            x = x0;
            y += ch_height;
        }
        
        if (y >= height)
        {
            break;
        }
      
      lcd_show_char(x, y, *str, font, 0, color);
      
      x += ch_width;
      str++;
    }
}

/**
 * @brief       Square function,x^y
 * @param       x: Base number
 *              y: index
 * @retval      x^y
 */
static uint32_t lcd_pow(uint8_t x, uint8_t y)
{
    uint8_t loop;
    uint32_t res = 1;
    
    for (loop = 0; loop < y; loop++)
    {
        res *= x;
    }
    
    return res;
}

/**
 * @brief       LCD displays numbers and can control the display of high 0
 * @param       x    : The X coordinate of the number to be displayed
 *              y    : The Y coordinate of the number to be displayed
 *              num  : To display numbers
 *              len  : The number of digits to be displayed
 *              mode : NUM_SHOW_NOZERO: The high digit 0 is not displayed
 *                     NUM_SHOW_ZERO  : the high digit 0 display
 *              font : Font for numbers to be displayed
 *              color: The color of the numbers to be displayed
 * @retval      None
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len,  lcd_font_t font, num_mode_t mode, uint16_t color)
{
    uint8_t ch_width;
    uint8_t len_index;
    uint8_t num_index;
    uint8_t first_nozero = 0;
    char pad;
  
    switch (font)
    {
#if (FONT_12 != 0)
        case LCD_FONT_12:
        {
            ch_width = FONT_12_CHAR_WIDTH;
            break;
        }
#endif
#if (FONT_16 != 0)
        case LCD_FONT_16:
        {
            ch_width = FONT_16_CHAR_WIDTH;
            break;
        }
#endif
#if (FONT_24 != 0)
        case LCD_FONT_24:
        {
            ch_width = FONT_24_CHAR_WIDTH;
            break;
        }
#endif
#if (FONT_32 != 0)
        case LCD_FONT_32:
        {
            ch_width = FONT_32_CHAR_WIDTH;
            break;
        }
#endif
        default:
        {
            return;
        }
    }
  
    switch (mode)
    {
        case NUM_SHOW_NOZERO:
        {
            pad = ' ';
            break;
        }
        case NUM_SHOW_ZERO:
        {
            pad = '0';
            break;
        }
        default:
        {
            return;
        }
    }
  
    for (len_index = 0; len_index < len; len_index++)                 /* Cycle by total number of displayed digits */
    {
        num_index = (num / lcd_pow(10, len - len_index - 1)) % 10;    /* Get the corresponding digit */
        if ((first_nozero == 0) && (len_index < (len - 1)))           /* Not enabled to display, and there are still bits to display */
        {
            if (num_index == 0)
            {
                lcd_show_char(x + ch_width * len_index, y, pad, font, mode & 0x01, color);   /* High position needs to be filled with 0 */
                continue;
            }
            else
            {
                first_nozero = 1;   /* Enable display */
            }
        }
        
        lcd_show_char(x + ch_width * len_index, y, num_index + '0', font, mode & 0x01, color);
    }
}

/**
 * @brief       LCD displays numbers and does not display high zeros
 * @param       x    : The X coordinate of the number to be displayed
 *              y    : The Y coordinate of the number to be displayed
 *              num  : To display numbers
 *              len  : The number of digits to be displayed
 *              font : Font for numbers to be displayed
 *              color: The color of the numbers to be displayed
 * @retval      None
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, lcd_font_t font, uint16_t color)
{
    lcd_show_xnum(x, y, num, len, font,  NUM_SHOW_NOZERO, color);
}

/**
 * @brief       LCD show picture
 * @note        Image sampling method: horizontal scanning, RGB565, high bit in front
 * @param       x     : The X coordinate of the image to be displayed
 *              y     : The Y coordinate of the image to be displayed
 *              width : The width of the image to be displayed
 *              height: The height of the image to be displayed
 *              pic   : The first address of the image array to be displayed
 * @retval      None
 */
void lcd_show_pic(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *pic)
{
    if ((x + width > spilcd_width) || (y + height > spilcd_height))
    {
        return;
    }

    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3));                               
    LCD_CS(0);

    lcd_set_address(x, y, x + width - 1, y + height - 1);

    lcd_write_bytes(pic, width * height * sizeof(uint16_t));

    LCD_CS(1);
    spi_lcd->endTransaction();
}


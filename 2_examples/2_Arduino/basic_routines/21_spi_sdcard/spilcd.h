/**
 ****************************************************************************************************
 * @file        spilcd.h
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

#ifndef __SPILCD_H
#define __SPILCD_H

#include "Arduino.h"

/* LCD width and height definition */
extern uint16_t spilcd_width;
extern uint16_t spilcd_height;
extern uint8_t  spilcd_dir;  

/* Define the font enabled for LCD */
#define FONT_12             1
#define FONT_16             1
#define FONT_24             1
#define FONT_32             1

/* Enable font size 12 by default */
#if ((FONT_12 == 0) && (FONT_16 == 0) && (FONT_24 == 0) && (FONT_32 == 0))
#undef FONT_12
#define FONT_12 1
#endif

/* LCD display font enumeration */
typedef enum
{
#if (FONT_12 != 0)
    LCD_FONT_12,             /* Font size 12 */
#endif
#if (FONT_16 != 0)
    LCD_FONT_16,             /* Font size 16 */
#endif
#if (FONT_24 != 0)
    LCD_FONT_24,             /* Font size 24 */
#endif
#if (FONT_32 != 0)
    LCD_FONT_32,             /* Font size 32 */
#endif
} lcd_font_t;

/* LCD display number mode */
typedef enum
{
    NUM_SHOW_NOZERO = 0x00,  /* The high digit 0 is not displayed */
    NUM_SHOW_ZERO,           /* The high digit 0 display */
} num_mode_t;

/* pin definition */
#define SLCD_CS_PIN       21   
#define SLCD_SDA_PIN      11 
#define SLCD_SCK_PIN      12
#define SLCD_SDI_PIN      -1
#define SLCD_WR_PIN       40

/* The Pin required for the following two SPI-LCD is already defined in xl9555.h
 * #define SLCD_PWR_PIN          
 * #define SLCD_RST_PIN  
 */

/* LCD operate definition */
#define LCD_PWR(x)        xl9555_pin_set(SLCD_PWR, x ? IO_SET_HIGH : IO_SET_LOW)
#define LCD_RST(x)        xl9555_pin_set(SLCD_RST, x ? IO_SET_HIGH : IO_SET_LOW)
#define LCD_WR(x)         digitalWrite(SLCD_WR_PIN, x)
#define LCD_CS(x)         digitalWrite(SLCD_CS_PIN, x)

/******************************************************************************************/
/* LCD scanning direction and color definition */

/* Scanning direction definition */
#define L2R_U2D         0           /* From left to right, from top to bottom */
#define L2R_D2U         1           /* From left to right, from bottom to top */
#define R2L_U2D         2           /* From right to left, from top to bottom */
#define R2L_D2U         3           /* From right to left, from bottom to top */

#define U2D_L2R         4           /* From top to bottom, from left to right */
#define U2D_R2L         5           /* From top to bottom, from right to left */
#define D2U_L2R         6           /* From bottom to top, from left to right */
#define D2U_R2L         7           /* From bottom to top, from right to left */

#define DFT_SCAN_DIR    L2R_U2D     /* Default scanning direction */

/* stroke color */
#define WHITE               0xFFFF
#define BLACK               0x0000
#define BLUE                0x001F  
#define BRED                0XF81F
#define GRED                0XFFE0
#define GBLUE               0X07FF
#define RED                 0xF800
#define MAGENTA             0xF81F
#define GREEN               0x07E0
#define CYAN                0x7FFF
#define YELLOW              0xFFE0
#define BROWN               0XBC40      
#define BRRED               0XFC07      
#define GRAY                0X8430      
#define DARKBLUE            0X01CF      
#define LIGHTBLUE           0X7D7C      
#define GRAYBLUE            0X5458      
#define LIGHTGREEN          0X841F      
#define LGRAY               0XC618      
#define LGRAYBLUE           0XA651      
#define LBBLUE              0X2B12      

/* function declaration */
void lcd_init(void);                                                                     
void lcd_set_address(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye);              
void lcd_clear(uint16_t color);                                                          
void lcd_display_on(void);                                                               
void lcd_display_off(void);                                                               
void lcd_fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);        
void lcd_display_dir(uint8_t dir);
void lcd_scan_dir(uint8_t dir);

void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);                                  
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);      
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);                    
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);  
void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                     
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                     

void lcd_show_char(uint16_t x, uint16_t y, char ch, lcd_font_t font, uint8_t mode, uint16_t color);                         
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, lcd_font_t font, char *str,  uint16_t color); 
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len,  lcd_font_t font, num_mode_t mode, uint16_t color);   
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, lcd_font_t font, uint16_t color);                      
void lcd_show_pic(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *pic);                                   

#endif

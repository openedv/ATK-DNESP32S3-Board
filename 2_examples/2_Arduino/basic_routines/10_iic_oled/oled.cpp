/**
 ****************************************************************************************************
 * @file        oled.cpp
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

#include "oled.h"
#include "xl9555.h"
#include "SSD1306Wire.h"

SSD1306Wire display(OLED_ADDR, OLED_SDA_PIN, OLED_SCL_PIN, GEOMETRY_128_64, I2C_TWO, 800000);   /* OLED objects */

/**
 * @brief       OLED module Init
 * @param       None
 * @retval      None
 */
void oled_init(void)
{
    /* oled module pins init */
    pinMode(OLED_D2_PIN, INPUT_PULLUP);           /* OLED module D2 and D1 Pins connected, also D2_Pin set Input */
    pinMode(OLED_DC_PIN, OUTPUT);                 /* OLED_DC is related to address */
    digitalWrite(OLED_DC_PIN, 0);                 /* OLED_DC is connected to GND, 8bits address is 0x78; is connected to VCC, 8bits address is 0x7A */

    xl9555_io_config(OV_RESET, IO_SET_OUTPUT);    /* OLED_RST is connected to XL9555_P05 */
    xl9555_pin_set(OV_RESET, IO_SET_HIGH);        /* OLED_RST is connected to VCC by default */

    /* hardware reset */
    OLED_RST(0);
    delay(100);
    OLED_RST(1);
    delay(100);

    display.init();                               /* OLED module init */
    display.flipScreenVertically();               /* Flip the screen Settings vertically */
    display.setFont(ArialMT_Plain_24);            /* set 24 font */
    display.drawString(0, 0, "ALIENTEK");         /* 24 font show ALIENTEK */
    display.setFont(ArialMT_Plain_16);            /* set 16 font */    
    display.drawString(0, 24, "0.96' OLED TEST"); /* 24 font show 0.96' OLED TEST */
    display.setFont(ArialMT_Plain_10);            /* set 10 font */ 
    display.drawString(0, 40, "ATOM 2023/12/1");  /* 24 font show ATOM 2023/12/1 */
    display.drawString(0, 50, "ASCII:");          /* 24 font show ASCII: */
    display.drawString(64, 50, "CODE:");          /* 24 font show CODE: */
    display.display();                            /* Cache data put into screen */
}

/**
 * @brief       OLED module show demo
 * @param       none
 * @retval      none
 */
void oled_show_demo(void)
{
    char t = ' ';

    while(1)
    {
        display.setColor(BLACK);                        /* color is set black */
        display.fillRect(35, 50, 10, 15);               /* clear ASCII display area */
        display.fillRect(104, 50, 20, 15);              /* clear CODE display area */
        display.setColor(WHITE);                        /* color is set white */
        display.drawString(35, 50, String(t));          /* show ASCII char */
        display.drawString(104, 50, String(t - 0));     /* show ASCII value  */
        display.display();                              /* cache data put into screen */

        t++;
        if (t > '~') 
        {
            t = ' ';
        }

        delay(500);
    }
}



  

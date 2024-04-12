/**
 ****************************************************************************************************
 * @file        13_adc.ino
 * @author      ALIENTEK
 * @brief       ADC example
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

#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "adc.h"


uint16_t adc_value = 0;
float adc_vol = 0;

/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    uart_init(0, 115200);   /* uart0 initialization */
    xl9555_init();          /* xl9555 initialization */
    lcd_init();             /* lcd initialization */

    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "ADC TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "ADC_VAL:", BLUE);
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "ADC_VOL:0.000V", BLUE);   
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    adc_value = adc_get(ADC_IN_PIN);                                                      /* read the ADC value of the Pin8 */
    lcd_show_xnum(94, 110, adc_value, 5, LCD_FONT_16, NUM_SHOW_NOZERO, BLUE);             /* displays the raw value of the ADC sample */

    adc_vol = (float)adc_value * 3.3 / 4095;                                              /* the voltage value is obtained by conversion */
    adc_value = adc_vol;                                                                  /* assign the integer part to the adc_value variable because adc_value is an integer of u16 */
    lcd_show_xnum(94, 130, (uint16_t)adc_value, 1, LCD_FONT_16, NUM_SHOW_NOZERO, BLUE);   /* displays the integer portion of the voltage value */

    adc_vol -= adc_value;                                                                 /* remove the integer part that is already displayed, leaving the decimal part */
    adc_vol *= 1000;                                                                      /* multiplying the decimal part by 1000 is equivalent to keeping three decimal places */
    lcd_fill(110, 130, 110 + 24, 130 + 16, WHITE);
    lcd_show_xnum(110, 130, adc_vol, 3, LCD_FONT_16, NUM_SHOW_ZERO, BLUE);                /* display fractional parts (previously converted to plastic display) */

    delay(500);
}

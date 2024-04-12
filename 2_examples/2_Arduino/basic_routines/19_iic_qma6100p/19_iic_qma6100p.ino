/**
 ****************************************************************************************************
 * @file        19_qma6100p.ino
 * @author      ALIENTEK
 * @brief       QMA6100P example
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

#include "led.h"
#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "qma6100p.h"
#include "imu.h"


float g_acc_data[3];
float g_angle_data[2];

/**
 * @brief       show angle
 * @param       x, y : coordinates
 * @param       title: title
 * @param       angle: angle
 * @retval      None
 */
void user_show_angle(uint16_t x, uint16_t y, char *title, float angle)
{
    char buf[20];

    sprintf(buf,"%s%3.1f", title, angle);                    
    lcd_fill(x, y, x + 160, y + 16, WHITE);                   
    lcd_show_string(x, y, 160, 16, LCD_FONT_16, buf, BLUE);   
}

/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    led_init();             /* led initialization */
    uart_init(0, 115200);   /* uart0 initialization */
    xl9555_init();          /* xl9555 initialization */
    lcd_init();             /* lcd initialization */
    qma6100p_init();        /* qma6100p initialization */
    
    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "IMU TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    qma6100p_read_acc_xyz(g_acc_data);
    acc_get_angle(g_acc_data, g_angle_data);

    user_show_angle(30, 130, "Pitch :", g_angle_data[0]);
    user_show_angle(30, 150, " Roll :", g_angle_data[1]);

    LED_TOGGLE();
    delay(500);
}

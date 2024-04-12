/**
 ****************************************************************************************************
 * @file        09_iic_exio.ino
 * @author      ALIENTEK
 * @brief       EXIO example
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
 
#include "xl9555.h"
#include "led.h"
#include "uart.h"


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

    xl9555_io_config(KEY0 | KEY1 | KEY2 | KEY3, IO_SET_INPUT);  /* set xl9555 as INPUT function pin */
    xl9555_io_config(BEEP, IO_SET_OUTPUT);                      /* set xl9555 as OUTPUT function pin */
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    if (!IIC_INT)   /* 16Pin state have change */
    {
        if (xl9555_get_pin(KEY0) == 0)
        {
            Serial.printf("KEY0 is pressed, BEEP is on \r\n");
            xl9555_pin_set(BEEP, IO_SET_LOW);
        }

        if (xl9555_get_pin(KEY1) == 0)
        {
            Serial.printf("KEY1 is pressed, BEEP is off \r\n");
            xl9555_pin_set(BEEP, IO_SET_HIGH);
        }

        if (xl9555_get_pin(KEY2) == 0)
        {
            Serial.printf("KEY2 is pressed, LED is on \r\n");
            LED(0);
        }

        if (xl9555_get_pin(KEY3) == 0)
        {
            Serial.printf("KEY3 is pressed, LED is off \r\n");
            LED(1);
        }
    }

    delay(200);
}

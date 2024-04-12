/**
 ****************************************************************************************************
 * @file        02_key.ino
 * @author      ALIENTEK
 * @brief       KEY example
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
#include "key.h"


/**
 * @brief    Initialize function
 * @param    None
 * @retval   None
 */
void setup() 
{
    led_init();         /* led initialization */
    key_init();         /* key initialization */
}

/**
 * @brief    Loop function
 * @param    None
 * @retval   None
 */
void loop() 
{
    if (KEY == 0)       /* KEY_PIN to read Low when the switch is pressed */
    {
        delay(10);      /* wait for 10ms */

        if (KEY == 0)   /* the switch is pressed */
        {
            LED(0);     /* led on */
        }
    }
    else                /* KEY_PIN to read HIGH when the switch is open */
    {
        LED(1);         /* led off */
    }
}

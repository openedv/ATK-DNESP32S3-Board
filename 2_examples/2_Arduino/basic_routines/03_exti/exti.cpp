/**
 ****************************************************************************************************
 * @file        exti.cpp
 * @author      ALIENTEK
 * @brief       EXTI code
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

#include "exti.h"
#include "key.h"

uint8_t led_state = 0;        /* led_state determined led state */

/**
 * @brief       configure pin with interrupts
 * @param       None
 * @retval      None
 */
void exti_init(void) 
{
    key_init();                                                             /* key initialization */
    attachInterrupt(digitalPinToInterrupt(KEY_INT_PIN), key_isr, FALLING);  /* pin with interrupt initialization */
}

/**
 * @brief       interrupt service routine
 * @param       None
 * @retval      None
 */
void key_isr(void)
{
    delay(10);                      /* wait for 500ms */

    if (KEY == 0)                   /* the pin goes from high to low when the switch is pressed */
    {
        led_state =! led_state;     /* led_state：goes from 0 to 1 or goes from 1 to 0 */
    }
}

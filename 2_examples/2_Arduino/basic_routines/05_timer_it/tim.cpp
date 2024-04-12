/**
 ****************************************************************************************************
 * @file        tim.cpp
 * @author      ALIENTEK
 * @brief       TIMER code
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

#include "tim.h"
#include "led.h"

hw_timer_t *timer = NULL;

/**
 * @brief       configure timer
 * @param       arr: arr set interrupt period
 * @param       psc: prescaler
 * @retval      None
 */
void timx_int_init(uint16_t arr, uint16_t psc)
{
    timer = timerBegin(TIMx_INT, psc, true);         /* timer 0, div psc */

    timerAlarmWrite(timer, arr, true);               /* set time in us */

    timerAttachInterrupt(timer, &TIMx_ISR, true);    /* attach callback */

    timerAlarmEnable(timer);                         /* enable interrupt */
}

/**
 * @brief      timer interrupt service routine
 * @param      None
 * @retval     None
 */
void TIMx_ISR(void)
{
    LED_TOGGLE();
}


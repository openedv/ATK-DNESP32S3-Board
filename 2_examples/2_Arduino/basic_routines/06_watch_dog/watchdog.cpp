/**
 ****************************************************************************************************
 * @file        watchdog.cpp
 * @author      ALIENTEK
 * @brief       WATCHDOG code
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

#include "watchdog.h"

hw_timer_t *wdg_timer = NULL;

/**
 * @brief       configure watchdog timer
 * @param       arr: arr set interrupt period
 * @param       psc: prescaler
 * @retval      None
 */
void wdg_init(uint32_t arr, uint16_t psc)
{
    wdg_timer = timerBegin(WDG_TIMx, psc, true);         /* timer 1, div psc */

    timerAlarmWrite(wdg_timer, arr, true);               /* set time in us */

    timerAttachInterrupt(wdg_timer, &WDG_ISR, true);     /* attach callback */

    timerAlarmEnable(wdg_timer);                         /* enable interrupt */
}

/**
 * @brief      watchdog timer interrupt service routine
 * @param      None
 * @retval     None
 */
void WDG_ISR(void)
{
    ets_printf("reboot\n");
    esp_restart();
}

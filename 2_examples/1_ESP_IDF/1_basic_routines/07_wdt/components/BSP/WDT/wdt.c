/**
 ****************************************************************************************************
 * @file        wdt.c
 * @author      ALIENTEK
 * @brief       Analog wdt code
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

#include "wdt.h"

esp_timer_handle_t esp_tim_handle;

/**
 * @brief       configure esptim
 * @param       tps:tps = 1s = 1000000μs
 * @retval      None
 */
void wdt_init(uint64_t tps)
{
    esp_timer_create_args_t tim_periodic_arg = {
    .callback =	&wdt_isr_handler,
    .arg = NULL,
    };

    esp_timer_create(&tim_periodic_arg, &esp_tim_handle);
    esp_timer_start_periodic(esp_tim_handle, tps);          /* cpoy and wait next packet, start timer tps ms */
}

/**
 * @brief       Restart Timer
 * @param       timeout:1s = 1000000μs
 * @retval      None
 */
void restart_timer(uint64_t timeout)
{
    esp_timer_restart(esp_tim_handle, timeout);             /* Restart Timer */
}

/**
 * @brief       Timer callback function
 * @param       arg:None
 * @retval      None
 */
void IRAM_ATTR wdt_isr_handler(void *arg)
{
    esp_restart();                                          /* reset CPU */
}

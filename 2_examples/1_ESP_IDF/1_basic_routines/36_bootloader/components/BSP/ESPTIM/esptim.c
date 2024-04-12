/**
 ****************************************************************************************************
 * @file        esptim.c
 * @author      ALIENTEK
 * @brief       ESPTIM code
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

#include "esptim.h"


uint8_t frameup;
esp_timer_handle_t esp_tim_handle;

/**
 * @brief       configure esptim
 * @param       arr : auto reload
 * @param       tps : tps = 1s = 1000000μs
 * @retval      None
 */
void esptim_int_init(uint16_t arr, uint64_t tp)
{
    timer_config_t esp_timx_handle = {0};


    esp_timer_create_args_t tim_periodic_arg = {
    .callback =	&TIM_PeriodElapsedCallback,
    .arg = NULL,
    };

    esp_timx_handle.alarm_en = TIMER_ALARM_DIS;
    esp_timx_handle.counter_en = TIMER_START;
    esp_timx_handle.intr_type = TIMER_INTR_MAX;
    esp_timx_handle.counter_dir = TIMER_COUNT_UP;
    esp_timx_handle.auto_reload = arr;
    esp_timx_handle.clk_src = TIMER_SRC_CLK_DEFAULT;

    esp_timer_create(&tim_periodic_arg, &esp_tim_handle);
    esp_timer_start_periodic(esp_tim_handle, tp);   /* cpoy and wait next packet, start timer tps ms */
}

/**
 * @brief       Timer callback function
 * @param       arg:None
 * @retval      None
 */
void TIM_PeriodElapsedCallback(void *arg)
{
    frameup = 1;
}

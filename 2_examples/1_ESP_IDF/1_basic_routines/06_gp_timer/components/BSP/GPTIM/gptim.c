/**
 ****************************************************************************************************
 * @file        gptim.c
 * @author      ALIENTEK
 * @brief       GPTIM code
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

#include "gptim.h"


QueueHandle_t queue;

/**
 * @brief       configure gptim
 * @param       counts: count
 * @param       resolution:Timer cycle,resolution = 1s = 1000000μs
 * @retval      None
 */
void gptim_int_init(uint16_t counts,uint16_t resolution)
{
    gptimer_config_t g_tim_handle;
    gptimer_alarm_config_t alarm_config;
    gptimer_event_callbacks_t g_tim_callbacks;

    uint64_t count;

    /* GPTimer handle */
    gptimer_handle_t g_tim = NULL;
    g_tim_handle.clk_src = GPTIMER_CLK_SRC_DEFAULT;
    g_tim_handle.direction = GPTIMER_COUNT_UP;
    g_tim_handle.resolution_hz = resolution;                    /* 1MHz, 1 tick = 1us */
    g_tim_callbacks.on_alarm = gptimer_callback;                /* set a new callback function */
    alarm_config.alarm_count = 1000000;                         /* period = 1s */
    ESP_ERROR_CHECK(gptimer_new_timer(&g_tim_handle, &g_tim));

    queue = xQueueCreate(10, sizeof(gptimer_event_t));

    if (!queue)
    {
        ESP_LOGE("GPTIMER_ALARM", "Missed one count event");

        return;
    }

    ESP_LOGI("GPTIMER_ALARM", "Set count value");
    ESP_ERROR_CHECK(gptimer_set_raw_count(g_tim, counts));
    ESP_LOGI("GPTIMER_ALARM", "Get count value");
    ESP_ERROR_CHECK(gptimer_get_raw_count(g_tim, &count));
    ESP_LOGI("GPTIMER_ALARM", "Timer count value=%llu", count);

    /* set a new callback function */
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(g_tim, &g_tim_callbacks, queue));

    ESP_LOGI("GPTIMER_ALARM", "Enable timer");
    ESP_ERROR_CHECK(gptimer_enable(g_tim));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(g_tim, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(g_tim));
}

/**
 * @brief       Timer callback function
 * @param       timer:Timer handle created by `gptimer_new_timer`
 * @param       edata:Alarm event data, fed by driver
 * @param       user_ctx:User data, passed from `gptimer_register_event_callbacks`
 * @retval      Whether a high priority task has been waken up by this function
 */
bool IRAM_ATTR gptimer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    queue = (QueueHandle_t)user_data;

    /* Retrieve count value and send to queue */
    gptimer_event_t ele = {
        .event_count = edata->count_value
    };

    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    
    /* reconfigure alarm value */ 
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = edata->alarm_value + 1000000, /* alarm in next 1s */ 
    };
    gptimer_set_alarm_action(timer, &alarm_config);
    
    /* return whether we need to yield at the end of ISR */ 
    return high_task_awoken == pdTRUE;
}

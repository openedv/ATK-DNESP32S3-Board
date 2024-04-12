/**
 ****************************************************************************************************
 * @file        gptim.h
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

#ifndef __GPTIM_H_
#define __GPTIM_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"


typedef struct {
    uint64_t event_count;
} gptimer_event_t;

extern QueueHandle_t queue;

/* function declaration */
void gptim_int_init(uint16_t counts,uint16_t resolution);
bool IRAM_ATTR gptimer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data);

#endif
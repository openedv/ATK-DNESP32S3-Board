/**
 ****************************************************************************************************
 * @file        wdt.h
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

#ifndef __WDT_H_
#define __WDT_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"


#define TWDT_TIMEOUT_MS         3000
#define TASK_RESET_PERIOD_MS    2000
#define MAIN_DELAY_MS           10000

/* function declaration */
void wdt_init(uint64_t tps);
void restart_timer(uint64_t timeout);
void IRAM_ATTR wdt_isr_handler(void *arg);

#endif

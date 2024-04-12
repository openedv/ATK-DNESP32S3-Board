/**
 ****************************************************************************************************
 * @file        esp_rtc.h
 * @author      ALIENTEK
 * @brief       RTC code
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

#ifndef __ESP_RTC_H
#define __ESP_RTC_H

#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <sys/time.h>


/* Time structure */
typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t year;
    uint8_t  month;
    uint8_t  date;
    uint8_t  week;
} _calendar_obj;

extern _calendar_obj calendar;

/* function declaration */
void rtc_set_time(int year,int mon,int mday,int hour,int min,int sec);
void rtc_get_time(void);
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);

#endif

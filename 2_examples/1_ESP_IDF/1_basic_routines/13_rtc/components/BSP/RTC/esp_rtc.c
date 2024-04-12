/**
 ****************************************************************************************************
 * @file        esp_rtc.c
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

#include "esp_rtc.h"


_calendar_obj calendar;         /* Time structure */

/**
 * @brief       set time
 * @param       year    :Year
 * @param       mon     :Month
 * @param       mday    :Day
 * @param       hour    :Hour
 * @param       min     :Minute
 * @param       sec     :Second
 * @retval      None
 */
void rtc_set_time(int year,int mon,int mday,int hour,int min,int sec)
{
    struct tm datetime;
    /* Setting up a false actual time - used when the file is created and for modification with the current time */
    datetime.tm_year = year - 1900;
    datetime.tm_mon = mon - 1;
    datetime.tm_mday = mday;
    datetime.tm_hour = hour;
    datetime.tm_min = min;
    datetime.tm_sec = sec;
    datetime.tm_isdst = -1;
    time_t second = mktime(&datetime);
    struct timeval val = { .tv_sec = second, .tv_usec = 0 };
    /* set time */
    settimeofday(&val, NULL);
}

/**
 * @brief       Get the current time
 * @param       None
 * @retval      Get the current time
 */
void rtc_get_time(void)
{
    struct tm *datetime;
    time_t second;
    /* check time and assert the year */
    time(&second);
    datetime = localtime(&second);

    calendar.hour = datetime->tm_hour;          /* hour */
    calendar.min = datetime->tm_min;            /* minute */
    calendar.sec = datetime->tm_sec;            /* second */
    calendar.year = datetime->tm_year + 1900;   /* Year */
    calendar.month = datetime->tm_mon + 1;      /* Month */
    calendar.date = datetime->tm_mday;          /* Day */
    /* calculate week */
    calendar.week = rtc_get_week(calendar.year, calendar.month, calendar.date);
}

/**
 * @brief       calculate week
 * @param       syear : Year
 * @param       smon  : Month
 * @param       sday  : Day
 * @retval      0:Sunday,1 ~ 6: Monday to Saturday
 */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t week = 0;

    if (month < 3)
    {
        month += 12;
        --year;
    }

    week = (day + 1 + 2 * month + 3 * (month + 1) / 5 + year + (year >> 2) - year / 100 + year / 400) % 7;
    return week;
}


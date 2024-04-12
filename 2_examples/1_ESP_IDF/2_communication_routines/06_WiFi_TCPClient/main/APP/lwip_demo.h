/**
 ****************************************************************************************************
 * @file        lwip_demo.h
 * @author      ALIENTEK
 * @brief       lwip code
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

#ifndef __LWIP_DEMO_H
#define __LWIP_DEMO_H

#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lcd.h"


#define LWIP_SEND_DATA              0X80
extern uint8_t g_lwip_send_flag;

/* function declaration */
void lwip_demo(void);

#endif

/**
 ****************************************************************************************************
 * @file        wifi_config.h
 * @author      ALIENTEK
 * @brief       wifi sta code
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

#ifndef __WIFI_CONFIG_H
#define __WIFI_CONFIG_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include <netdb.h>
#include "lcd.h"


typedef struct _network_connet_info_t
{
    uint8_t         connet_state;
    char            ip_buf[100];
    char            mac_buf[100];
    void (*fun)(uint8_t x);
} network_connet_info;

extern network_connet_info network_connet;

/* function declaration */
void wifi_sta_init(void);

#endif

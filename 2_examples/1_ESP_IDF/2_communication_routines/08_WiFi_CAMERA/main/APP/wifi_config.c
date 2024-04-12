/**
 ****************************************************************************************************
 * @file        wifi_config.c
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

#include "wifi_config.h"


/* wifi */
#define DEFAULT_SSID        "ALIENTEK-YF"
/* wifi pwd */
#define DEFAULT_PWD         "15902020353"

static EventGroupHandle_t   wifi_event;
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

network_connet_info network_connet;
static const char *TAG = "static_ip";
char lcd_buff[100] = {0};

/* wifi config */
#define WIFICONFIG()   {                            \
    .sta = {                                        \
        .ssid = DEFAULT_SSID,                       \
        .password = DEFAULT_PWD,                    \
        .threshold.authmode = WIFI_AUTH_WPA2_PSK,   \
    },                                              \
}

/**
 * @brief       connet display
 * @param       flag:2,connect;1,connect fail;0,wart connect...
 * @retval      None
 */
void connet_display(uint8_t flag)
{
    if((flag & 0x80) == 0x80)
    {
        lcd_fill(0,90,320,240,WHITE);
        sprintf(lcd_buff, "ssid:%s",DEFAULT_SSID);
        lcd_show_string(0, 90, 240, 16, 16, lcd_buff, BLUE);
        sprintf(lcd_buff, "psw:%s",DEFAULT_PWD);
        lcd_show_string(0, 110, 240, 16, 16, lcd_buff, BLUE);
        lcd_show_string(0, 130, 200, 16, 16, "KEY0:Send data", MAGENTA);
    }
    else if ((flag & 0x04) == 0x04)
    {
        lcd_show_string(0, 90, 240, 16, 16, "wifi connecting......", BLUE);
    }
    else if ((flag & 0x02) == 0x02)
    {
        lcd_show_string(0, 90, 240, 16, 16, "wifi connecting fail", BLUE);
    }
    else if ((flag & 0x01) == 0x01)
    {
        lcd_show_string(0, 150, 200, 16, 16, (char*)network_connet.ip_buf, MAGENTA);
    }

    network_connet.connet_state &= 0x00;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static int s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        network_connet.connet_state |= 0x04;
        network_connet.fun(network_connet.connet_state);
        esp_wifi_connect();
    }

    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        network_connet.connet_state |= 0x80;
        network_connet.fun(network_connet.connet_state);
    }

    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        network_connet.connet_state |= 0x02;

        if (s_retry_num < 20)
        {
            esp_wifi_connect();
            s_retry_num ++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(wifi_event, WIFI_FAIL_BIT);
            network_connet.fun(network_connet.connet_state);
        }

        ESP_LOGI(TAG,"connect to the AP fail");
    }

    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        network_connet.connet_state |= 0x01;
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        sprintf(network_connet.ip_buf, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        network_connet.fun(network_connet.connet_state);
        xEventGroupSetBits(wifi_event, WIFI_CONNECTED_BIT);
    }
}

/**
 * @brief       Initialize Wi-Fi as sta and set scan method
 * @param       None
 * @retval      None
 */
void wifi_sta_init(void)
{
    static esp_netif_t *sta_netif = NULL;
    network_connet.connet_state = 0x00;
    network_connet.fun = connet_display;

    wifi_event= xEventGroupCreate();
    /* initialize the underlying TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_init());
    /* create default event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif= esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL) );
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));    
    wifi_config_t  wifi_config = WIFICONFIG();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits( wifi_event,
                                            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                            pdFALSE,
                                            pdFALSE,
                                            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 DEFAULT_SSID, DEFAULT_PWD);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 DEFAULT_SSID, DEFAULT_PWD);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    vEventGroupDelete(wifi_event);
}

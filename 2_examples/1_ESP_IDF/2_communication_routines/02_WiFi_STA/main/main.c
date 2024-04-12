/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       wifi sta example
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include <netdb.h>
#include "led.h"
#include "lcd.h"


i2c_obj_t i2c0_master;
/* wifi */
#define DEFAULT_SSID        "123"
/* wifi pwd */
#define DEFAULT_PWD         "aa1234567"

static EventGroupHandle_t   wifi_event;
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1
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
 * @param       flag:2,connect;1,connect fail;0,connect...
 * @retval      None
 */
void connet_display(uint8_t flag)
{
    if(flag == 2)
    {
        lcd_fill(0,90,320,240,WHITE);
        sprintf(lcd_buff, "ssid:%s",DEFAULT_SSID);
        lcd_show_string(0, 90, 240, 16, 16, lcd_buff, BLUE);
        sprintf(lcd_buff, "psw:%s",DEFAULT_PWD);
        lcd_show_string(0, 110, 240, 16, 16, lcd_buff, BLUE);
    }
    else if (flag == 1)
    {
        lcd_show_string(0, 90, 240, 16, 16, "wifi connecting fail", BLUE);
    }
    else
    {
        lcd_show_string(0, 90, 240, 16, 16, "wifi connecting......", BLUE);
    }
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static int s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        connet_display(0);
        esp_wifi_connect();
    }

    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        connet_display(2);
    }

    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {

        if (s_retry_num < 20)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(wifi_event, WIFI_FAIL_BIT);
        }

        ESP_LOGI(TAG,"connect to the AP fail");
    }

    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
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

    EventBits_t bits = xEventGroupWaitBits(wifi_event,
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
        connet_display(1);
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 DEFAULT_SSID, DEFAULT_PWD);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    vEventGroupDelete(wifi_event);
}

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;

    ret = nvs_flash_init();             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                         /* Initialize led */
    i2c0_master = iic_init(I2C_NUM_0);  /* Initialize iic0 */
    spi2_init();                        /* Initialize spi2 */
    xl9555_init(i2c0_master);           /* Initialize xl9555 */
    lcd_init();                         /* Initialize lcd */

    lcd_show_string(0, 0, 240, 32, 32, "ESP32-S3", RED);
    lcd_show_string(0, 40, 240, 24, 24, "WiFi STA Test", RED);
    lcd_show_string(0, 70, 240, 16, 16, "ATOM@ALIENTEK", RED);
    wifi_sta_init();

    while (1)
    {
        LED_TOGGLE();
        vTaskDelay(500);
    }
}

/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       wifi ap example
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
static const char *TAG = "AP";
#define EXAMPLE_ESP_WIFI_SSID   "123"
#define EXAMPLE_ESP_WIFI_PASS   "123456789"
#define EXAMPLE_MAX_STA_CONN    5
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
static char lcd_buff[100] = {0};


static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        lcd_fill(0,90,320,240,WHITE);
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
                 MAC2STR(event->mac), event->aid);
        sprintf(lcd_buff, "MACSTR:"MACSTR,MAC2STR(event->mac));
        lcd_show_string(0, 90, 320, 16, 16, lcd_buff, BLUE);
        lcd_show_string(0, 110, 320, 16, 16, "With device connection", BLUE);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
        lcd_fill(0,90,320,320,WHITE);
        sprintf(lcd_buff, "Device disconnected:"MACSTR,MAC2STR(event->mac));
        lcd_show_string(0, 90, 320, 16, 16, lcd_buff, BLUE);
    }
}

/**
 * @brief       Initialize Wi-Fi
 * @param       None
 * @retval      None
 */
static void wifi_init_softap(void)
{
    /* initialize the underlying TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_init());

    /* create default event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

    char ip_addr[16];
    inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
    ESP_LOGI(TAG, "Set up softAP with IP: %s", ip_addr);

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:'%s' password:'%s'",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    
    lcd_show_string(0, 90, 240, 16, 16, "wifi connecting......", BLUE);
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
    lcd_show_string(0, 40, 240, 24, 24, "WiFi AP Test", RED);
    lcd_show_string(0, 70, 240, 16, 16, "ATOM@ALIENTEK", RED);
    wifi_init_softap();

    while (1)
    {
        LED_TOGGLE();
        vTaskDelay(500);
    }
}

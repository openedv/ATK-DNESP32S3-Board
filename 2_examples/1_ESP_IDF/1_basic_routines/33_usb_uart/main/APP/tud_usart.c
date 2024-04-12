/**
 ****************************************************************************************************
 * @file        tud_usart.c
 * @author      ALIENTEK
 * @brief       TUD_USART code
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

#include "tud_usart.h"


static const char *TAG = "usb_msc";
static uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];

/**
 * @brief       CDC RX Callback
 * @param       itf     :CDC port
 * @param       event   :CDC event
 * @retval      None
 */
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    /* initialization */
    size_t rx_size = 0;

    /* read */
    esp_err_t ret = tinyusb_cdcacm_read(itf, buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);

    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Data from channel %d:", itf);
        ESP_LOG_BUFFER_HEXDUMP(TAG, buf, rx_size, ESP_LOG_INFO);
    }
    else
    {
        ESP_LOGE(TAG, "Read error");
    }

    /* write back */
    tinyusb_cdcacm_write_queue(itf, buf, rx_size);
    tinyusb_cdcacm_write_flush(itf, 0);
}

/**
 * @brief       state changed callback
 * @param       itf     :CDC port
 * @param       event   :CDC event
 * @retval      None
 */
void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    int dtr = event->line_state_changed_data.dtr;
    int rts = event->line_state_changed_data.rts;
    ESP_LOGI(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
}

/**
 * @brief       SD card simulation USB function initialization
 * @param       None
 * @retval      None
 */
void tud_usb_usart(void)
{
    ESP_LOGI(TAG, "USB initialization");
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false,
        .configuration_descriptor = NULL,
    };

    /* This is an all-in-one helper function */
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    /* Configuration structure for CDC-ACM */
    tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,                /* Usb device to set up */
        .cdc_port = TINYUSB_CDC_ACM_0,              /* CDC port */
        .rx_unread_buf_sz = 64,                     /* Amount of data that can be passed to the ACM at once */
        .callback_rx = &tinyusb_cdc_rx_callback,    /* the first way to register a callback */
        .callback_rx_wanted_char = NULL,            /* Pointer to the function with the `tusb_cdcacm_callback_t` type that will be handled as a callback */
        .callback_line_state_changed = NULL,        /* Pointer to the function with the `tusb_cdcacm_callback_t` type that will be handled as a callback */
        .callback_line_coding_changed = NULL        /* Pointer to the function with the `tusb_cdcacm_callback_t` type that will be handled as a callback */
    };
    /* the second way to register a callback */
    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    /* Register a callback invoking on CDC event */
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                        TINYUSB_CDC_ACM_0,
                        CDC_EVENT_LINE_STATE_CHANGED,
                        &tinyusb_cdc_line_state_changed_callback));

    ESP_LOGI(TAG, "USB initialization DONE");
}

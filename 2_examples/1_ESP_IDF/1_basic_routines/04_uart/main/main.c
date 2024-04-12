/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       UART example
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

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "led.h"
#include "usart.h"


/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;
    uint8_t len = 0;
    uint16_t times = 0;
    unsigned char data[RX_BUF_SIZE] = {0};

    ret = nvs_flash_init();                                                             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    led_init();                                                                         /* Initialize LED */
    usart_init(115200);                                                                 /* Initialize UART */

    while(1)
    {
        uart_get_buffered_data_len(USART_UX, (size_t*) &len);                           /*  UART get RX ring buffer cached data length */

        if (len > 0)
        {
            memset(data, 0, RX_BUF_SIZE);
            printf("\nThe message you sent is:\n");
            uart_read_bytes(USART_UX, data, len, 100);                                  /* Read slave_data from UART */
            uart_write_bytes(USART_UX, (const char*)data, strlen((const char*)data));   /* Send data to the UART port from a given buffer and lengt */
        }
        else
        {
            times++;

            if (times % 5000 == 0)
            {
                printf("\nALIENTEK ATK-DNESP32-S3 Board \n");
            }

            if (times % 200 == 0)
            {
                printf("please enter data and end with enter key\n");
            }

            if (times % 30 == 0)
            {
                LED_TOGGLE();
            }

            vTaskDelay(10);
        }
    }
}

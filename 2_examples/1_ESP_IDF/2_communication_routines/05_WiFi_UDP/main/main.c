/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       wifi udp example
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
#include "esp_system.h"
#include "nvs_flash.h"
#include "led.h"
#include "lcd.h"
#include "wifi_config.h"
#include "lwip_demo.h"

/*FreeRTOS*********************************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/******************************************************************************************************/
/* FreeRTOS Configuration */
/* LED_TASK Task Configuration
 * Includes: Task Handle, Task Priority, Stack Size, Task Creation
 */
#define LED_TASK_PRIO           10          /* task priority */
#define LED_STK_SIZE            2048        /* task stack size */
TaskHandle_t LEDTask_Handler;               /* task handle */
void led_task(void *pvParameters);          /* task function */

/* KEY_TASK Task Configuration
 * Includes: Task Handle, Task Priority, Stack Size, Task Creation
 */
#define KEY_TASK_PRIO           11          /* task priority */
#define KEY_STK_SIZE            2048        /* task stack size */
TaskHandle_t KEYTask_Handler;               /* task handle */
void key_task(void *pvParameters);          /* task function */

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;
i2c_obj_t i2c0_master;

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
    lcd_show_string(0, 40, 240, 24, 24, "WiFi UDP Test", RED);
    lcd_show_string(0, 70, 240, 16, 16, "ATOM@ALIENTEK", RED);
    wifi_sta_init();
    taskENTER_CRITICAL(&my_spinlock);

    xTaskCreate((TaskFunction_t )key_task,
                (const char *   )"key_task",
                (uint16_t       )KEY_STK_SIZE,
                (void *         )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t * )&KEYTask_Handler);

    xTaskCreate((TaskFunction_t )led_task,
                (const char*    )"led_task",
                (uint16_t       )LED_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LEDTask_Handler);
    taskEXIT_CRITICAL(&my_spinlock);
    lwip_demo();
}

/**
 * @brief       key task function
 * @param       pvParameters : None
 * @retval      None
 */
void key_task(void *pvParameters)
{
    pvParameters = pvParameters;
    
    uint8_t key;
  
    while (1)
    {
        key = xl9555_key_scan(0);

        if (KEY0_PRES == key)
        {
            g_lwip_send_flag |= LWIP_SEND_DATA;
        }
        
        vTaskDelay(10);
    }
}

/**
 * @brief       runing task
 * @param       pvParameters : None
 * @retval      None
 */
void led_task(void *pvParameters)
{
    pvParameters = pvParameters;

    while (1)
    {
        LED_TOGGLE();
        vTaskDelay(500);
    }
}

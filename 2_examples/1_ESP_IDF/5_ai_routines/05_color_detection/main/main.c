/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       color detection example
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
#include "led.h"
#include "lcd.h"
#include "xl9555.h"
#include "camera.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_color_detection.hpp"


i2c_obj_t i2c0_master;
unsigned long x_i = 0;
unsigned long x_j = 0;
extern QueueHandle_t xQueueAIFrameO;
camera_fb_t *face_ai_frameO = NULL;

/**
 * @brief       color face detection
 * @param       x      : The x-coordinate of the string to be displayed
 * @param       y      : The y-coordinate of the string to be displayed
 * @retval      None
 */
void lcd_color_detection_camera(uint16_t x, uint16_t y)
{
    if (xQueueReceive(xQueueAIFrameO, &face_ai_frameO, portMAX_DELAY))
    {
        lcd_set_window(x, y, x + face_ai_frameO->width - 1, y + face_ai_frameO->height - 1);

        if (x + face_ai_frameO->width > lcd_self.width || y + face_ai_frameO->height > lcd_self.height)
        {
            goto err;
        }

        for (x_j = 0; x_j < face_ai_frameO->width * face_ai_frameO->height; x_j++)
        {
            lcd_buf[2 * x_j] = (face_ai_frameO->buf[2 * x_i]) ;
            lcd_buf[2 * x_j + 1] =  (face_ai_frameO->buf[2 * x_i + 1]);
            x_i ++;
        }
        
        for (x_j = 0; x_j < (face_ai_frameO->width * face_ai_frameO->height * 2 / LCD_BUF_SIZE); x_j++)
        {
            lcd_write_data(&lcd_buf[x_j * LCD_BUF_SIZE] , LCD_BUF_SIZE);
        }
err:
        esp_camera_fb_return(face_ai_frameO);
        x_i = 0;
        face_ai_frameO = NULL;
    }
}

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    uint8_t x = 0;
    esp_err_t ret;
    
    ret= nvs_flash_init();                  /* initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    led_init();                             /* initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);      /* initialize IIC0 */
    spi2_init();                            /* initialize SPI2 */
    xl9555_init(i2c0_master);               /* initialize XL9555 */
    lcd_init();                             /* initialize LCD */

    lcd_show_string(30, 50, 200, 16, 16, "ESP32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "COLOR DETECTION TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    /* initialize CAMERA */
    while (camera_init())
    {
        lcd_show_string(30, 110, 200, 16, 16, "CAMERA Fail!", BLUE);
        vTaskDelay(500);
    }

    lcd_clear(BLACK);

    /* enable ai */
    while (esp_color_detection_ai_strat())
    {
        lcd_show_string(30, 110, 200, 16, 16, "Create Task/Queue Fail!", BLUE);
        /* disable ai */
        esp_color_detection_ai_deinit();
        vTaskDelay(500);
    }

    while (1)
    {
        lcd_color_detection_camera(42,0);
        
        x++;

        if (x % 20 == 0)
        {
            LED_TOGGLE();
        }

        vTaskDelay(1);
    }
}

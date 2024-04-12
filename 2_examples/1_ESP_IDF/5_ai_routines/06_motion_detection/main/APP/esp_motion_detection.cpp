/**
 ****************************************************************************************************
 * @file        esp_motion_detection.cpp
 * @author      ALIENTEK
 * @brief       motion detection code
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

#include "esp_motion_detection.hpp"
#include "dl_image.hpp"


TaskHandle_t camera_task_handle;
TaskHandle_t ai_task_handle;
QueueHandle_t xQueueFrameO = NULL;
QueueHandle_t xQueueAIFrameO = NULL;


/**
 * @brief       Get camera data
 * @param       arg:not used
 * @retval      None
 */
static void esp_camera_process_handler(void *arg)
{
    arg = arg;
    camera_fb_t *camera_frame = NULL;

    while (1)
    {
        /* Obtain pointer to a frame buffer */
        camera_frame = esp_camera_fb_get();

        if (camera_frame)
        {
            xQueueSend(xQueueFrameO, &camera_frame, portMAX_DELAY);
        }
    }
}

/**
 * @brief       ai process task
 * @param       arg:not used
 * @retval      None
 */
static void esp_ai_process_handler(void *arg)
{
    arg = arg;
    camera_fb_t *face_ai_frameI = NULL;
    camera_fb_t *face_ai_frameI2 = NULL;

    while(1)
    {
        if (xQueueReceive(xQueueFrameO, &face_ai_frameI, portMAX_DELAY))
        {
            if (xQueueReceive(xQueueFrameO, &face_ai_frameI2, portMAX_DELAY))
            {
                uint32_t moving_point_number = dl::image::get_moving_point_number((uint16_t *)face_ai_frameI->buf, (uint16_t *)face_ai_frameI2->buf, face_ai_frameI->height, face_ai_frameI->width, 8, 15);

                if (moving_point_number > 50)
                {
                    printf("Something moved\r\n");
                    dl::image::draw_filled_rectangle((uint16_t *)face_ai_frameI2->buf, face_ai_frameI2->height, face_ai_frameI2->width, 0, 0, 40, 40);
                }
                else
                {
                    printf("Something not moved\r\n");
                }
                
                esp_camera_fb_return(face_ai_frameI);
                xQueueSend(xQueueAIFrameO, &face_ai_frameI2, portMAX_DELAY);
            }
        }
    }
}

/**
 * @brief       Enable AI
 * @param       None
 * @retval      1:Create Fail;0:Create success
 */
uint8_t esp_motion_detection_ai_strat(void)
{
    xQueueFrameO = xQueueCreate(5, sizeof(camera_fb_t *));
    xQueueAIFrameO = xQueueCreate(5, sizeof(camera_fb_t *));
    xTaskCreatePinnedToCore(esp_camera_process_handler, "esp_camera_process_handler", 4 * 1024, NULL, 5, &camera_task_handle, 1);
    xTaskCreatePinnedToCore(esp_ai_process_handler, "esp_ai_process_handler", 6 * 1024, NULL, 5, &ai_task_handle, 1);

    if (xQueueFrameO != NULL 
        || xQueueAIFrameO != NULL 
        || camera_task_handle != NULL 
        || ai_task_handle != NULL)
    {
        return 0;
    }
    
    return 1;
}

/**
 * @brief       Disable AI
 * @param       None
 * @retval      None
 */
void esp_motion_detection_ai_deinit(void)
{
    if (xQueueFrameO != NULL)
    {
        vQueueDelete(xQueueFrameO);
    }

    if (xQueueAIFrameO != NULL)
    {
        vQueueDelete(xQueueAIFrameO);
    }

    if (camera_task_handle != NULL)
    {
        vTaskDelete(camera_task_handle);
    }

    if (ai_task_handle != NULL)
    {
        vTaskDelete(ai_task_handle);
    }
}

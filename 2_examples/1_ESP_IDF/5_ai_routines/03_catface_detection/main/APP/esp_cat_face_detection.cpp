/**
 ****************************************************************************************************
 * @file        esp_cat_face_detection.cpp
 * @author      ALIENTEK
 * @brief       cat face detection code
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

#include "esp_cat_face_detection.hpp"
#include "dl_image.hpp"
#include "cat_face_detect_mn03.hpp"
#include "who_ai_utils.hpp"


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
    CatFaceDetectMN03 detector(0.4F, 0.3F, 10, 0.3F);

    while(1)
    {
        if (xQueueReceive(xQueueFrameO, &face_ai_frameI, portMAX_DELAY))
        {
            std::list<dl::detect::result_t> &detect_results = detector.infer((uint16_t *)face_ai_frameI->buf, {(int)face_ai_frameI->height, (int)face_ai_frameI->width, 3});
            
            if (detect_results.size() > 0)
            {
                ESP_LOGE("Camera", "Cat Face detected");
                /* Draw detection result on RGB565 image */
                draw_detection_result((uint16_t *)face_ai_frameI->buf, face_ai_frameI->height, face_ai_frameI->width, detect_results);
            }
            else
            {
                ESP_LOGE("Camera", "Cat Face not detected");
            }

            xQueueSend(xQueueAIFrameO, &face_ai_frameI, portMAX_DELAY);
        }
    }
}

/**
 * @brief       Enable AI
 * @param       None
 * @retval      1:Create Fail;0:Create success
 */
uint8_t esp_cat_face_detection_ai_strat(void)
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
void esp_cat_face_detection_ai_deinit(void)
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
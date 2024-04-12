/**
 ****************************************************************************************************
 * @file        esp_cat_face_detection.c
 * @author      ALIENTEK
 * @brief       cat_face code
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

extern "C" {
#include "esp_config.h"
#include "esp_log.h"
}
#include "dl_image.hpp"
#include "cat_face_detect_mn03.hpp"
#include "who_ai_utils.hpp"


static TaskHandle_t camera_task_handle;
static TaskHandle_t ai_task_handle;
static QueueHandle_t xQueueFrameO = NULL;


/**
 * @brief       Camera image data acquisition task
 * @param       arg:None
 * @retval      None
 */
static void esp_camera_process_handler(void *arg)
{
    arg = arg;
    camera_fb_t *camera_frame = NULL;

    while (1)
    {
        /* Obtain camera images */
        camera_frame = esp_camera_fb_get();

        if (camera_frame)
        {
            xQueueSend(xQueueFrameO, &camera_frame, portMAX_DELAY);
        }
    }
}

/**
 * @brief       AI processing tasks
 * @param       arg:None
 * @retval      None
 */
static void esp_ai_process_handler(void *arg)
{
    arg = arg;
    camera_fb_t *face_ai_frameI = NULL;
    CatFaceDetectMN03 detector(0.4F, 0.3F, 10, 0.3F);

    while(1)
    {
        /* receive camera data */
        if (xQueueReceive(xQueueFrameO, &face_ai_frameI, portMAX_DELAY))
        {
            std::list<dl::detect::result_t> &detect_results = detector.infer((uint16_t *)face_ai_frameI->buf, {(int)face_ai_frameI->height, (int)face_ai_frameI->width, 3});
            
            if (detect_results.size() > 0)
            {
                /* AI processing */
                draw_detection_result((uint16_t *)face_ai_frameI->buf, face_ai_frameI->height, face_ai_frameI->width, detect_results);
            }

            xQueueSend(xQueueAIFrameO, &face_ai_frameI, portMAX_DELAY);
        }
    }
}

/**
 * @brief       Enable AI
 * @param       None
 * @retval      1:Creation failed.0:Created successfully
 */
mp_obj_t esp_cat_face_detection_ai_strat(void)
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
        return mp_const_none;
    }

    return mp_const_true;
}

/**
 * @brief       Turn off AI
 * @param       None
 * @retval      1:failed.0:successfully
 */
mp_obj_t esp_cat_face_detection_ai_deinit(void)
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
	
	return mp_const_true;
}

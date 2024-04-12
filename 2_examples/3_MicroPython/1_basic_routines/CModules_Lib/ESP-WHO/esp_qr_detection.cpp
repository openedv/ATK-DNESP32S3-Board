/**
 ****************************************************************************************************
 * @file        esp_qr_detection.c
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
#include "esp_code_scanner.h"
}
#include "dl_image.hpp"


static TaskHandle_t camera_task_handle;

/**
 * @brief       QR code recognition
 * @param       camera_frame:Image pointer
 * @retval      None
 */
extern "C" void esp_qr_scanner(camera_fb_t *camera_frame)
{
    esp_image_scanner_t *esp_scn = esp_code_scanner_create();
    esp_code_scanner_config_t config = {ESP_CODE_SCANNER_MODE_FAST, ESP_CODE_SCANNER_IMAGE_RGB565, camera_frame->width, camera_frame->height};
    esp_code_scanner_set_config(esp_scn, config);
    int decoded_num = esp_code_scanner_scan_image(esp_scn, camera_frame->buf);

    if (decoded_num)
    {
        dl::image::draw_filled_rectangle((uint16_t *)camera_frame->buf, camera_frame->height, camera_frame->width, 0, 0, 20, 20);
        esp_code_scanner_symbol_t result = esp_code_scanner_result(esp_scn);
        printf("Decoded %s symbol \"%s\"\n", result.type_name, result.data);
    }
    
    esp_code_scanner_destroy(esp_scn);
}

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
            /* QR code recognition */
            esp_qr_scanner(camera_frame);
            xQueueSend(xQueueAIFrameO, &camera_frame, portMAX_DELAY);
        }
    }
}

/**
 * @brief       Enable AI
 * @param       None
 * @retval      1:Creation failed.0:Created successfully
 */
mp_obj_t esp_qr_detection_ai_strat(void)
{
    xQueueAIFrameO = xQueueCreate(5, sizeof(camera_fb_t *));
    xTaskCreatePinnedToCore(esp_camera_process_handler, "esp_camera_process_handler", 4 * 1024, NULL, 5, &camera_task_handle, 1);
    
    if (xQueueAIFrameO != NULL 
        || camera_task_handle != NULL)
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
mp_obj_t esp_qr_detection_ai_deinit(void)
{
    if (xQueueAIFrameO != NULL)
    {
        vQueueDelete(xQueueAIFrameO);
    }

    if (camera_task_handle != NULL)
    {
        vTaskDelete(camera_task_handle);
    }
	
	return mp_const_true;
}

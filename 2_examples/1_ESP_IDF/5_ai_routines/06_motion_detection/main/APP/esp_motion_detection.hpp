/**
 ****************************************************************************************************
 * @file        esp_motion_detection.hpp
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

#ifndef __ESP_MOTION_DETECTION_H
#define __ESP_MOTION_DETECTION_H

#include "esp_camera.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


#ifdef __cplusplus
extern "C" {
#endif

extern QueueHandle_t xQueueAIFrameO;

/* function declaration */
uint8_t esp_motion_detection_ai_strat(void);
void esp_motion_detection_ai_deinit(void);

#ifdef __cplusplus
}
#endif

#endif

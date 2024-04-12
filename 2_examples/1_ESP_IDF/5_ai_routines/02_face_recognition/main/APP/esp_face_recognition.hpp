/**
 ****************************************************************************************************
 * @file        esp_face_recognition.hpp
 * @author      ALIENTEK
 * @brief       face recognition code
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

#ifndef __ESP_FACE_DETECTION_H
#define __ESP_FACE_DETECTION_H

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

/* Key state */
typedef enum
{
    KEY_SHORT_PRESS = 1,        /* Short press status */
    KEY_LONG_PRESS,             /* Long press status */
    KEY_DOUBLE_CLICK,           /* Double click status */
} key_state_t;

/* Key scanning status */
typedef struct
{
    gpio_num_t      io_num;     /* KEY IO */
    key_state_t     state;      /* Key state */
} key_scan_state_t;

typedef enum
{
    IDLE = 0,                   /* Idle state */
    DETECT,                     /* Detection status */
    ENROLL,                     /* Registration status */
    RECOGNIZE,                  /* Identify status */
    DELETE,                     /* deleted state */
} recognizer_state_t;

typedef enum
{
    SHOW_STATE_IDLE,            /* Idle */
    SHOW_STATE_DELETE,          /* Delete */
    SHOW_STATE_RECOGNIZE,       /* Identification */
    SHOW_STATE_ENROLL,          /* Registration */
} show_state_t;

#define RGB565_MASK_RED         0xF800
#define RGB565_MASK_GREEN       0x07E0
#define RGB565_MASK_BLUE        0x001F
#define FRAME_DELAY_NUM         16

/* Long press or double-click time */
#define LONG_PRESS_THRESH       700000
#define DOUBLE_CLICK_THRESH     300000

/* Function declaration */
uint8_t esp_face_recognition_ai_strat(void);
void esp_face_recognition_ai_deinit(void);

#ifdef __cplusplus
}
#endif

#endif

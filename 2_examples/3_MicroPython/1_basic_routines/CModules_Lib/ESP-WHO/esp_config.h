/**
 ****************************************************************************************************
 * @file        esp_config.h
 * @author      ALIENTEK
 * @brief       esp_config code
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

#ifndef __ESP_CONFIG_H
#define __ESP_CONFIG_H

#include "py/runtime.h"
#include "esp_camera.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


/*****************************Color recognition*****************************/
#define RGB565_LCD_RED          0x00F8
#define RGB565_LCD_ORANGE       0x20FD
#define RGB565_LCD_YELLOW       0xE0FF
#define RGB565_LCD_GREEN        0xE007
#define RGB565_LCD_CYAN         0xFF07
#define RGB565_LCD_BLUE         0x1F00
#define RGB565_LCD_PURPLE       0x1EA1
#define RGB565_LCD_WHITE        0xFFFF
#define RGB565_LCD_GRAY         0x1084
#define RGB565_LCD_BLACK        0x0000

#define RGB565_MASK_RED         0xF800
#define RGB565_MASK_GREEN       0x07E0
#define RGB565_MASK_BLUE        0x001F
#define FRAME_DELAY_NUM         16
/*****************************Color recognition*****************************/

/* function */
#define FACE_DETECTION          0  /* Facial recognition */
#define FACE_RECONGNITION       1  /* Facial detection */
#define QR_RECONGNITION         2  /* QR code recognition */
#define COLOR_RECONGNITION      3  /* Color recognition */
#define CAT_DETECTION           4  /* Cat face recognition */

/*****************************Facial detection*****************************/
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

/*****************************Facial detection*****************************/

extern QueueHandle_t xQueueAIFrameO;

/* Function declaration */
extern mp_obj_t esp_face_detection_ai_strat(void);
extern mp_obj_t esp_face_detection_ai_deinit(void);
extern mp_obj_t esp_face_recognition_ai_strat(void);
extern mp_obj_t esp_face_recognition_ai_deinit(void);
extern mp_obj_t esp_cat_face_detection_ai_strat(void);
extern mp_obj_t esp_cat_face_detection_ai_deinit(void);
extern mp_obj_t esp_qr_detection_ai_strat(void);
extern mp_obj_t esp_qr_detection_ai_deinit(void);
extern mp_obj_t esp_color_detection_ai_strat(void);
extern mp_obj_t esp_color_detection_ai_deinit(void);
extern mp_obj_t esp_motion_detection_ai_strat(void);
extern mp_obj_t esp_motion_detection_ai_deinit(void);


#endif

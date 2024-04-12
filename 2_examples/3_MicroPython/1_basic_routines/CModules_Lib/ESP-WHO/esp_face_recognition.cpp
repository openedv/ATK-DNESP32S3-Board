/**
 ****************************************************************************************************
 * @file        esp_face_detection.c
 * @author      ALIENTEK
 * @brief       esp_face_detection code
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
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"
#include "face_recognition_tool.hpp"
#include "face_recognition_112_v1_s16.hpp"
#include "who_ai_utils.hpp"
#include "fb_gfx.h"


static TaskHandle_t camera_task_handle;
static TaskHandle_t ai_task_handle;
static QueueHandle_t xQueueFrameO = NULL;
static QueueHandle_t xQueueKeyState = NULL;
static QueueHandle_t gpio_evt_queue = NULL;
static QueueHandle_t xQueueEventLogic = NULL;
static SemaphoreHandle_t xMutex;
static recognizer_state_t gEvent = DETECT;
static bool gReturnFB = true;
static face_info_t recognize_result;
static recognizer_state_t recognizer_state;

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
 * @brief       print
 * @param       fb   :camera hardle
 * @param       color:color
 * @param       str  :data
 * @retval      None
 */
static void esp_rgb_print(camera_fb_t *fb, uint32_t color, const char *str)
{
    fb_gfx_print(fb, (fb->width - (strlen(str) * 14)) / 2, 10, color, str);
}

/**
 * @brief       print
 * @param       fb    :camera hardle
 * @param       color :color
 * @param       format:data
 * @retval      lenght
 */
static int esp_rgb_printf(camera_fb_t *fb, uint32_t color, const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);

    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            return 0;
        }
    }

    vsnprintf(temp, len + 1, format, arg);
    va_end(arg);
    esp_rgb_print(fb, color, temp);

    if (len > 64)
    {
        free(temp);
    }

    return len;
}

/**
 * @brief       AI processing tasks
 * @param       arg:None
 * @retval      None
 */
static void esp_ai_process_handler(void *arg)
{
    arg = arg;
    camera_fb_t *frame = NULL;
    HumanFaceDetectMSR01 detector(0.3F, 0.3F, 10, 0.3F);
    HumanFaceDetectMNP01 detector2(0.4F, 0.3F, 10);
    FaceRecognition112V1S16 *recognizer = new FaceRecognition112V1S16();

    show_state_t frame_show_state = SHOW_STATE_IDLE;
    recognizer_state_t _gEvent;
    recognizer->set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr");
    int partition_result = recognizer->set_ids_from_flash();

    while(1)
    {
        xSemaphoreTake(xMutex, portMAX_DELAY);
        _gEvent = gEvent;
        gEvent = DETECT;
        xSemaphoreGive(xMutex);

        if (_gEvent)
        {
            bool is_detected = false;

            if (xQueueReceive(xQueueFrameO, &frame, portMAX_DELAY))
            {
                std::list<dl::detect::result_t> &detect_candidates = detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
                std::list<dl::detect::result_t> &detect_results = detector2.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);

                if (detect_results.size() == 1)
                    is_detected = true;

                if (is_detected)
                {
                    switch (_gEvent)
                    {
                        /* register */
                        case ENROLL:
                            recognizer->enroll_id((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint, "", true);
                            ESP_LOGW("ENROLL", "ID %d is enrolled", recognizer->get_enrolled_ids().back().id);
                            frame_show_state = SHOW_STATE_ENROLL;
                            break;
                        /* recognition */
                        case RECOGNIZE:
                            recognize_result = recognizer->recognize((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint);
                            print_detection_result(detect_results);
                            if (recognize_result.id > 0)
                                printf("Similarity: %f, Match ID: %d", recognize_result.similarity, recognize_result.id);
                            else
                                printf("Similarity: %f, Match ID: %d", recognize_result.similarity, recognize_result.id);
                            frame_show_state = SHOW_STATE_RECOGNIZE;
                            break;
                        /* delete */
                        case DELETE:
                            vTaskDelay(10);
                            recognizer->delete_id(true);
                            printf("% d IDs left", recognizer->get_enrolled_id_num());
                            frame_show_state = SHOW_STATE_DELETE;
                            break;

                        default:
                            break;
                    }
                }

                if (frame_show_state != SHOW_STATE_IDLE)
                {
                    static int frame_count = 0;

                    switch (frame_show_state)
                    {
                        /* Delete Image */
                        case SHOW_STATE_DELETE:
                            esp_rgb_printf(frame, RGB565_MASK_RED, "%d IDs left", recognizer->get_enrolled_id_num());
                            break;
                        /* image recognition */
                        case SHOW_STATE_RECOGNIZE:
                            if (recognize_result.id > 0)
                                esp_rgb_printf(frame, RGB565_MASK_GREEN, "ID %d", recognize_result.id);
                            else
                                esp_rgb_print(frame, RGB565_MASK_RED, "who ?");
                            break;
                        /* Image registration */
                        case SHOW_STATE_ENROLL:
                            esp_rgb_printf(frame, RGB565_MASK_BLUE, "Enroll: ID %d", recognizer->get_enrolled_ids().back().id);
                            break;

                        default:
                            break;
                    }

                    if (++frame_count > FRAME_DELAY_NUM)
                    {
                        frame_count = 0;
                        frame_show_state = SHOW_STATE_IDLE;
                    }
                }

                if (detect_results.size())
                {
                    draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
                }
            }

            if (xQueueAIFrameO)
            {

                xQueueSend(xQueueAIFrameO, &frame, portMAX_DELAY);
            }
            else if (gReturnFB)
            {
                esp_camera_fb_return(frame);
            }
            else
            {
                free(frame);
            }
        }
    }
}

/**
 * @brief       Button interrupt callback function
 * @param       arg:id
 * @retval      None
 */
static void IRAM_ATTR esp_gpio_isr_handler_key(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

/**
 * @brief       Key scanning (to determine the status of short press, long press, and double click)
 * @param       ticks_to_wait:delay
 * @retval      Return button status
 */
int esp_key_scan(TickType_t ticks_to_wait)
{
    gpio_num_t io_num;
    BaseType_t press_key = pdFALSE;
    BaseType_t lift_key = pdFALSE;
    int64_t backup_time = 0;
    int64_t interval_time = 0;
    static int64_t last_time = 0;

    while (1)
    {
        xQueueReceive(gpio_evt_queue, &io_num, ticks_to_wait);

        if (gpio_get_level(io_num) == 0)
        {
            press_key = pdTRUE;
            backup_time = esp_timer_get_time();
            interval_time = backup_time - last_time;
        }
        else if (press_key)
        {
            lift_key = pdTRUE;
            last_time = esp_timer_get_time();
            backup_time = last_time - backup_time;
        }

        if (press_key & lift_key)
        {
            press_key = pdFALSE;
            lift_key = pdFALSE;

            if (backup_time > LONG_PRESS_THRESH)
            {
                return KEY_LONG_PRESS;
            }
            else
            {
                if ((interval_time < DOUBLE_CLICK_THRESH) && (interval_time > 0))
                    return KEY_DOUBLE_CLICK;
                else
                    return KEY_SHORT_PRESS;
            }
        }
    }
}

/**
 * @brief       key task
 * @param       arg:None
 * @retval      None
 */
static void esp_key_trigger(void *arg)
{
    arg = arg;
    int ret = 0;

    while (1)
    {
        ret = esp_key_scan(portMAX_DELAY);
        xQueueOverwrite(xQueueKeyState, &ret);
    }

    vTaskDelete(NULL);
}

/**
 * @brief       key init
 * @param       gpio_num:gpio num
 * @retval      None
 */
void esp_key_init(gpio_num_t gpio_num)
{
    gpio_config_t io_conf = {0};
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = 1ull << gpio_num;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    gpio_evt_queue = xQueueCreate(5, sizeof(uint32_t));
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, esp_gpio_isr_handler_key, (void *)gpio_num);
}

/**
 * @brief       event
 * @param       arg:None
 * @retval      None
 */
void esp_event_generate(void *arg)
{
    arg = arg;
    static key_state_t key_state;

    while (1)
    {
        xQueueReceive(xQueueKeyState, &key_state, portMAX_DELAY);
        switch (key_state)
        {
            case KEY_SHORT_PRESS: 
                recognizer_state = RECOGNIZE;
                break;

            case KEY_LONG_PRESS:
                recognizer_state = ENROLL;
                break;

            case KEY_DOUBLE_CLICK:
                recognizer_state = DELETE;
                break;

            default:
                recognizer_state = DETECT;
                break;
        }

        xQueueSend(xQueueEventLogic, &recognizer_state, portMAX_DELAY);
    }
}

/**
 * @brief       Key scanning task
 * @param       arg:None
 * @retval      None
 */
static void esp_task_event_handler(void *arg)
{
    arg = arg;
    recognizer_state_t _gEvent;

    while (1)
    {
        xQueueReceive(xQueueEventLogic, &(_gEvent), portMAX_DELAY);
        xSemaphoreTake(xMutex, portMAX_DELAY);
        gEvent = _gEvent;
        xSemaphoreGive(xMutex);
    }
}

/**
 * @brief       Enable AI
 * @param       None
 * @retval      1:Creation failed.0:Created successfully
 */
mp_obj_t esp_face_recognition_ai_strat(void)
{
    xQueueFrameO = xQueueCreate(5, sizeof(camera_fb_t *));
    xQueueAIFrameO = xQueueCreate(5, sizeof(camera_fb_t *));
    xQueueKeyState = xQueueCreate(1, sizeof(int *));
    xQueueEventLogic = xQueueCreate(1, sizeof(int *));
    xMutex = xSemaphoreCreateMutex();
    esp_key_init(GPIO_NUM_0);
    xTaskCreatePinnedToCore(esp_key_trigger, "esp_key_scan_task", 1024, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(esp_event_generate, "event_logic_task", 1024, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(esp_camera_process_handler, "esp_camera_process_handler", 4 * 1024, NULL, 5, &camera_task_handle, 1);
    xTaskCreatePinnedToCore(esp_ai_process_handler, "esp_ai_process_handler", 6 * 1024, NULL, 5, &ai_task_handle, 1);
    xTaskCreatePinnedToCore(esp_task_event_handler, "esp_task_event_handler", 4 * 1024, NULL, 5, NULL, 1);
    
    if (xQueueFrameO != NULL 
        || xQueueAIFrameO != NULL 
        || xQueueEventLogic != NULL
        || camera_task_handle != NULL 
        || ai_task_handle != NULL)
    {
        return mp_const_none;
    }

    return mp_const_true;
}

/**
 * @brief       Disable AI
 * @param       None
 * @retval      1:failed.0:successfully
 */
mp_obj_t esp_face_recognition_ai_deinit(void)
{
    if (xQueueFrameO != NULL)
    {
        vQueueDelete(xQueueFrameO);
    }

    if (xQueueAIFrameO != NULL)
    {
        vQueueDelete(xQueueAIFrameO);
    }

    if (xQueueEventLogic != NULL)
    {
        vQueueDelete(xQueueEventLogic);
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

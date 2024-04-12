/**
 ****************************************************************************************************
 * @file        esp_color_detection.cpp
 * @author      ALIENTEK
 * @brief       color detection code
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

#include "esp_color_detection.hpp"
#include "dl_image.hpp"
#include "fb_gfx.h"
#include "color_detector.hpp"


using namespace std;
using namespace dl;

TaskHandle_t camera_task_handle;
TaskHandle_t ai_task_handle;
QueueHandle_t xQueueFrameO = NULL;
QueueHandle_t xQueueAIFrameO = NULL;
static bool draw_box = true;

vector<color_info_t> std_color_info = {{{156, 10, 70, 255, 90, 255}, 64, "red"},
                                       {{11, 22, 70, 255, 90, 255}, 64, "orange"},
                                       {{23, 33, 70, 255, 90, 255}, 64, "yellow"},
                                       {{34, 75, 70, 255, 90, 255}, 64, "green"},
                                       {{76, 96, 70, 255, 90, 255}, 64, "cyan"},
                                       {{97, 124, 70, 255, 90, 255}, 64, "blue"},
                                       {{125, 155, 70, 255, 90, 255}, 64, "purple"},
                                       {{0, 180, 0, 40, 220, 255}, 64, "white"},
                                       {{0, 180, 0, 50, 50, 219}, 64, "gray"},
                                        {{0, 180, 0, 255, 0, 45}, 64, "black"}
                                       };

static void esp_draw_color_detection_result(uint16_t *image_ptr, int image_height, int image_width, vector<color_detect_result_t> &results, uint16_t color)
{
    for (int i = 0; i < results.size(); ++i)
    {
        dl::image::draw_hollow_rectangle(image_ptr, image_height, image_width,
                                         results[i].box[0],
                                         results[i].box[1],
                                         results[i].box[2],
                                         results[i].box[3],
                                         color);
    }
}

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

    ColorDetector detector;
    detector.set_detection_shape({80, 80, 1});

    for (int i = 0; i < std_color_info.size(); ++i)
    {
        detector.register_color(std_color_info[i].color_thresh, std_color_info[i].area_thresh, std_color_info[i].name);
    }

    vector<vector<int>> color_thresh_boxes = {{110, 110, 130, 130}, {100, 100, 140, 140}, {90, 90, 150, 150}, {80, 80, 160, 160}, {60, 60, 180, 180}, {40, 40, 200, 200}, {20, 20, 220, 220}};
    int color_thresh_boxes_num = color_thresh_boxes.size();
    int color_thresh_boxes_index = color_thresh_boxes_num / 2;
    vector<int> color_area_threshes = {1, 4, 16, 32, 64, 128, 256, 512, 1024};
    int color_area_thresh_num = color_area_threshes.size();
    int color_area_thresh_index = color_area_thresh_num / 2;
    
    detector.set_area_thresh({color_area_threshes[color_area_thresh_index]});


    vector<uint16_t> draw_lcd_colors = {RGB565_LCD_RED, 
                                        RGB565_LCD_ORANGE, 
                                        RGB565_LCD_YELLOW,
                                        RGB565_LCD_GREEN, 
                                        RGB565_LCD_CYAN, 
                                        RGB565_LCD_BLUE,
                                        RGB565_LCD_PURPLE, 
                                        RGB565_LCD_WHITE, 
                                        RGB565_LCD_GRAY, 
                                        RGB565_LCD_BLACK
                                        };

    int draw_colors_num = draw_lcd_colors.size();
    vector<uint8_t> color_thresh;

    while(1)
    {
        if (xQueueReceive(xQueueFrameO, &face_ai_frameI, portMAX_DELAY))
        {
            std::vector<std::vector<color_detect_result_t>> &results = detector.detect((uint16_t *)face_ai_frameI->buf, {(int)face_ai_frameI->height, (int)face_ai_frameI->width, 3});
            
            if (draw_box)
            {
                for (int i = 0; i < results.size(); ++i)
                {
                    esp_draw_color_detection_result((uint16_t *)face_ai_frameI->buf, (int)face_ai_frameI->height, (int)face_ai_frameI->width, results[i], draw_lcd_colors[i % draw_colors_num]);
                }
            }
            else
            {
                detector.draw_segmentation_results((uint16_t *)face_ai_frameI->buf, {(int)face_ai_frameI->height, (int)face_ai_frameI->width, 3}, draw_lcd_colors, true, 0x0000);
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
uint8_t esp_color_detection_ai_strat(void)
{
    xQueueFrameO = xQueueCreate(5, sizeof(camera_fb_t *));
    xQueueAIFrameO = xQueueCreate(5, sizeof(camera_fb_t *));
    xTaskCreatePinnedToCore(esp_camera_process_handler, "esp_camera_process_handler", 6 * 1024, NULL, 5, &camera_task_handle, 1);
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
void esp_color_detection_ai_deinit(void)
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

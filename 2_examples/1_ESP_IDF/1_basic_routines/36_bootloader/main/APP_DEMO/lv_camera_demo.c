/**
 ****************************************************************************************************
 * @file        lv_camera_demo.c
 * @author      ALIENTEK
 * @brief       LVGL CAMERA Demo code
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

#include "lv_camera_demo.h"


lv_camera_struct camera;
camera_fb_t *camera_fb = NULL;
uint8_t * camera_video_buf;
LV_IMG_DECLARE(ui_img_pattern_png);

lv_img_dsc_t img_dsc = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = NULL,
};

/* TASK1 Task Configuration
 * Including: task handle, task priority, stack size, creation task
 */
#define TASK1_PRIO      10                  /* task priority */
#define TASK1_STK_SIZE  5*1024              /* task stack size */
TaskHandle_t            Task1Task_Handler;  /* task handle */
void task1(void *pvParameters);             /* Task function */


/**
  * @brief  del camera
  * @param  None
  * @retval None
  */
void lv_camera_del(void)
{
    if (Task1Task_Handler != NULL)
    {
        vTaskDelete(Task1Task_Handler);
        vTaskDelay(10);
        Task1Task_Handler = NULL;
    }

    if (camera_video_buf != NULL)
    {
        heap_caps_free(camera_video_buf);
        camera_video_buf = NULL;
    }

    esp_camera_deinit();

    lv_obj_clean(app_obj_general.del_parent);
    lv_obj_del(app_obj_general.del_parent);
    app_obj_general.del_parent = NULL;
    lv_app_show();
}

/**
 * @brief       task1
 * @param       pvParameters : parameters (not used)
 * @retval      None
 */
void task1(void *pvParameters)
{
    pvParameters = pvParameters;
    unsigned long iw = 0;
    unsigned long jw = 0;
    camera_video_buf = heap_caps_malloc(240 * 176 * 2, MALLOC_CAP_8BIT);

    while (1)
    {
        camera_fb = esp_camera_fb_get();

        img_dsc.header.w = camera_fb->width;
        img_dsc.header.h = camera_fb->height;
        img_dsc.data_size = camera_fb->width * camera_fb->height * 2;
       
        memset(camera_video_buf,0,img_dsc.data_size);

        for (jw = 0; jw < camera_fb->width * camera_fb->height; jw++)
        {
            camera_video_buf[2 * jw] =  (camera_fb->buf[2 * iw + 1]);
            camera_video_buf[2 * jw + 1] =  (camera_fb->buf[2 * iw]);
            iw ++;
        }

        xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
        img_dsc.data = (const uint8_t *)camera_video_buf;
        lv_img_set_src(camera.camera_buf.camera_header,&img_dsc);
        xSemaphoreGive(xGuiSemaphore);

        esp_camera_fb_return(camera_fb);
        camera_fb = NULL;
        iw = 0;
        vTaskDelay(5);
    }
}

/**
  * @brief  camera demo
  * @param  None
  * @retval None
  */
void lv_camera_demo(void)
{
    esp_err_t err = ESP_OK;

    if (app_obj_general.current_parent != NULL)
    {
        lv_obj_del(app_obj_general.current_parent);
        app_obj_general.current_parent = NULL;
    }

    err = camera_init();

    if (err != ESP_OK)
    {
        lv_msgbox("Camera device not detected");
    }
    else
    {
        lv_app_del();

        camera.lv_camera_cont = lv_obj_create(lv_scr_act());
        lv_obj_set_style_radius(camera.lv_camera_cont, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(camera.lv_camera_cont,LV_OPA_0,LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(camera.lv_camera_cont, lv_color_make(0,0,0), LV_STATE_DEFAULT);
        lv_obj_set_size(camera.lv_camera_cont,lcd_self.width,lcd_self.height);
        lv_obj_set_pos(camera.lv_camera_cont,0,0);
        lv_page_tile(camera.lv_camera_cont,"CAMERA");

        camera.camera_buf.camera_header = lv_img_create(camera.lv_camera_cont);
        lv_obj_set_style_bg_color(camera.camera_buf.camera_header, lv_color_make(0,0,0), LV_STATE_DEFAULT);
        lv_obj_center(camera.camera_buf.camera_header);
        lv_group_add_obj(ctrl_g, camera.camera_buf.camera_header);
        lv_group_focus_obj(camera.camera_buf.camera_header);
        lv_obj_clear_flag(camera.lv_camera_cont, LV_OBJ_FLAG_SCROLLABLE);
        app_obj_general.del_parent = camera.lv_camera_cont;
        app_obj_general.Function = lv_camera_del;

        if (Task1Task_Handler == NULL)
        {
            xTaskCreatePinnedToCore((TaskFunction_t )task1,
                                    (const char*    )"task1",
                                    (uint16_t       )TASK1_STK_SIZE,
                                    (void*          )NULL,
                                    (UBaseType_t    )TASK1_PRIO,
                                    (TaskHandle_t*  )&Task1Task_Handler,
                                    (BaseType_t     ) 1);
        }
    }
}

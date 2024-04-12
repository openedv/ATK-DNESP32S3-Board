/**
 ****************************************************************************************************
 * @file        lvgl_demo.c
 * @author      ALIENTEK
 * @brief       LVGL DEMO code
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

#include "lvgl_demo.h"
#include "demos/lv_demos.h"
#include "app_ui.h"
#include "image.h"
#include "sdmmc_cmd.h"


lv_indev_t *indev_keypad;
uint32_t back_act_key = 0;
uint8_t sd_check_en = 0;
uint8_t decode_en = ESP_OK;
SemaphoreHandle_t xGuiSemaphore;

/* function declaration */
static void lvgl_disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void keypad_init(void);
static void keypad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static uint32_t keypad_get_key(void);
void lv_port_indev_init(void);
void increase_lvgl_tick(void *arg);
void lv_port_disp_init(void);


/* WATCH Task Configuration
 * Including: task handle, task priority, stack size, creation task
 */
#define WATCH_PRIO      1                   /* task priority */
#define WATCH_STK_SIZE  5*1024              /* task stack size */
TaskHandle_t            WATCHTask_Handler;  /* task handle */
void watch(void *pvParameters);             /* Task function */

SemaphoreHandle_t BinarySemaphore;

/**
 * @brief       watch task
 * @param       pvParameters : parameters (not used)
 * @retval      None
 */
void watch(void *pvParameters)
{
    pvParameters = pvParameters;

    while(1)
    {
        LED_TOGGLE();

        vTaskDelay(500);
    }
}

/**
 * @brief       lvgl_demo
 * @param       None
 * @retval      None
 */
void lvgl_demo(void)
{
    while(images_init())
    {
        while (sd_spi_init())
        {
            lcd_show_string(30, 50, 200, 16, 16, "SD Card Failed!", RED);
            vTaskDelay(200);
            lcd_show_string(30, 50, 200, 16, 16, "               ", WHITE);
            vTaskDelay(200);
        }

        sd_check_en = 1;
        
        lcd_show_string(30, 50, 200, 16, 16, "image Error!", RED);
        images_update_image(30, 90, 16, (uint8_t *)"0:", RED);
        vTaskDelay(1000);
        lcd_clear(WHITE);
    }

    if (xl9555_key_scan(0) == KEY3_PRES)
    {
        images_update_image(30, 90, 16, (uint8_t *)"0:", RED);
    }
    
    BinarySemaphore = xSemaphoreCreateBinary();
    xGuiSemaphore = xSemaphoreCreateMutex();

    if (WATCHTask_Handler == NULL)
    {
        xTaskCreatePinnedToCore((TaskFunction_t )watch,
                                (const char*    )"watch",
                                (uint16_t       )WATCH_STK_SIZE,
                                (void*          )NULL,
                                (UBaseType_t    )WATCH_PRIO,
                                (TaskHandle_t*  )&WATCHTask_Handler,
                                (BaseType_t     ) 1);
    }

    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 1 * 1000));

    lv_load_main_window();

    while (1)
    {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_timer_handler();
            xSemaphoreGive(xGuiSemaphore);
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/**
 * @brief       Initialize disp
 * @param       None
 * @retval      None
 */
void lv_port_disp_init(void)
{
    void *buf1 = NULL;
    void *buf2 = NULL;

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    buf1 = heap_caps_malloc(lcd_self.width * 10 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, lcd_self.width * 10);  /* Initialize the display buffer */
    
    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;      /* Descriptor of a display driver */
    lv_disp_drv_init(&disp_drv);        /* Basic initialization */
    
    /* Set up the functions to access to your display */

    /* Set the resolution of the display */
    disp_drv.hor_res = lcd_self.width;
    disp_drv.ver_res = lcd_self.height;

    /* Used to copy the buffer's content to the display */
    disp_drv.flush_cb = lvgl_disp_flush_cb;

    /* Set a display buffer */
    disp_drv.draw_buf = &disp_buf;

    /* Finally register the driver */
    lv_disp_drv_register(&disp_drv);
}

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
static void lvgl_disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    lcd_color_fill(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_map);
    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing */
    lv_disp_flush_ready(drv);
}

/**
 * @brief       lvgl tick
 * @param       arg : parameters (not used)
 * @retval      None
 */
void increase_lvgl_tick(void *arg)
{
    lv_tick_inc(1);
}

/**
 * @brief       Initialize indev
 * @param       None
 * @retval      None
 */
void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;

    /* Register a keypad input device */
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&indev_drv);
}

uint32_t g_last_key = 0;

/* Will be called by the library to read the mouse */
static void keypad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    /* Get whether the a key is pressed and save the pressed key */
    uint32_t act_key = keypad_get_key();

    if(act_key != 0) 
    {
        data->state = LV_INDEV_STATE_PR;

        /* Translate the keys to LVGL control characters according to your key definitions */
        switch(act_key) 
        {
            case KEY0_PRES:
                act_key = LV_KEY_NEXT;
            break;

            case KEY1_PRES:
                act_key = KEY1_PRES;
                back_act_key = KEY1_PRES;
            break;

            case KEY2_PRES:
                act_key = LV_KEY_PREV;
            break;
            
            case KEY3_PRES:
                act_key = LV_KEY_ENTER;
            break;
        }

        g_last_key = act_key;
    } 
    else 
    {
        data->state = LV_INDEV_STATE_REL;
        g_last_key = 0;
    }

    if (back_act_key == KEY1_PRES && decode_en == ESP_OK)
    {
        if (app_obj_general.current_parent != NULL || app_obj_general.del_parent != NULL)
        {
            app_obj_general.Function();
        }

        back_act_key = 0;
    }

    data->key = g_last_key;
}

/* Get the currently being pressed key.  0 if no key is pressed */
static uint32_t keypad_get_key(void)
{
    return xl9555_key_scan(0);
}

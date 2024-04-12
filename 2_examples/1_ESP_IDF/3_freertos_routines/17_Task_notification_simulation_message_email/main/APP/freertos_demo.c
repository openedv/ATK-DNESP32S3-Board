/**
 ****************************************************************************************************
 * @file        freertos_demo.c
 * @author      ALIENTEK
 * @brief       task_notification_simulation_message_email example
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

#include "freertos_demo.h"
#include "lcd.h"
#include "led.h"
/*FreeRTOS*********************************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/******************************************************************************************************/
/*FreeRTOS configuration*/

/* TASK1 task configuration
 * include: Task handle,Task Priority,Stack size,Create a task function
 */
#define TASK1_PRIO      1                   
#define TASK1_STK_SIZE  5*1024              
TaskHandle_t            Task1Task_Handler;  
void task1(void *pvParameters);   

/* TASK2 task configuration
 * include: Task handle,Task Priority,Stack size,Create a task function
 */
#define TASK2_PRIO      2                   
#define TASK2_STK_SIZE  5*1024              
TaskHandle_t            Task2Task_Handler; 
void task2(void *pvParameters);   

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;

/******************************************************************************************************/

/* lcd brush color */
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};

/**
 * @brief       FreeRTOS entry function
 * @param       None
 * @retval      None
 */
void freertos_demo(void)
{
    lcd_show_string(10, 10, 220, 32, 32, "ESP32-S3", RED);
    lcd_show_string(10, 47, 220, 24, 24, "Notify MailBox", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_draw_rectangle(5, 125, 234, 314, BLACK);        /* draw rectangle */
    
    taskENTER_CRITICAL(&my_spinlock);
    /* Create Task 1 */
    xTaskCreatePinnedToCore((TaskFunction_t )task1,                 /* task function */
                            (const char*    )"task1",               /* task name */
                            (uint16_t       )TASK1_STK_SIZE,        /* task stack size */
                            (void*          )NULL,                  /* task function parameters */
                            (UBaseType_t    )TASK1_PRIO,            /* task priority */
                            (TaskHandle_t*  )&Task1Task_Handler,    /* task handle */
                            (BaseType_t     ) 0);                   /* kernel runs the task */
    /* Create Task 2 */
    xTaskCreatePinnedToCore((TaskFunction_t )task2,                 
                            (const char*    )"task2",               
                            (uint16_t       )TASK2_STK_SIZE,        
                            (void*          )NULL,                  
                            (UBaseType_t    )TASK2_PRIO,            
                            (TaskHandle_t*  )&Task2Task_Handler,    
                            (BaseType_t     ) 0);   
    taskEXIT_CRITICAL(&my_spinlock);
}

/**
 * @brief       task1
 * @param       pvParameters : passed parameter(not used)
 * @retval      None
 */
void task1(void *pvParameters)
{
    pvParameters = pvParameters;
    uint8_t key = 0;
    
    while (1)
    {
        key = xl9555_key_scan(0);
        
        if ((Task2Task_Handler != NULL) && (key != 0))
        {
            xTaskNotify((TaskHandle_t   )Task2Task_Handler,         /* task handle that receives task notifications */
                        (uint32_t       )key,                       /* the bit to update */
                        (eNotifyAction  )eSetValueWithOverwrite);   /* the update mode is overwrite */
        }
        
        vTaskDelay(10);
    }
}

/**
 * @brief       task2
 * @param       pvParameters : passed parameter(not used)
 * @retval      None
 */
void task2(void *pvParameters)
{
    pvParameters = pvParameters;
    uint32_t    notify_val  = 0;
    uint32_t    task2_num   = 0;
    
    while (1)
    {
        xTaskNotifyWait((uint32_t     )0x00000000,      /* the task notification value is not cleared when entering the function */
                        (uint32_t     )0xFFFFFFFF,      /* when the function exits, clear the task notification value */
                        (uint32_t*    )&notify_val,     /* the value of the received notification */
                        (TickType_t   )portMAX_DELAY);  /* wait time */
        
        switch (notify_val)
        {
            case KEY0_PRES:                             /* lcd area refresh */
            {
                lcd_fill(6, 126, 233, 313, lcd_discolor[++task2_num % 11]);
                break;
            }
            case KEY1_PRES:                             
            {
                LED_TOGGLE();
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

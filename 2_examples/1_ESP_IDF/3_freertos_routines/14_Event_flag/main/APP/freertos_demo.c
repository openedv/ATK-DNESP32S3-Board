/**
 ****************************************************************************************************
 * @file        freertos_demo.c
 * @author      ALIENTEK
 * @brief       event_flag example
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
#include "freertos/event_groups.h"

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

/* TASK3 task configuration
 * include: Task handle,Task Priority,Stack size,Create a task function
 */
#define TASK3_PRIO      4                   
#define TASK3_STK_SIZE  5*1024              
TaskHandle_t            Task3Task_Handler;  
void task3(void *pvParameters);

EventGroupHandle_t EventGroupHandler;       /* event flag group handle */

#define EVENTBIT_0      (1 << 0)            /* event bit */
#define EVENTBIT_1      (1 << 1)
#define EVENTBIT_ALL    (EVENTBIT_0 | EVENTBIT_1)

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;

/******************************************************************************************************/


/**
 * @brief       FreeRTOS entry function
 * @param       None
 * @retval      None
 */
void freertos_demo(void)
{
    lcd_show_string(10, 10, 220, 32, 32, "ESP32-S3", RED);
    lcd_show_string(10, 47, 220, 24, 24, "Event Group", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 220, 16, 16, "Event Group Value: 0", BLUE);
    taskENTER_CRITICAL(&my_spinlock);
    /* Create an event flag group */
    EventGroupHandler = xEventGroupCreate();
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

    /* Create Task 3 */
    xTaskCreatePinnedToCore((TaskFunction_t )task3,                
                            (const char*    )"task3",               
                            (uint16_t       )TASK3_STK_SIZE,        
                            (void*          )NULL,                  
                            (UBaseType_t    )TASK3_PRIO,           
                            (TaskHandle_t*  )&Task3Task_Handler,    
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
        
        switch (key)
        {
            case KEY0_PRES:
            {
                xEventGroupSetBits((EventGroupHandle_t  )EventGroupHandler, /* event flag group handle to be operated */
                                   (EventBits_t         )EVENTBIT_0);       /* bit to be set */
                break;
            }
            case KEY1_PRES:
            {
                xEventGroupSetBits((EventGroupHandle_t  )EventGroupHandler, 
                                   (EventBits_t         )EVENTBIT_1);       
                break;
            }
            default:
            {
                break;
            }
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
    uint32_t task2_num = 0;
    
    while (1)
    {
        xEventGroupWaitBits((EventGroupHandle_t )EventGroupHandler, /* wait event flag group handle */
                            (EventBits_t        )EVENTBIT_ALL,      /* wait event */
                            (BaseType_t         )pdTRUE,            /* the event that clears the zero wait when the function exits */
                            (BaseType_t         )pdTRUE,            /* wait for all events in the waiting event */
                            (TickType_t         )portMAX_DELAY);    /* wait time */
        vTaskDelay(10);
    }
}

/**
 * @brief       task3
 * @param       pvParameters : passed parameter(not used)
 * @retval      None
 */
void task3(void *pvParameters)
{
    pvParameters = pvParameters;
    EventBits_t event_val = 0;
    
    while (1)
    {
        event_val = xEventGroupGetBits((EventGroupHandle_t)EventGroupHandler);  /* get the event flag group handle */
        
        lcd_show_xnum(182, 110, event_val, 1, 16, 0, BLUE);                     /* lcd display event value */
        
        vTaskDelay(10);
    }
}

/**
 ****************************************************************************************************
 * @file        freertos_demo.c
 * @author      ALIENTEK
 * @brief       task_suspension_and_recovery example
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

/* TASK3 task configuration
 * include: Task handle,Task Priority,Stack size,Create a task function
 */
#define TASK3_PRIO      4                   
#define TASK3_STK_SIZE  5*1024              
TaskHandle_t            Task3Task_Handler;  
void task3(void *pvParameters);

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
    lcd_show_string(10, 47, 220, 24, 24, "Task Susp & Resum", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Task1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Task2: 000", BLUE);
    
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
}

/**
 * @brief       task1
 * @param       pvParameters : passed parameter(not used)
 * @retval      None
 */
void task1(void *pvParameters)
{
    pvParameters = pvParameters;
    uint32_t task1_num = 0;
    
    while (1)
    {
        taskENTER_CRITICAL(&my_spinlock);
        lcd_fill(6, 131, 114, 313, lcd_discolor[++task1_num % 11]);
        lcd_show_xnum(71, 111, task1_num, 3, 16, 0x80, BLUE);
        taskEXIT_CRITICAL(&my_spinlock);
        vTaskDelay(1000);
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
        taskENTER_CRITICAL(&my_spinlock);
        lcd_fill(126, 131, 233, 313, lcd_discolor[11 - (++task2_num % 11)]);
        lcd_show_xnum(191, 111, task2_num, 3, 16, 0x80, BLUE);
        taskEXIT_CRITICAL(&my_spinlock);
        vTaskDelay(1000);
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
    uint8_t key = 0;
    uint8_t timer = 0;

    while (1)
    {
        key = xl9555_key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                vTaskSuspend(Task1Task_Handler);
                break;
            }
            case KEY1_PRES:
            {
                vTaskResume(Task1Task_Handler);
                break;
            }
            default:
            {
                break;
            }
        }
        
        timer ++;

        if (timer % 30 == 0)
        {
            LED_TOGGLE();
        }

        vTaskDelay(10);
    }
}

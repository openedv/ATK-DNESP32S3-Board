/**
 ****************************************************************************************************
 * @file        freertos_demo.c
 * @author      ALIENTEK
 * @brief       queue_set example
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
#include "freertos/queue.h"

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

static QueueSetHandle_t xQueueSet;                  /* define queue set */
QueueHandle_t           xQueue1;                    /* define queue 1 */
QueueHandle_t           xQueue2;                    /* define queue 2 */
SemaphoreHandle_t       xSemaphore;                 /* define a binary semaphore */

#define QUEUE_LENGTH            1                   /* number of messages supported by the queue */
#define QUEUE_ITEM_SIZE         sizeof(uint8_t)     /* the size of each message in the queue */
#define SEMAPHORE_BINARY_LENGTH 1                   /* the effective length of a binary semaphore */
#define QUEUESET_LENGTH         ((2 * QUEUE_LENGTH) + SEMAPHORE_BINARY_LENGTH)  /* number of messages supported by the queue set */

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
    lcd_show_string(10, 47, 220, 24, 24, "Queue set", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    taskENTER_CRITICAL(&my_spinlock);
    /* Creates a queue set */
    xQueueSet = xQueueCreateSet(QUEUESET_LENGTH);
    /* Creates a queue */
    xQueue1 = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    xQueue2 = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    /* Creates a binary semaphore */
    xSemaphore = xSemaphoreCreateBinary();
    /* Adds the queue and binary semaphore to the queue set */
    xQueueAddToSet(xQueue1, xQueueSet);
    xQueueAddToSet(xQueue2, xQueueSet);
    xQueueAddToSet(xSemaphore, xQueueSet);

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
    uint8_t timer = 0;

    while (1)
    {
        key = xl9555_key_scan(0);

        switch (key)
        {
            case KEY0_PRES:                     /* queue 1 sends the message */
            {
                xQueueSend(xQueue1, &key, portMAX_DELAY);
                break;
            }
            case KEY1_PRES:                     /* queue 2 sends the message */
            {
                xQueueSend(xQueue2, &key, portMAX_DELAY);
                break;
            }
            case KEY2_PRES:                     /* release binary semaphore */
            {
                xSemaphoreGive(xSemaphore);
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

/**
 * @brief       task2
 * @param       pvParameters : passed parameter(not used)
 * @retval      None
 */
void task2(void *pvParameters)
{
    pvParameters = pvParameters;
    QueueSetMemberHandle_t activate_member = NULL;
    uint8_t queue_recv = 0;
    
    while (1)
    {
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);/* wait for the queue in the queue set to receive the message */
        
        if (activate_member == xQueue1)
        {
            xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
            printf("receive xQueue1 message: %d\r\n", queue_recv);
        }
        else if (activate_member == xQueue2)
        {
            xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
            printf("receive xQueue2 message:: %d\r\n", queue_recv);
        }
        else if (activate_member == xSemaphore)
        {
            xSemaphoreTake(activate_member, portMAX_DELAY);
            printf("A binary semaphore is obtained: xSemaphore\r\n");
        }
    }
}

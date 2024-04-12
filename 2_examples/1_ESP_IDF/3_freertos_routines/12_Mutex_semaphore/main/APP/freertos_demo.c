/**
 ****************************************************************************************************
 * @file        freertos_demo.c
 * @author      ALIENTEK
 * @brief       mutex_semaphore example
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
#define TASK1_PRIO      4                   
#define TASK1_STK_SIZE  5*1024              
TaskHandle_t            Task1Task_Handler; 
void task1(void *pvParameters);             

/* TASK2 task configuration
 * include: Task handle,Task Priority,Stack size,Create a task function
 */
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  5*1024              
TaskHandle_t            Task2Task_Handler;  
void task2(void *pvParameters);             

/* TASK3 task configuration
 * include: Task handle,Task Priority,Stack size,Create a task function
 */
#define TASK3_PRIO      2                   
#define TASK3_STK_SIZE  5*1024              
TaskHandle_t            Task3Task_Handler;  
void task3(void *pvParameters);             

SemaphoreHandle_t MutexSemaphore;   /* mutex semaphores  */

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
    lcd_show_string(10, 47, 220, 24, 24, "Mutex Semaphore", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    taskENTER_CRITICAL(&my_spinlock);
    /* 创建互斥信号量 */
    MutexSemaphore = xSemaphoreCreateMutex();
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
 * @brief       delay function
 * @param       ts : clock count
 * @retval      None
 */
static __inline void delay_clock(int ts)
{
    uint32_t start, curr;
 
    __asm__ __volatile__("rsr %0, ccount" : "=r"(start));
    do
    {
        __asm__ __volatile__("rsr %0, ccount" : "=r"(curr));
        
    }while (curr - start <= ts);
}

/**
 * @brief       microsecond delay
 * @param       us : delay microsecond
 * @retval      None
 */
void sys_delays_us(int us)
{
    while (us--)
    {
        delay_clock(240);
    }
}

/**
 * @brief       millisecond delay
 * @param       ms : delay millisecond
 * @retval      None
 */
void sys_delays_ms(int ms)
{
    while (ms--)
    {
        delay_clock(240 * 1000);
    }
}

/**
 * @brief       task1
 * @param       pvParameters : passed parameter(not used)
 * @retval      None
 */
void task1(void *pvParameters)
{
    pvParameters = pvParameters;
    vTaskDelay(500);

    while (1)
    {
        printf("task1 ready to take semaphore\r\n");
        xSemaphoreTake(MutexSemaphore, portMAX_DELAY);  /* get a mutually exclusive semaphore */
        printf("task1 has taked semaphore\r\n");
        printf("task1 running\r\n");
        printf("task1 give semaphore\r\n");
        xSemaphoreGive(MutexSemaphore);                 /* release a mutually exclusive semaphore */
        vTaskDelay(100);
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
    
    vTaskDelay(200);

    while (1)
    {
        for (task2_num = 0; task2_num < 5; task2_num ++)
        {
            printf("task2 running\r\n");
            sys_delays_ms(100);     /* the simulated operation does not trigger task scheduling */
        }

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
    uint32_t task3_num = 0;
    
    while (1)
    {
        printf("task3 ready to take semaphore\r\n");
        xSemaphoreTake(MutexSemaphore, portMAX_DELAY);      /* get a mutually exclusive semaphore */
        printf("task3 has taked semaphore\r\n");

        for (task3_num = 0; task3_num < 5; task3_num ++)
        {
            printf("task3 running\r\n");
            sys_delays_ms(100);                             /* the simulated operation does not trigger task scheduling */
        }

        printf("task3 give semaphore\r\n");
        xSemaphoreGive(MutexSemaphore);                     /* release a mutually exclusive semaphore */
        vTaskDelay(1000);
    }
}

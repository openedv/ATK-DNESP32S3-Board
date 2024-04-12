/**
 ****************************************************************************************************
 * @file        freertos_demo.c
 * @author      ALIENTEK
 * @brief       software_timers example
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
#include "freertos/timers.h"

/******************************************************************************************************/
/*FreeRTOS configuration*/

/* TASK1 task configuration
 * include: Task handle,Task Priority,Stack size,Create a task function
 */
#define TASK1_PRIO      1                   
#define TASK1_STK_SIZE  5*1024              
TaskHandle_t            Task1Task_Handler;  
void task1(void *pvParameters);   

TimerHandle_t           Timer1Timer_Handler;    /* Timer 1 handle */
TimerHandle_t           Timer2Timer_Handler;    /* Timer 2 handle */

void Timer1Callback(TimerHandle_t xTimer);      /* Timer 1 timeout callback function */
void Timer2Callback(TimerHandle_t xTimer);      /* Timer 2 timeout callback function */

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
    lcd_show_string(10, 47, 220, 24, 24, "Timer", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Timer1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Timer2: 000", BLUE);
    taskENTER_CRITICAL(&my_spinlock);
    /* Timer 1 Creates a periodic timer */
    Timer1Timer_Handler = xTimerCreate((const char*  )"Timer1",                  /* timer name */
                                       (TickType_t   )1000,                      /* timer timeout */
                                       (UBaseType_t  )pdTRUE,                    /* cycle timer */
                                       (void*        )1,                         /* timer id */
                                       (TimerCallbackFunction_t)Timer1Callback); /* timer callback function */
    /* Timer 2 Create a single-timer */
    Timer2Timer_Handler = xTimerCreate((const char*  )"Timer2",                  
                                      (TickType_t    )1000,                      
                                      (UBaseType_t   )pdFALSE,                   
                                      (void*         )2,                         
                                      (TimerCallbackFunction_t)Timer2Callback);  
    /* Create Task 1 */
    xTaskCreatePinnedToCore((TaskFunction_t )task1,                 /* task function */
                            (const char*    )"task1",               /* task name */
                            (uint16_t       )TASK1_STK_SIZE,        /* task stack size */
                            (void*          )NULL,                  /* task function parameters */
                            (UBaseType_t    )TASK1_PRIO,            /* task priority */
                            (TaskHandle_t*  )&Task1Task_Handler,    /* task handle */
                            (BaseType_t     ) 0);                   /* kernel runs the task */
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
        if ((Timer1Timer_Handler != NULL) && (Timer2Timer_Handler != NULL))
        {
            key = xl9555_key_scan(0);
            
            switch (key)
            {
                case KEY0_PRES:
                {
                    xTimerStart((TimerHandle_t  )Timer1Timer_Handler,   /* indicate the handle of the timer to be started */
                                (TickType_t     )portMAX_DELAY);        /* the maximum time for waiting for the system to start the timer */
                    xTimerStart((TimerHandle_t  )Timer2Timer_Handler,   
                                (TickType_t     )portMAX_DELAY);        
                    break;
                }
                case KEY1_PRES:
                {
                    xTimerStop( (TimerHandle_t  )Timer1Timer_Handler,   /* handle of the timer to be stopped */
                                (TickType_t     )portMAX_DELAY);        /* Maximum time for waiting for the system to stop the timer */
                    xTimerStop( (TimerHandle_t  )Timer2Timer_Handler,   
                                (TickType_t     )portMAX_DELAY);        
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        
        vTaskDelay(10);
    }
}

/**
 * @brief       Timer1 timeout callback function
 * @param       xTimer : passed parameter(not used)
 * @retval      None
 */
void Timer1Callback(TimerHandle_t xTimer)
{
    static uint32_t timer1_num = 0;
    
    lcd_fill(6, 131, 114, 313, lcd_discolor[++timer1_num % 11]);    /* lcd area refresh */
    lcd_show_xnum(79, 111, timer1_num, 3, 16, 0x80, BLUE);          /* lcd display the timeout times of timer 1 */
}

/**
 * @brief       Timer2 timeout callback function
 * @param       xTimer : passed parameter(not used)
 * @retval      None
 */
void Timer2Callback(TimerHandle_t xTimer)
{
    static uint32_t timer2_num = 0;
    
    lcd_fill(126, 131, 233, 313, lcd_discolor[++timer2_num % 11]);  /* lcd area refresh */
    lcd_show_xnum(199, 111, timer2_num, 3, 16, 0x80, BLUE);         /* lcd display the timeout times of timer 2 */
}

/**
 ****************************************************************************************************
 * @file        freertos_demo.c
 * @author      ALIENTEK
 * @brief       insertion_and_deletion_list_items example
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

List_t                  TestList;           /* define test list */
ListItem_t              ListItem1;          /* define list item1 */
ListItem_t              ListItem2;          /* define list item2 */
ListItem_t              ListItem3;          /* define list item3 */
/******************************************************************************************************/

/**
 * @brief       FreeRTOS entry function
 * @param       None
 * @retval      None
 */
void freertos_demo(void)
{
    lcd_show_string(10, 10, 220, 32, 32, "ESP32-S3", RED);
    lcd_show_string(10, 47, 220, 24, 24, "List & ListItem", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    /* Create Task 1 */
    xTaskCreatePinnedToCore((TaskFunction_t )task1,                 /* task function */
                            (const char*    )"task1",               /* task name */
                            (uint16_t       )TASK1_STK_SIZE,        /* task stack size */
                            (void*          )NULL,                  /* task function parameters */
                            (UBaseType_t    )TASK1_PRIO,            /* task priority */
                            (TaskHandle_t*  )&Task1Task_Handler,    /* task handle */
                            (BaseType_t     ) 0);                   /* kernel runs the task */
}

/**
 * @brief       task1
 * @param       pvParameters : passed parameter(not used)
 * @retval      None
 */
void task1(void *pvParameters)
{
    pvParameters = pvParameters;
    /* Step 1: Initialize the list and list items */
    vListInitialise(&TestList);                 /* Initialize the list */
    vListInitialiseItem(&ListItem1);            /* Initialize list item1 */
    vListInitialiseItem(&ListItem2);            /* Initialize list item2 */
    vListInitialiseItem(&ListItem3);            /* Initialize list item3 */
    
    /* Step 2: Print the addresses of the list and other list items */
    printf("/***Step 2: Print the address of the list and list items***/\r\n");
    printf("project\t\t\address\r\n");
    printf("TestList\t\t0x%p\t\r\n", &TestList);
    printf("TestList->pxIndex\t0x%p\t\r\n", TestList.pxIndex);
    printf("TestList->xListEnd\t0x%p\t\r\n", (&TestList.xListEnd));
    printf("ListItem1\t\t0x%p\t\r\n", &ListItem1);
    printf("ListItem2\t\t0x%p\t\r\n", &ListItem2);
    printf("ListItem3\t\t0x%p\t\r\n", &ListItem3);
    printf("/**************************end*****************************/\r\n");
    printf("Press KEY0 to continue!\r\n\r\n\r\n");

    while (xl9555_key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /* Step 3: List Items 1 Insert the list */
    printf("/*********Step 3: List Items 1 Insert the list************/\r\n");
    vListInsert((List_t*    )&TestList,         /* list */
                (ListItem_t*)&ListItem1);       /* list item */
    printf("project\t\t\t\address\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("/**************************end***************************/\r\n");
    printf("Press KEY0 to continue!\r\n\r\n\r\n");

    while (xl9555_key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /* Step 4: List item 2 Insert list */
    printf("/*******Step 4: List item 2 Insert list*****************/\r\n");
    vListInsert((List_t*    )&TestList,         /* 列表 */
                (ListItem_t*)&ListItem2);       /* 列表项 */
    printf("project\t\t\t\address\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("/**************************end***************************/\r\n");
    printf("Press KEY0 to continue!\r\n\r\n\r\n");

    while (xl9555_key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /* Step 5: List items 3 Insert the list */
    printf("/**********Step 5: List items 3 Insert the list***********/\r\n");
    vListInsert((List_t*    )&TestList,         /* list */
                (ListItem_t*)&ListItem3);       /* list item */
    printf("project\t\t\t\address\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/**************************end***************************/\r\n");
    printf("Press KEY0 to continue!\r\n\r\n\r\n");

    while (xl9555_key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /* Step 6: Remove list item 2 */
    printf("/*******************Step 6: Remove list item 2********************/\r\n");
    uxListRemove((ListItem_t*   )&ListItem2);   /* remove list item */
    printf("project\t\t\t\address\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/**************************end***************************/\r\n");
    printf("Press KEY0 to continue!\r\n\r\n\r\n");

    while (xl9555_key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /* Step 7: Add list item 2 to the end of the list */
    printf("/***Step 7: Add list item 2 to the end of the list**********/\r\n");
    vListInsertEnd((List_t*     )&TestList,     /* list */
                   (ListItem_t* )&ListItem2);   /* list item */
    printf("project\t\t\t\address\r\n");
    printf("TestList->pxIndex\t\t0x%p\r\n", TestList.pxIndex);
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/************************experiment end***************************/\r\n");
    
    while(1)
    {
        LED_TOGGLE();
        vTaskDelay(1000);
    }
}

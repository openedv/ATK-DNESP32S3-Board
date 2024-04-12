/**
 ****************************************************************************************************
 * @file        manage.h
 * @author      ALIENTEK
 * @brief       MANAGE code
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

#ifndef __MANAGE_H
#define __MANAGE_H

#include "list.h"
#include <stdint.h>
#include <stddef.h>
#include "esp_timer.h"


/* test status */
enum STATE
{
    TEST_OK,
    TEST_FAIL
};

typedef struct Test
{
    atk_list_node_t test_list_node;     /* node */
    char *name_test;                    /* experiment name */
    uint8_t label;                      /* assign a label */
    int (*Function)(void * widget);     /* test function */
}Test_Typedef;

extern uint16_t test_status;

/* function declaration */
Test_Typedef *test_create(char name[],int (*pfunc)(Test_Typedef * obj));
void test_handler(void);

#endif

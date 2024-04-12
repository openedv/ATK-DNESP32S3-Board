/**
 ****************************************************************************************************
 * @file        manage.c
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

#include "manage.h"


atk_list_node_t test_list_head = ATK_LIST_INIT(test_list_head);
uint16_t test_status = 0x00;  /* record test status */

/**
 * @brief       Create test items
 * @param       name  : experiment name
 * @param       pfunc : test function
 * @retval      Return handle
 */
Test_Typedef *test_create(char name[],int (*pfunc)(Test_Typedef * obj))
{
    static int i = 0;
    Test_Typedef * obj = NULL;

    obj = malloc(sizeof(Test_Typedef));                     /* apply memory for each testing experiment */
    obj->label = i ++;                                      /* assign a label */
    obj->Function = pfunc;                                  /* test function */
    obj->name_test = name;                                  /* experiment name */
    atk_list_add_tail(&test_list_head,&obj->test_list_node);/* insert list */
    return obj;                                             /* return handle */
}

/**

 * @brief       Execution list entry
 * @param       None
 * @retval      None
 */
void test_handler(void)
{
    int status = 0;
    Test_Typedef *data;
    Test_Typedef *data_temp;
    data = (Test_Typedef *)malloc(sizeof(Test_Typedef));

    /* traverse List */
    atk_list_for_each_entry_safe(data, data_temp, &test_list_head, Test_Typedef, test_list_node)
    {
        status = data->Function(data);                  /* execute test code */

        if (status == TEST_FAIL)
        {
            test_status |= (1 << data->label);          /* record test failure */
        }
    }

    printf("0x%x\r\n",test_status);
}
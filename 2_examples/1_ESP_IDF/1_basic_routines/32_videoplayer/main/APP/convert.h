/**
 ****************************************************************************************************
 * @file        convert.h
 * @author      ALIENTEK
 * @brief       CONVERT code
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

#ifndef CONVERY_H_
#define CONVERY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ARRAY_SIZE(a)       ((sizeof(a))/(sizeof((a)[0])))

/* function declaration */
int convet_gbk_to_utf8(char **ptr, void *pin_buf, int in_len);
int convet_utf8_to_gbk(char **ptr, void *pin_buf, int in_len);
void convet_test(void);

#endif

/**
 ****************************************************************************************************
 * @file        key.cpp
 * @author      ALIENTEK
 * @brief       KEY code
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

#include "key.h"

/**
 * @brief       configure key pin
 * @param       None
 * @retval      None
 */
void key_init(void) 
{
    pinMode(KEY_PIN, INPUT_PULLUP);    /* set KEY_PIN as INPUT_PULLUP */
}



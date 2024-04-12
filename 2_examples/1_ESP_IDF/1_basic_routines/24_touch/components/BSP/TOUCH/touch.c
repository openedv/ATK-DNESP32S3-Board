/**
 ****************************************************************************************************
 * @file        touch.c
 * @author      ALIENTEK
 * @brief       TOUCH code
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

#include "touch.h"


_m_tp_dev tp_dev =
{{
    tp_init,
    0,
    0,
    0,
    0,
    0,
}};

/**
 * @brief       touch initialization
 * @param       None
 * @retval      0:Not calibrated
 *              1:calibrated
 */
uint8_t tp_init(void)
{
    tp_dev.touchtype = 0;                                   /* default setting*/
    tp_dev.touchtype |= ltdcdev.dir & 0X01;                 /* determine whether it is landscape or vertical based on LCD */

    if (ltdcdev.id == 0X4342 || ltdcdev.id == 0X4384)
    {
        gt9xxx_init();
        tp_dev.scan = gt9xxx_scan;                          /* scan function points to GT9147 touch screen scan */
        tp_dev.touchtype |= 0X80;
        return 0;
    }

    return 1;
}

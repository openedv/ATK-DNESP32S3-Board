/**
 ****************************************************************************************************
 * @file        imu.h
 * @author      ALIENTEK
 * @brief       IMU code
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

#ifndef __IMU_H
#define __IMU_H

#include "Arduino.h"

/* function declaration */
void acc_get_angle(float accl_in[3], float angle[2]);  

#endif

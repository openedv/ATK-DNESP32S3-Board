/**
 ****************************************************************************************************
 * @file        imu.cpp
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

#include "imu.h"
#include <math.h>


#define M_PI		  	(3.14159265358979323846f)
#define RAD_TO_DEG	(180.0f / M_PI) 	


/**
 * @brief       get euler Angle 
 * @param       accl_in : 3 axis acceleration data
 * @param       angle   : pitch Angle, roll Angle
 * @retval      None
 */
void acc_get_angle(float accl_in[3], float angle[2])
{   
    float accl_data[3];
    float acc_normal, pitch, roll;

    acc_normal = sqrtf(accl_in[0] * accl_in[0] + accl_in[1] * accl_in[1] + accl_in[2] * accl_in[2]);
    
    accl_data[0] = accl_in[0] / acc_normal;
    accl_data[1] = accl_in[1] / acc_normal;
    accl_data[2] = accl_in[2] / acc_normal;

    pitch = -atan2f(accl_in[0], accl_in[2]);
    angle[0] = pitch * RAD_TO_DEG;  

    acc_normal = sqrtf(accl_data[0] * accl_data[0] + accl_data[1] * accl_data[1] + accl_data[2] * accl_data[2]);
    roll = asinf((accl_data[1] / acc_normal));
    angle[1] = roll * RAD_TO_DEG; 
    //Serial.printf("g_pitch:%lf g_roll:%lf \r\n", angle[0], angle[1]);
}
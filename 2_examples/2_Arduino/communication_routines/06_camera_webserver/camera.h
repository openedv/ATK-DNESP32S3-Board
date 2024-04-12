/**
 ****************************************************************************************************
 * @file        camera.h
 * @author      ALIENTEK
 * @brief       CAMERA code
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

#ifndef __CAMERA_H
#define __CAMERA_H

#include "Arduino.h"

/* pin definition */
#define OV_SCL_PIN       38 
#define OV_SDA_PIN       39 
#define OV_D0_PIN        4 
#define OV_D1_PIN        5 
#define OV_D2_PIN        6 
#define OV_D3_PIN        7 
#define OV_D4_PIN        15 
#define OV_D5_PIN        16 
#define OV_D6_PIN        17 
#define OV_D7_PIN        18 
#define OV_VSYNC_PIN     47   
#define OV_HREF_PIN      48 
#define OV_PCLK_PIN      45    
#define OV_XCLK_PIN      -1
#define OV_RESET_PIN     -1
#define OV_PWDN_PIN      -1

/* This is already defined in the xl9555.h file
#define OV_RESET     
#define OV_PWDN        
*/

/* function declaration */
uint8_t camera_init(void);           
uint8_t camera_capture_show(void);    

#endif
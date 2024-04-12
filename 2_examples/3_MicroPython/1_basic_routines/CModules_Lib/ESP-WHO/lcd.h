/**
 ****************************************************************************************************
 * @file        lcd.h
 * @author      ALIENTEK
 * @brief       LCD code
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
 
#ifndef __LCD_H__
#define __LCD_H__

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "esp_log.h"
#include "esp_camera.h"
#include "esp_config.h"


#ifdef __cplusplus
extern "C" {
#endif

/* stroke color */
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

/* Scanning direction definition */
#define L2R_U2D         0           /* From left to right, from top to bottom */
#define L2R_D2U         1           /* From left to right, from bottom to top */
#define R2L_U2D         2           /* From right to left, from top to bottom */
#define R2L_D2U         3           /* From right to left, from bottom to top */

#define U2D_L2R         4           /* From top to bottom, from left to right */
#define U2D_R2L         5           /* From top to bottom, from right to left */
#define D2U_L2R         6           /* From bottom to top, from left to right */
#define D2U_R2L         7           /* From bottom to top, from right to left */

#define DFT_SCAN_DIR    L2R_U2D     /* Default scanning direction */

#define LCD_320X240     0
#define LCD_240X240     1

#ifdef  __cplusplus
}
#endif /*  __cplusplus */

#endif  /*  __LCD_H__ */

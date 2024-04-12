/**
 ****************************************************************************************************
 * @file        xl9555.h
 * @author      ALIENTEK
 * @brief       XL9555 code
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
 
#ifndef __XL9555_H__
#define __XL9555_H__

#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/mphal.h"
#include "myiic.h"
#include "esp_log.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"


/* XL9555 register */
#define XL9555_INPUT_PORT0_REG          0   /* Input port 0 register */
#define XL9555_INPUT_PORT1_REG          1   /* Input port 1 register */
#define XL9555_OUTPUT_PORT0_REG         2   /* Output port 0 register */
#define XL9555_OUTPUT_PORT1_REG         3   /* Output port 1 register */
#define XL9555_INVERSION_PORT0_REG      4   /* Polarity Inversion port 0 register */
#define XL9555_INVERSION_PORT1_REG      5   /* Polarity Inversion port 1 register */
#define XL9555_CONFIG_PORT0_REG         6   /* Configuration port 0 register */
#define XL9555_CONFIG_PORT1_REG         7   /* Configuration port 1 register */


#define XL9555_ADDR                     0X20    /* xl9555 device address(7bit) */

/* xl9555 pin function */
#define AP_INT_IO                       0x0001
#define QMA_INT_IO                      0x0002
#define SPK_EN_IO                       0x0004
#define BEEP_IO                         0x0008
#define OV_PWDN_IO                      0x0010
#define OV_RESET_IO                     0x0020
#define GBC_LED_IO                      0x0040
#define GBC_KEY_IO                      0x0080
#define LCD_BL_IO                       0x0100
#define CT_RST_IO                       0x0200
#define SLCD_RST_IO                     0x0400
#define SLCD_PWR_IO                     0x0800
#define KEY3_IO                         0x1000
#define KEY2_IO                         0x2000
#define KEY1_IO                         0x4000
#define KEY0_IO                         0x8000

#define KEY0_PRES                       1       /* KEY0 */
#define KEY1_PRES                       2       /* KEY1 */
#define KEY2_PRES                       3       /* KEY1 */
#define KEY3_PRES                       4       /* KEY1 */

#endif  /*  __XL9555_H__ */

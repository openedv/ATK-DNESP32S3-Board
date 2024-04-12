/**
 ****************************************************************************************************
 * @file        ap3216c.h
 * @author      ALIENTEK
 * @brief       AP3216C code
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
 
#ifndef __AP3216C_H__
#define __AP3216C_H__

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


/* ap3216c device address(7bits) */
#define AP3216C_ADDR                0X1E

#endif  /*  __AP3216C_H__ */

/**
 ****************************************************************************************************
 * @file        tud_sd.h
 * @author      ALIENTEK
 * @brief       TUD_SD code
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

#ifndef __TUD_SD_H
#define __TUD_SD_H

#include <inttypes.h>
#include "ff.h"
#include "diskio.h"
#include "esp_vfs_fat.h"
#include "tinyusb.h"
#include "esp_idf_version.h"


typedef struct
{
    uint8_t status;
}__usbdev;

extern __usbdev g_usbdev;

/* function declaration */
void tud_usb_sd(void);

#endif

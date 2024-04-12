/**
 ****************************************************************************************************
 * @file        tud_usart.h
 * @author      ALIENTEK
 * @brief       TUD_USART code
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

#ifndef __TUD_USART_H
#define __TUD_USART_H

#include <inttypes.h>
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "sdkconfig.h"
#include "esp_log.h"


/* function declaration */
void tud_usb_usart(void);

#endif

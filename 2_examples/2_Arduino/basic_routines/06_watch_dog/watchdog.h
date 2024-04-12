/**
 ****************************************************************************************************
 * @file        watchdog.h
 * @author      ALIENTEK
 * @brief       WATCHDOG code
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

#ifndef __WATCHDOG_H
#define __WATCHDOG_H

#include "Arduino.h"
#include "esp_system.h"

extern hw_timer_t *wdg_timer;

/* watchdog timer interrupt definition */
#define WDG_TIMx       1                    
#define WDG_ISR        tim1_ISR            

/* function declaration */
void wdg_init(uint32_t arr, uint16_t psc);  
void WDG_ISR(void);                        

#endif

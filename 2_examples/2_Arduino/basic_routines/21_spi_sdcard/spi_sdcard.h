/**
 ****************************************************************************************************
 * @file        spi_sdcard.h
 * @author      ALIENTEK
 * @brief       SPI_SDCARD code
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

#ifndef __SPI_SDCARD_H
#define __SPI_SDCARD_H

#include "Arduino.h"


/* pin definition */
#define SD_CS_PIN         2   
#define SD_MISO_PIN       13 
#define SD_MOSI_PIN       11
#define SD_SCK_PIN        12

/* function declaration */
uint8_t sdcard_init(void);    
void sd_test(void);          

#endif
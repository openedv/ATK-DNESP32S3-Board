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

#include <unistd.h>
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "spi.h"


/* pin definition */
#define SD_NUM_CS       GPIO_NUM_2
#define MOUNT_POINT     "/0:"
extern sdmmc_card_t *card;

/* function declaration */
esp_err_t sd_spi_init(void);
void sd_get_fatfs_usage(size_t *out_total_bytes, size_t *out_free_bytes);
#endif

/**
 ****************************************************************************************************
 * @file        spi.h
 * @author      ALIENTEK
 * @brief       SPI code
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

#ifndef __SPI_H
#define __SPI_H

#include <string.h>
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"


/* pin definition */
#define SPI_MOSI_GPIO_PIN   GPIO_NUM_11         /* SPI2_MOSI */
#define SPI_CLK_GPIO_PIN    GPIO_NUM_12         /* SPI2_CLK */
#define SPI_MISO_GPIO_PIN   GPIO_NUM_13         /* SPI2_MISO */

/* function declaration */
void spi2_init(void);
void spi2_write_cmd(spi_device_handle_t handle, uint8_t cmd);
void spi2_write_data(spi_device_handle_t handle, const uint8_t *data, int len);
uint8_t spi2_transfer_byte(spi_device_handle_t handle, uint8_t byte);

#endif

/**
 ****************************************************************************************************
 * @file        I2S.h
 * @author      ALIENTEK
 * @brief       I2S code
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

#ifndef _I2S_H
#define _I2S_H

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/i2s.h"
#include "esp_err.h"
#include "esp_log.h"
#include "es8388.h"
#include "driver/i2s_std.h"
#include "driver/i2s_pdm.h"


#define I2S_NUM                 (I2S_NUM_0)                 /* I2S port */
#define I2S_BCK_IO              (GPIO_NUM_46)               /* ES8388_SCLK */
#define I2S_WS_IO               (GPIO_NUM_9)                /* ES8388_LRCK */
#define I2S_DO_IO               (GPIO_NUM_10)               /* ES8388_SDOUT */
#define I2S_DI_IO               (GPIO_NUM_14)               /* ES8388_SDIN */
#define IS2_MCLK_IO             (GPIO_NUM_3)                /* ES8388_MCLK */
#define SAMPLE_RATE             (44100)                     /* sampling rate */

/* function declaration */
esp_err_t i2s_init(void);
void i2s_trx_start(void);
void i2s_trx_stop(void);
void i2s_deinit(void);
void i2s_set_samplerate_bits_sample(int samplerate,int bits_sample);
size_t i2s_tx_write(uint8_t *buffer, uint32_t frame_size);
size_t i2s_rx_read(uint8_t *buffer, uint32_t frame_size);

#endif

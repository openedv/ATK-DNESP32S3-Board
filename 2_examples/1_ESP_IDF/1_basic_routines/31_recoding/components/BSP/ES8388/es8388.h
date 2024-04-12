/**
 ****************************************************************************************************
 * @file        es8388.h
 * @author      ALIENTEK
 * @brief       ES8388 code
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

#ifndef __ES8388_H__
#define __ES8388_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/i2s.h"
#include "iic.h"
#include "math.h"


/* es8388 device address(7bits) */
#define ES8388_ADDR             0x20

/* function declaration */
uint8_t es8388_init(i2c_obj_t self);
esp_err_t es8388_deinit(void);
esp_err_t es8388_write_reg(uint8_t reg, uint8_t val);
esp_err_t es8388_read_reg(uint8_t reg_add, uint8_t *p_data);
void es8388_sai_cfg(uint8_t fmt, uint8_t len);
void es8388_hpvol_set(uint8_t volume);
void es8388_spkvol_set(uint8_t volume);
void es8388_3d_set(uint8_t depth);
void es8388_adda_cfg(uint8_t dacen, uint8_t adcen);
void es8388_output_cfg(uint8_t o1en, uint8_t o2en);
void es8388_mic_gain(uint8_t gain);
void es8388_alc_ctrl(uint8_t sel, uint8_t maxgain, uint8_t mingain);
void es8388_input_cfg(uint8_t in);


#endif

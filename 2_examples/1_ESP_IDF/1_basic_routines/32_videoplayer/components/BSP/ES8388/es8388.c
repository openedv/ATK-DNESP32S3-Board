/**
 ****************************************************************************************************
 * @file        es8388.c
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

#include "es8388.h"


i2c_obj_t es8388_i2c_master;

/**
 * @brief       write data to es8388 register
 * @param       reg_addr  : register address
 * @param       data      : write data
 * @retval      ESP_OK    : None
 */
esp_err_t es8388_write_reg(uint8_t reg_addr, uint8_t data)
{
    i2c_buf_t buf[2] = {
        {.len = 1, .buf = &reg_addr},
        {.len = 1, .buf = &data},
    };

    i2c_transfer(&es8388_i2c_master, ES8388_ADDR >> 1, 2, buf, I2C_FLAG_STOP);

    return ESP_OK;
}

/**
 * @brief       read bytes data from es8388
 * @param       reg_addr    : register address
 * @param       pdata       : buffer
 * @retval      ESP_OK : None
 */
esp_err_t es8388_read_reg(uint8_t reg_addr, uint8_t *pdata)
{
    i2c_buf_t buf[2] = {
        {.len = 1, .buf = &reg_addr},
        {.len = 1, .buf = pdata},
    };

    i2c_transfer(&es8388_i2c_master, ES8388_ADDR >> 1, 2, buf, I2C_FLAG_WRITE | I2C_FLAG_READ | I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief       Configure es8388
 * @param       None
 * @retval      0:succeed; !0:failed
 */
uint8_t es8388_init(i2c_obj_t self)
{
    esp_err_t ret_val = ESP_OK;

    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_0);
    }

    es8388_i2c_master = self;

    ret_val |= es8388_write_reg(0, 0x80);
    ret_val |= es8388_write_reg(0, 0x00);
    vTaskDelay(100);

    ret_val |= es8388_write_reg(0x01, 0x58);
    ret_val |= es8388_write_reg(0x01, 0x50);
    ret_val |= es8388_write_reg(0x02, 0xF3);
    ret_val |= es8388_write_reg(0x02, 0xF0);

    ret_val |= es8388_write_reg(0x03, 0x09);
    ret_val |= es8388_write_reg(0x00, 0x06);
    ret_val |= es8388_write_reg(0x04, 0x00);
    ret_val |= es8388_write_reg(0x08, 0x00);
    ret_val |= es8388_write_reg(0x2B, 0x80);

    ret_val |= es8388_write_reg(0x09, 0x88);
    ret_val |= es8388_write_reg(0x0C, 0x4C);
    ret_val |= es8388_write_reg(0x0D, 0x02);
    ret_val |= es8388_write_reg(0x10, 0x00);
    ret_val |= es8388_write_reg(0x11, 0x00);

    ret_val |= es8388_write_reg(0x17, 0x18);
    ret_val |= es8388_write_reg(0x18, 0x02);
    ret_val |= es8388_write_reg(0x1A, 0x00);
    ret_val |= es8388_write_reg(0x1B, 0x00);
    ret_val |= es8388_write_reg(0x27, 0xB8);
    ret_val |= es8388_write_reg(0x2A, 0xB8);
    vTaskDelay(100);

    if (ret_val != ESP_OK)
    {
        while(1)
        {
            printf("ES8388 initialization failed...\r\n");
            vTaskDelay(500);
        }
    }
    else
    {
        printf("ES8388 initialization successful...\r\n");
    }
    
    return 0;
}

/**
 * @brief       es8388 deinit
 * @param       None
 * @retval      0:succeed; !0:failed
 */
esp_err_t es8388_deinit(void)
{
    int res = 0;
    res = es8388_write_reg(0x02, 0xFF);
    return res;
}

/**
 * @brief       ES8388 working mode
 * @param       fmt : working mode
 *    @arg      0: Philips Standard I2S;
 *    @arg      1: MSB (Left aligned);
 *    @arg      2: LSB (Right aligned);
 *    @arg      3: PCM/DSP
 * @param       len : lenght
 *    @arg      0: 24bit
 *    @arg      1: 20bit
 *    @arg      2: 18bit
 *    @arg      3: 16bit
 *    @arg      4: 32bit
 * @retval      None
 */
void es8388_sai_cfg(uint8_t fmt, uint8_t len)
{
    fmt &= 0x03;
    len &= 0x07;
    es8388_write_reg(23, (fmt << 1) | (len << 3));
}

/**
 * @brief       Set headphone volume
 * @param       volume : headphone volume(0 ~ 33)
 * @retval      None
 */
void es8388_hpvol_set(uint8_t volume)
{
    if (volume > 33)
    {
        volume = 33;
    }

    es8388_write_reg(0x2E, volume);
    es8388_write_reg(0x2F, volume);
}

/**
 * @brief       Set speaker volume
 * @param       volume : speaker volume(0 ~ 33)
 * @retval      None
 */
void es8388_spkvol_set(uint8_t volume)
{
    if (volume > 33)
    {
        volume = 33;
    }

    es8388_write_reg(0x30, volume);
    es8388_write_reg(0x31, volume);
}

/**
 * @brief       Set up 3D surround sound
 * @param       depth : 0 ~ 7
 * @retval      None
 */
void es8388_3d_set(uint8_t depth)
{
    depth &= 0x7;
    es8388_write_reg(0x1D, depth << 2);
}

/**
 * @brief       ES8388 DAC/ADC config
 * @param       dacen : dac enable (1)/disable (0)
 * @param       adcen : adc enable (1)/disable (0)
 * @retval      None
 */
void es8388_adda_cfg(uint8_t dacen, uint8_t adcen)
{
    uint8_t tempreg = 0;

    tempreg |= !dacen << 0;
    tempreg |= !adcen << 1;
    tempreg |= !dacen << 2;
    tempreg |= !adcen << 3;
    es8388_write_reg(0x02, tempreg);
}

/**
 * @brief       ES8388 DAC output channel configuration
 * @param       o1en : Channel 1 enable (1)/disable (0)
 * @param       o2en : Channel 2 enable (1)/disable (0)
 * @retval      None
 */
void es8388_output_cfg(uint8_t o1en, uint8_t o2en)
{
    uint8_t tempreg = 0;
    tempreg |= o1en * (3 << 4);
    tempreg |= o2en * (3 << 2);
    es8388_write_reg(0x04, tempreg);
}

/**
 * @brief       ES8388 MIC gain setting (MIC PGA gain)
 * @param       gain : 0~8
 * @retval      None
 */
void es8388_mic_gain(uint8_t gain)
{
    gain &= 0x0F;
    gain |= gain << 4;
    es8388_write_reg(0x09, gain);
}

/**
 * @brief       ES8388 ALC settings
 * @param       sel
 *   @arg       0 : disable ALC
 *   @arg       1 : right channel ALC
 *   @arg       2 : left channel ALC
 *   @arg       3 : stereo ALC
 * @param       maxgain : 0~7
 * @param       minigain: 0~7
 * @retval      None
 */
void es8388_alc_ctrl(uint8_t sel, uint8_t maxgain, uint8_t mingain)
{
    uint8_t tempreg = 0;
    tempreg = sel << 6;
    tempreg |= (maxgain & 0x07) << 3;
    tempreg |= mingain & 0x07;
    es8388_write_reg(0x12, tempreg);
}

/**
 * @brief       ES8388 ADC output channel configuration
 * @param       in : Input channel
 *    @arg      0 : channel 1 input
 *    @arg      1 : channel 2 input
 * @retval      None
 */
void es8388_input_cfg(uint8_t in)
{
    es8388_write_reg(0x0A, (5 * in) << 4);
}

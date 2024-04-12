/**
 ****************************************************************************************************
 * @file        I2S.c
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

#include "i2s.h"


/* I2S config default */
#define I2S_CONFIG_DEFAULT() { \
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),      \
    .sample_rate = SAMPLE_RATE,                                             \
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,                           \
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           \
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,                      \
    .intr_alloc_flags = 0,                                                  \
    .dma_buf_count = 8,                                                     \
    .dma_buf_len = 256,                                                     \
    .use_apll = false                                                       \
}

/**
 * @brief       Configure I2S
 * @param       None
 * @retval      0:succeed; !0:failed
 */
esp_err_t i2s_init(void)
{
    esp_err_t ret_val = ESP_OK;

    /* Initialize I2C peripheral */
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DO_IO,
        .data_in_num = I2S_DI_IO,
        .mck_io_num = IS2_MCLK_IO,
    };
    
    i2s_config_t i2s_config = I2S_CONFIG_DEFAULT();
    i2s_config.sample_rate = SAMPLE_RATE;
    i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config.use_apll = true;
    ret_val |= i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    ret_val |= i2s_set_pin(I2S_NUM, &pin_config);
    ret_val |= i2s_zero_dma_buffer(I2S_NUM);
    return ret_val;
}

/**
 * @brief       Enable I2S
 * @param       None
 * @retval      None
 */
void i2s_trx_start(void)
{
    i2s_start(I2S_NUM);
}

/**
 * @brief       Stop I2S
 * @param       None
 * @retval      None
 */
void i2s_trx_stop(void)
{
    i2s_stop(I2S_NUM);
}

/**
 * @brief       I2S uninstall
 * @param       None
 * @retval      None
 */
void i2s_deinit(void)
{
    i2s_driver_uninstall(I2S_NUM);
}

/**
 * @brief       Set sampling rate
 * @param       sampleRate  : rate
 * @param       bits_sample : bit
 * @retval      None
 */
void i2s_set_samplerate_bits_sample(int samplerate,int bits_sample)
{
    i2s_set_clk(I2S_NUM,samplerate,bits_sample,I2S_CHANNEL_STEREO);
}

/**
 * @brief       I2S transmission data
 * @param       buffer      : Pointer to the source buffer
 * @param       frame_size  : lenght
 * @retval      None
 */
size_t i2s_tx_write(uint8_t *buffer, uint32_t frame_size)
{
    size_t bytes_written;
    i2s_write(I2S_NUM, buffer, frame_size, &bytes_written, 100);
    return bytes_written;
}

/**
 * @brief       I2S reading data
 * @param       buffer      : Pointer to the source buffer
 * @param       frame_size  : lenght
 * @retval      None
 */
size_t i2s_rx_read(uint8_t *buffer, uint32_t frame_size)
{
    size_t bytes_written;
    i2s_read(I2S_NUM, buffer, frame_size, &bytes_written, 1000);
    return bytes_written;
}

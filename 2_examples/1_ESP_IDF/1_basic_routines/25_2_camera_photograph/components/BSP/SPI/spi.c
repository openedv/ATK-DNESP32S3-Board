/**
 ****************************************************************************************************
 * @file        spi.c
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

#include "spi.h"


/**
 * @brief       Initialize the SPI2
 * @param       None
 * @retval      None
 */
void spi2_init(void)
{
    esp_err_t ret = 0;
    /* zero-initialize the config structure */
    spi_bus_config_t spi_bus_conf = {0};

    spi_bus_conf.miso_io_num = SPI_MISO_GPIO_PIN;   /* GPIO pin for Master In Slave Out (=spi_q) signal, or -1 if not used */
    spi_bus_conf.mosi_io_num = SPI_MOSI_GPIO_PIN;   /* GPIO pin for Master Out Slave In (=spi_d) signal, or -1 if not used */
    spi_bus_conf.sclk_io_num = SPI_CLK_GPIO_PIN;    /* GPIO pin for SPI Clock signal, or -1 if not used */
    spi_bus_conf.quadwp_io_num = -1;                /* GPIO pin for WP (Write Protect) signal, or -1 if not used */
    spi_bus_conf.quadhd_io_num = -1;                /* GPIO pin for HD (Hold) signal, or -1 if not used */
    spi_bus_conf.max_transfer_sz = 320 * 240 * 2;   /* Maximum transfer size, in bytes. Defaults to 4092 if 0 when DMA enabled, or to `SOC_SPI_MAXIMUM_BUFFER_SIZE` if DMA is disabled */
    
    /* Initialize a SPI bus */
    ret = spi_bus_initialize(SPI2_HOST, &spi_bus_conf, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
}

/**
 * @brief       Send a command to the LCD
 * @param       handle : SPI Handle
 * @param       cmd    : cmd
 * @retval      None
 */
void spi2_write_cmd(spi_device_handle_t handle, uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t = {0};

    t.length = 8;                                       /* Command is 8 bits */
    t.tx_buffer = &cmd;                                 /* The data is the cmd itself */
    ret = spi_device_polling_transmit(handle, &t);      /* Transmit! */
    ESP_ERROR_CHECK(ret);                               /* Should have had no issues */
}

/**
 * @brief       Send data to the LCD
 * @param       handle : SPI Handle
 * @param       data   : Data
 * @param       len    : length 
 * @retval      None
 */
void spi2_write_data(spi_device_handle_t handle, const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t = {0};

    if (len == 0)
    {
        return;                                     /* no need to send anything */
    }

    t.length = len * 8;                             /* Len is in bytes, transaction length is in bits */
    t.tx_buffer = data;                             /* Data */
    ret = spi_device_polling_transmit(handle, &t);  /* Transmit! */
    ESP_ERROR_CHECK(ret);                           /* Should have had no issues */
}

/**
 * @brief       read/write
 * @param       handle       : SPI Handle
 * @param       data         : Data 
 * @retval      t.rx_data[0] : rx buf
 */
uint8_t spi2_transfer_byte(spi_device_handle_t handle, uint8_t data)
{
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));

    t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    t.length = 8;
    t.tx_data[0] = data;
    spi_device_transmit(handle, &t);

    return t.rx_data[0];
}

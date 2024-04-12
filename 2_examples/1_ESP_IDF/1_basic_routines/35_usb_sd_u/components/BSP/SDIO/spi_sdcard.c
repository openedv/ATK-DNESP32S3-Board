/**
 ****************************************************************************************************
 * @file        spi_sdcard.c
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

#include "spi_sdcard.h"


spi_device_handle_t MY_SD_Handle = NULL;
sdmmc_card_t *card;
const char mount_point[] = MOUNT_POINT;
esp_err_t ret = ESP_OK;
esp_err_t mount_ret = ESP_OK;

/**
 * @brief       Initialize the SD
 * @param       None
 * @retval      0:succeed; !0:failed
 */
esp_err_t sd_spi_init(void)
{
    if (MY_SD_Handle != NULL)
    {
        spi_bus_remove_device(MY_SD_Handle);                        /* remove a device from the SPI bus */

        if (mount_ret == ESP_OK)
        {
            esp_vfs_fat_sdcard_unmount(mount_point, card);          /* all done, unmount partition and disable SPI peripheral */
        }
    }

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 20 * 1000 * 1000,                         /* clock out at 20 MHz */
        .mode = 0,                                                  /* SPI mode 0 */
        .spics_io_num = SD_NUM_CS,                                  /* CS pin */
        .queue_size = 7,                                            /* we want to be able to queue 7 transactions at a time */
    };

    /* allocate a device on a SPI bus */
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &MY_SD_Handle);

    /* options for mounting the filesystem.
       If format_if_mount_failed is set to true, SD card will be partitioned and
       formatted in case when mounting fails. */
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 4 * 1024 * sizeof(uint8_t)
    };

    /* when testing SD and SPI modes, keep in mind that once the card has been
       initialized in SPI mode, it can not be reinitialized in SD mode without
       toggling power to the card. */
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    /* this initializes the slot without card detect (CD) and write protect (WP) signals.
       Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals. */
    sdspi_device_config_t slot_config = {0};
    slot_config.host_id   = host.slot;
    slot_config.gpio_cs   = SD_NUM_CS;
    slot_config.gpio_cd   = GPIO_NUM_NC;
    slot_config.gpio_wp   = GPIO_NUM_NC;
    slot_config.gpio_int  = GPIO_NUM_NC;

    /* convenience function to get FAT filesystem on SD card registered in VFS */
    mount_ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
    ret |= mount_ret;

    return ret;
}

/**
 * @brief       Card has been initialized, print its properties
 * @param       out_total_bytes: Total SD card size
 * @param       out_free_bytes : SD card remaining size
 * @retval      None
 */
void sd_get_fatfs_usage(size_t *out_total_bytes, size_t *out_free_bytes)
{
    FATFS *fs;
    size_t free_clusters;
    int res = f_getfree("0:", (size_t *)&free_clusters, &fs);
    assert(res == FR_OK);
    size_t total_sectors = (fs->n_fatent - 2) * fs->csize;
    size_t free_sectors = free_clusters * fs->csize;

    size_t sd_total = total_sectors / 1024;
    size_t sd_total_KB = sd_total * fs->ssize;
    size_t sd_free = free_sectors / 1024;
    size_t sd_free_KB = sd_free*fs->ssize;

    if (out_total_bytes != NULL)
    {
        *out_total_bytes = sd_total_KB;
    }
    
    if (out_free_bytes != NULL)
    {
        *out_free_bytes = sd_free_KB;
    }
}

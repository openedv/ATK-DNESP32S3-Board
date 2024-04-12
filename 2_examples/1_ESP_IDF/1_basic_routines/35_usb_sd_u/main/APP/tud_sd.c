/**
 ****************************************************************************************************
 * @file        tud_sd.c
 * @author      ALIENTEK
 * @brief       TUD_SD code
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

#include "tud_sd.h"


static const char *TAG = "usb_msc";
const char *disk_path = "/disk";
static uint8_t s_pdrv = 0;
static int s_disk_block_size = 0;
#define LOGICAL_DISK_NUM        1
static bool ejected[LOGICAL_DISK_NUM] = {true};
__usbdev g_usbdev;

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

/**
 * @brief       Invoked when device is mounted
 * @param       None
 * @retval      None
 */
void tud_mount_cb(void)
{
    for (uint8_t i = 0; i < LOGICAL_DISK_NUM; i++)
    {
        ejected[i] = false;
    }

    g_usbdev.status |= 0x01;

    ESP_LOGI(__func__, "");
}

/**
 * @brief       Invoked when device is unmounted
 * @param       None
 * @retval      None
 */
void tud_umount_cb(void)
{
    ESP_LOGW(__func__, "");
}

/**
 * @brief       Invoked when usb bus is suspended
 * @param       None
 * @retval      None
 */
void tud_suspend_cb(bool remote_wakeup_en)
{
    g_usbdev.status &= 0x00;
    ESP_LOGW(__func__, "");
}

/**
 * @brief       Invoked when usb bus is resumed
 * @param       None
 * @retval      None
 */
void tud_resume_cb(void)
{
    ESP_LOGW(__func__, "");
}

/**
 * @brief       Invoke when Write10 command is complete, can be used to flush flash caching
 * @param       lun : lun number
 * @retval      None
 */
void tud_msc_write10_complete_cb(uint8_t lun)
{
    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }

    ESP_LOGD(__func__, "");
}

/**
 * @brief       logical disk ejected
 * @param       None
 * @retval      None
 */
static bool _logical_disk_ejected(void)
{
    bool all_ejected = true;

    for (uint8_t i = 0; i < LOGICAL_DISK_NUM; i++)
    {
        all_ejected &= ejected[i];
    }

    return all_ejected;
}

/**
 * @brief       Invoked when received SCSI_CMD_INQUIRY
 * @param       lun         : lun
 * @param       vendor_id   : vendor id
 * @param       product_id  : product id
 * @param       product_rev : product rev
 * @retval      None
 */
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }

    const char vid[] = "ESPS3-S3";
    const char pid[] = "Mass Storage";
    const char rev[] = "1.0";

    memcpy(vendor_id, vid, strlen(vid));
    memcpy(product_id, pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
}

/**
 * @brief       Invoked when received Test Unit Ready command
 * @param       lun         : lun
 * @retval      return true allowing host to read/write this LUN e.g SD card inserted
 */
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }

    if (_logical_disk_ejected())
    {
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00);
        return false;
    }

    return true;
}

/**
 * @brief       Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
 * @param       lun         : lun
 * @param       block_count : block count
 * @param       block_size  : block size
 * @retval      return true allowing host to read/write this LUN e.g SD card inserted
 */
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }

    disk_ioctl(s_pdrv, GET_SECTOR_COUNT, block_count);
    disk_ioctl(s_pdrv, GET_SECTOR_SIZE, block_size);
    s_disk_block_size = *block_size;
    ESP_LOGD(__func__, "GET_SECTOR_COUNT = %"PRIu32"，GET_SECTOR_SIZE = %d", *block_count, *block_size);
}

/**
 * @brief       Invoked to check if device is writable as part of SCSI WRITE10
 * @param       lun         : lun
 * @retval      True: Yes; False: No
 */
bool tud_msc_is_writable_cb(uint8_t lun)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }

    return true;
}

/**
 * @brief       Invoked when received Start Stop Unit command
 * @param       lun                 : lun
 * @param       power_condition     : power condition
 * @param       start or load_eject : Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
 *                                    Start = 1 : active mode, if load_eject = 1 : load disk storage
 * @retval      True: Successfully loaded disk storage; False: Successfully uninstalled disk storage
 */
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
    ESP_LOGI(__func__, "");
    (void) power_condition;

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }

    if (load_eject)
    {
        if (!start)
        {
            if (disk_ioctl(s_pdrv, CTRL_SYNC, NULL) != RES_OK)
            {
                return false;
            }
            else
            {
                ejected[lun] = true;
            }
        }
        else
        {
            return !ejected[lun];
        }
    }
    else
    {
        if (!start)
        {
            if (disk_ioctl(s_pdrv, CTRL_SYNC, NULL) != RES_OK)
            {
                return false;
            }
        }
    }

    return true;
}

/**
 * @brief       Invoked when received SCSI READ10 command
 * @param       lun     : lun number
 * @param       lba     : Block Address
 * @param       offset  : Data offset
 * @param       buffer  : Storage area for reading data
 * @param       bufsize : Read data size
 * @retval      Return the number of bytes read
 */
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }

    const uint32_t block_count = bufsize / s_disk_block_size;

    disk_read(s_pdrv, buffer, lba, block_count);

    return block_count * s_disk_block_size;
}

/**
 * @brief       Invoked when received SCSI WRITE10 command
 * @param       lun     : lun number
 * @param       lba     : Block Address
 * @param       offset  : Data offset
 * @param       buffer  : Storage area for reading data
 * @param       bufsize : Read data size
 * @retval      Return the number of bytes read
 */
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    ESP_LOGD(__func__, "");
    (void) offset;

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }

    const uint32_t block_count = bufsize / s_disk_block_size;

    disk_write(s_pdrv, buffer, lba, block_count);

    return block_count * s_disk_block_size;
}

/**
 * @brief       Callback invoked when received an SCSI command not in built-in list below
 * @param       lun     : lun number
 * @param       scsi_cmd:The content of the scsi command must be checked by the application in order to make a corresponding response
 * @param       buffer  : Storage area for reading data
 * @param       bufsize : Read data size
 * @retval      Return the number of bytes written
 */
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }

    void const *response = NULL;
    uint16_t resplen = 0;

    bool in_xfer = true;

    switch (scsi_cmd[0])
    {
        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
            resplen = 0;
            break;

        default:
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

            resplen = -1;
            break;
    }

    if (resplen > bufsize)
    {
        resplen = bufsize;
    }

    if (response && (resplen > 0))
    {
        if (in_xfer)
        {
            memcpy(buffer, response, resplen);
        }
        else
        {
            /* SCSI output */
        }
    }

    return resplen;
}

/**
 * @brief       USB initialization
 * @param       base_path : The name of the partition table
 * @retval      None
 */
void tud_usb_sd(void)
{
    const tinyusb_config_t tusb_cfg = {0};
    /* This is an all-in-one helper function, including */
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    ESP_LOGI(TAG, "USB MSC initialization DONE");
}

/**
 ****************************************************************************************************
 * @file        image.c
 * @author      ALIENTEK
 * @brief       IMAGE code
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

#include "image.h"
#include "lcd.h"
#include "ff.h"
#include <string.h>

/* IAMGE size */
#define IMAGESECSIZE         90

/* Starting address for storing font library */
#define IMAGEINFOADDR        0

/* Limit each operation to within 4K */
#define SECTOR_SIZE          0X1000

_image_info g_ftinfo;

static const char *TAG = "storage_partition";
const esp_partition_t *storage_partition;

/* image library path */
char *const IMAGE_GBK_PATH[10] =
{
    "/SYSTEM/LVGLBIN/lv_camera.BIN",
    "/SYSTEM/LVGLBIN/lv_file.BIN",
    "/SYSTEM/LVGLBIN/lv_video.BIN",
    "/SYSTEM/LVGLBIN/lv_setting.BIN",
    "/SYSTEM/LVGLBIN/lv_weather.BIN",
    "/SYSTEM/LVGLBIN/lv_measure.BIN",
    "/SYSTEM/LVGLBIN/lv_photo.BIN",
    "/SYSTEM/LVGLBIN/lv_music.BIN",
    "/SYSTEM/LVGLBIN/lv_calendar.BIN",
    "/SYSTEM/LVGLBIN/lv_background.BIN",
};

/* Prompt information */
char *const IMAGE_UPDATE_REMIND_TBL[10] =
{
    "Updating lv_camera.BIN",
    "Updating lv_file.BIN",
    "Updating lv_video.BIN",
    "Updating lv_setting.BIN",
    "Updating lv_weather.BIN",
    "Updating lv_measure.BIN",
    "Updating lv_photo.BIN",
    "Updating lv_music.BIN",
    "Updating lv_calendar.BIN",
    "Updating lv_background.BIN",
};

#define IMAGE_GBK_NUM           (int)(sizeof(IMAGE_GBK_PATH) / sizeof(IMAGE_GBK_PATH[0]))
#define IMAGE_UPDATE_REMIND_NUM (int)(sizeof(IMAGE_UPDATE_REMIND_TBL) / sizeof(IMAGE_UPDATE_REMIND_TBL[0]))

/**
 * @brief       Read data from the partition without any transformation/decryption
 * @param       buffer    : Pointer to the buffer where data should be stored
 * @param       offset    : Address of the data to be read, relative to the beginning of the partition
 * @param       length    : Size of data to be read, in bytes
 * @retval      ESP_OK, if data was read successfully;
 *              ESP_ERR_INVALID_ARG, if src_offset exceeds partition size;
 *              ESP_ERR_INVALID_SIZE, if read would go out of bounds of the partition;or one of error codes from lower-level flash driver.
 */
esp_err_t images_partition_read(void *buffer, uint32_t offset, uint32_t length)
{
    esp_err_t err;

    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "ESP_ERR_INVALID_ARG");
        return ESP_ERR_INVALID_ARG;
    }

    err = esp_partition_read(storage_partition, offset, buffer, length);
    
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash read failed.");
        return err;
    }
    
    return err;
}

/**
 * @brief       Write data to the partition without any transformation/encryption
 * @param       buffer    : Pointer to the source buffer.  Pointer must be non-NULL and buffer must be at least 'size' bytes long
 * @param       offset    : Address where the data should be written, relative to the beginning of the partition
 * @param       length    : Size of data to be written, in bytes
 * @retval      ESP_OK, if data was written successfully;
 *              ESP_ERR_INVALID_ARG, if dst_offset exceeds partition size;
 *              ESP_ERR_INVALID_SIZE, if write would go out of bounds of the partition;or one of the error codes from lower-level flash driver.
 */
esp_err_t images_partition_write(void *buffer, uint32_t offset, uint32_t length)
{
    esp_err_t err;

    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "ESP_ERR_INVALID_ARG");
        return ESP_ERR_INVALID_ARG;
    }

    err = esp_partition_write(storage_partition, offset, buffer, length);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash write failed.");
        return err;
    }

    return err;
}

/**
 * @brief       Erase part of the partition
 * @param       offset : Offset from the beginning of partition where erase operation should start. Must be aligned to partition->erase_size
 * @retval      ESP_OK, if the range was erased successfully;
 *              ESP_ERR_INVALID_ARG, if iterator or dst are NULL;
 *              ESP_ERR_INVALID_SIZE, if erase would go out of bounds of the partition;or one of error codes from lower-level flash driver.
 */
esp_err_t images_partition_erase_sector(uint32_t offset)
{
    esp_err_t err;

    err = esp_partition_erase_range(storage_partition, offset, SECTOR_SIZE);
    
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash erase failed.");
        return err;
    }

    return err;
}

/**
 * @brief       Update progress
 * @param       x       : The x-coordinate of the string to be displayed
 * @param       y       : The y-coordinate of the string to be displayed
 * @param       size    : image size
 * @param       totsize : File size
 * @param       pos     : Current file pointer position
 * @param       color   : image color
 * @retval      None
 */
static void images_progress_show(uint16_t x, uint16_t y, uint8_t size, uint32_t totsize, uint32_t pos, uint16_t color)
{
    float prog;
    uint8_t t = 0XFF;
    prog = (float)pos / totsize;
    prog *= 100;

    if (t != prog)
    {
        lcd_show_string(x + 3 * size / 2, y, 240, 320, size, "%", color);
        t = prog;

        if (t > 100) t = 100;

        lcd_show_num(x, y, t, 3, size, color);
    }
}

/**
 * @brief       Update image library
 * @param       x       : The x-coordinate of the string to be displayed
 * @param       y       : The y-coordinate of the string to be displayed
 * @param       size    : image size
 * @param       fpath   : image library path
 * @param       fx      : Updated content
 *   @arg                 0:ungbk
 *   @arg                 1:gbk12
 *   @arg                 2:gbk16
 *   @arg                 3:gbk24
 * @param       color   : image color
 * @retval      0:succeed; !0:failed
 */
static uint8_t images_update_imagex(uint16_t x, uint16_t y, uint8_t size, uint8_t *fpath, uint8_t fx, uint16_t color)
{
    uint32_t flashaddr = 0;
    FIL *fftemp;
    uint8_t *tempbuf;
    uint8_t res;
    uint16_t bread;
    uint32_t offx = 0;
    uint8_t rval = 0;

    fftemp = (FIL *)malloc(sizeof(FIL));

    if (fftemp == NULL)rval = 1;

    tempbuf = malloc(4096);

    if (tempbuf == NULL)rval = 1;

    res = f_open(fftemp, (const TCHAR *)fpath, FA_READ);

    if (res) rval = 2;   /* fail to open file */

    if (rval == 0)
    {
        switch (fx)
        {
            case 0:
                g_ftinfo.lvgl_camera_addr = IMAGEINFOADDR + sizeof(g_ftinfo);
                g_ftinfo.lvgl_camera_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_camera_addr;
                break;
            case 1:
                g_ftinfo.lvgl_file_addr = g_ftinfo.lvgl_camera_addr + g_ftinfo.lvgl_camera_size;
                g_ftinfo.lvgl_file_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_file_addr;
                break;
            case 2:
                g_ftinfo.lvgl_video_addr = g_ftinfo.lvgl_file_addr + g_ftinfo.lvgl_file_size;
                g_ftinfo.lvgl_video_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_video_addr;
                break;
            case 3:
                g_ftinfo.lvgl_setting_addr = g_ftinfo.lvgl_video_addr + g_ftinfo.lvgl_video_size;
                g_ftinfo.lvgl_setting_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_setting_addr;
                break;
            case 4:
                g_ftinfo.lvgl_weather_addr = g_ftinfo.lvgl_setting_addr + g_ftinfo.lvgl_setting_size;
                g_ftinfo.lvgl_weather_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_weather_addr;
                break;
            case 5:
                g_ftinfo.lvgl_measure_addr = g_ftinfo.lvgl_weather_addr + g_ftinfo.lvgl_weather_size;
                g_ftinfo.lvgl_measure_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_measure_addr;
                break;
            case 6:
                g_ftinfo.lvgl_photo_addr = g_ftinfo.lvgl_measure_addr + g_ftinfo.lvgl_measure_size;
                g_ftinfo.lvgl_photo_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_photo_addr;
                break;
            case 7:
                g_ftinfo.lvgl_music_addr = g_ftinfo.lvgl_photo_addr + g_ftinfo.lvgl_photo_size;
                g_ftinfo.lvgl_music_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_music_addr;
                break;
            case 8:
                g_ftinfo.lvgl_calendar_addr = g_ftinfo.lvgl_music_addr + g_ftinfo.lvgl_music_size;
                g_ftinfo.lvgl_calendar_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_calendar_addr;
                break;
            case 9:
                g_ftinfo.lvgl_background_addr = g_ftinfo.lvgl_calendar_addr + g_ftinfo.lvgl_calendar_size;
                g_ftinfo.lvgl_background_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_background_addr;
                break;
        }

        while (res == FR_OK)
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);

            if (res != FR_OK) break;

            images_partition_write(tempbuf, offx + flashaddr, bread);
            offx += bread;
            images_progress_show(x, y, size, fftemp->obj.objsize, offx, color);

            if (bread != 4096) break;
        }

        f_close(fftemp);
    }

    free(fftemp);
    free(tempbuf);
    return res;
}

/**
 * @brief       Update iamge file
 *   @note      Update iamge file(UNIGBK,GBK12,GBK16,GBK24)
 * @param       x       : The x-coordinate of the string to be displayed
 * @param       y       : The y-coordinate of the string to be displayed
 * @param       size    : iamge size
 * @param       src     : Disk
 *   @arg                 "0:":SD
 *   @arg                 "1:":FLASH
 * @param       color   : iamge color
 * @retval      0:succeed; !0:failed
 */
uint8_t images_update_image(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color)
{
    uint8_t *pname;
    uint32_t *buf;
    uint8_t res = 0;
    uint16_t i, j;
    FIL *fftemp;
    uint8_t rval = 0;
    res = 0XFF;
    g_ftinfo.imageok = 0XFF;

    pname = malloc(100);
    buf = malloc(4096);
    fftemp = (FIL *)malloc(sizeof(FIL));

    if (buf == NULL || pname == NULL || fftemp == NULL)
    {
        free(fftemp);
        free(pname);
        free(buf);
        return 5;
    }

    for (i = 0; i < IMAGE_GBK_NUM; i++)     /* first, check if the files UNIGBK, GBK12, GBK16, and GBK24 are normal */
    {
        strcpy((char *)pname, (char *)src);
        strcat((char *)pname, (char *)IMAGE_GBK_PATH[i]);
        res = f_open(fftemp, (const TCHAR *)pname, FA_READ);

        if (res)
        {
            rval |= 1 << 7;
            break;
        }
    }

    free(fftemp);

    if (rval == 0)
    {
        lcd_show_string(x, y, 240, 320, size, "Erasing sectors... ", color);

        for (i = 0; i < IMAGESECSIZE; i++)
        {
            images_progress_show(x + 20 * size / 2, y, size, IMAGESECSIZE, i, color);
            images_partition_read((uint8_t *)buf, ((IMAGEINFOADDR / 4096) + i) * 4096, 4096);

            for (j = 0; j < 1024; j++)
            {
                if (buf[j] != 0XFFFFFFFF) break;
            }

            if (j != 1024)
            {
                images_partition_erase_sector(((IMAGEINFOADDR / 4096) + i) * 4096);
            }
        }

        for (i = 0; i < IMAGE_UPDATE_REMIND_NUM; i++)
        {
            lcd_show_string(x, y, 240, 320, size, IMAGE_UPDATE_REMIND_TBL[i], color);
            strcpy((char *)pname, (char *)src);
            strcat((char *)pname, (char *)IMAGE_GBK_PATH[i]);
            res = images_update_imagex(x + 20 * size / 2, y, size, pname, i, color);

            if (res)
            {
                free(buf);
                free(pname);
                return 1 + i;
            }
        }

        g_ftinfo.imageok = 0xBB;
        images_partition_write((uint8_t *)&g_ftinfo, IMAGEINFOADDR, sizeof(g_ftinfo));
    }

    free(pname);
    free(buf);

    return rval;  
}

/**
 * @brief       configure image
 * @param       None
 * @retval      0:succeed; !0:failed
 */
uint8_t images_init(void)
{
    uint8_t t = 0;

    storage_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "storage");
    
    /* flash partition not found */
    if (storage_partition == NULL)
    {
        ESP_LOGE(TAG, "Flash partition not found.");
        return 1;
    }

    while (t < 10)
    {
        t++;
        images_partition_read((uint8_t *)&g_ftinfo, IMAGEINFOADDR, sizeof(g_ftinfo));

        if (g_ftinfo.imageok == 0xBB)
        {
            break;
        }
        
        vTaskDelay(20);
    }

    if (g_ftinfo.imageok != 0xBB)
    {
        return 1;
    }
    
    return 0;
}

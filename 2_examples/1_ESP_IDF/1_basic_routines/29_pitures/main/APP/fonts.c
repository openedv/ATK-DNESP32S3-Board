/**
 ****************************************************************************************************
 * @file        font.c
 * @author      ALIENTEK
 * @brief       FONT code
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

#include "fonts.h"


/* Font size */
#define FONTSECSIZE         791
/* Limit each operation to within 4K */
#define SECTOR_SIZE         0X1000

/* Starting address for storing font library */
#define FONTINFOADDR        0


_font_info ftinfo;

static const char *TAG = "storage_partition";
const esp_partition_t *storage_partition;

/* Font library path */
char *const FONT_GBK_PATH[4] =
{
    "/SYSTEM/FONT/UNIGBK.BIN",      /* UNIGBK.BIN path*/
    "/SYSTEM/FONT/GBK12.FON",       /* GBK12 path */
    "/SYSTEM/FONT/GBK16.FON",       /* GBK16 path */
    "/SYSTEM/FONT/GBK24.FON",       /* GBK24 path */
};

/* Prompt information */
char *const FONT_UPDATE_REMIND_TBL[4] =
{
    "Updating UNIGBK.BIN",
    "Updating GBK12.FON ",
    "Updating GBK16.FON ",
    "Updating GBK24.FON ",
};

/**
 * @brief       Read data from the partition without any transformation/decryption
 * @param       buffer    : Pointer to the buffer where data should be stored
 * @param       offset    : Address of the data to be read, relative to the beginning of the partition
 * @param       length    : Size of data to be read, in bytes
 * @retval      ESP_OK, if data was read successfully;
 *              ESP_ERR_INVALID_ARG, if src_offset exceeds partition size;
 *              ESP_ERR_INVALID_SIZE, if read would go out of bounds of the partition;or one of error codes from lower-level flash driver.
 */
esp_err_t fonts_partition_read(void * buffer, uint32_t offset, uint32_t length)
{
    esp_err_t err;

    if (buffer == NULL || (length > SECTOR_SIZE))
    {
        ESP_LOGE(TAG, "ESP_ERR_INVALID_ARG");
        return ESP_ERR_INVALID_ARG;
    }

    err = esp_partition_read(storage_partition, offset, buffer,length);
    
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
esp_err_t fonts_partition_write(void * buffer, uint32_t offset, uint32_t length)
{
    esp_err_t err;

    if (buffer == NULL || (length > SECTOR_SIZE))
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
esp_err_t fonts_partition_erase_sector(uint32_t offset)
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
 * @param       size    : Font size
 * @param       totsize : File size
 * @param       pos     : Current file pointer position
 * @param       color   : Font color
 * @retval      None
 */
void fonts_progress_show(uint16_t x, uint16_t y, uint8_t size, uint32_t totsize, uint32_t pos, uint16_t color)
{
    float prog;
    uint8_t t = 0XFF;
    prog = (float)pos / totsize;
    prog *= 100;

    if (t != prog)
    {
        lcd_show_string(x + 3 * size / 2, y, 240, 320, size, "%", color);
        t = prog;

        if (t > 100)t = 100;

        lcd_show_num(x, y, t, 3, size, color);
    }
}

/**
 * @brief       Update font library
 * @param       x       : The x-coordinate of the string to be displayed
 * @param       y       : The y-coordinate of the string to be displayed
 * @param       size    : Font size
 * @param       fpath   : Font library path
 * @param       fx      : Updated content
 *   @arg                 0:ungbk
 *   @arg                 1:gbk12
 *   @arg                 2:gbk16
 *   @arg                 3:gbk24
 * @param       color   : Font color
 * @retval      0:succeed; !0:failed
 */
static uint8_t fonts_update_fontx(uint16_t x, uint16_t y, uint8_t size, uint8_t *fpath, uint8_t fx, uint16_t color)
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

    if (res)rval = 2;   /* fail to open file */

    if (rval == 0)
    {
        switch (fx)
        {
            case 0: /* update UNIGBK.BIN */
                ftinfo.ugbkaddr = FONTINFOADDR + sizeof(ftinfo);
                ftinfo.ugbksize = fftemp->obj.objsize;
                flashaddr = ftinfo.ugbkaddr;
                break;

            case 1: /* update GBK12.BIN */
                ftinfo.f12addr = ftinfo.ugbkaddr + ftinfo.ugbksize;
                ftinfo.gbk12size = fftemp->obj.objsize;
                flashaddr = ftinfo.f12addr;
                break;

            case 2: /* update GBK16.BIN */
                ftinfo.f16addr = ftinfo.f12addr + ftinfo.gbk12size;
                ftinfo.gbk16size = fftemp->obj.objsize;
                flashaddr = ftinfo.f16addr;
                break;

            case 3: /* update GBK24.BIN */
                ftinfo.f24addr = ftinfo.f16addr + ftinfo.gbk16size;
                ftinfo.gbk24size = fftemp->obj.objsize;
                flashaddr = ftinfo.f24addr;
                break;
        }

        while (res == FR_OK)
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);    /* read data */

            if (res != FR_OK)break;

            fonts_partition_write(tempbuf, offx + flashaddr, bread);
            offx += bread;
            fonts_progress_show(x, y, size, fftemp->obj.objsize, offx, color);

            if (bread != 4096) break;
        }

        f_close(fftemp);
    }

    free(fftemp);
    free(tempbuf);
    return res;
}

/**
 * @brief       Update font file
 *   @note      Update font file(UNIGBK,GBK12,GBK16,GBK24)
 * @param       x       : The x-coordinate of the string to be displayed
 * @param       y       : The y-coordinate of the string to be displayed
 * @param       size    : Font size
 * @param       src     : Disk
 *   @arg                 "0:":SD
 *   @arg                 "1:":FLASH
 * @param       color   : Font color
 * @retval      0:succeed; !0:failed
 */
uint8_t fonts_update_font(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color)
{
    uint8_t *pname;
    uint32_t *buf;
    uint8_t res = 0;
    uint16_t i, j;
    FIL *fftemp;
    uint8_t rval = 0;
    res = 0XFF;
    ftinfo.fontok = 0XFF;
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

    for (i = 0; i < 4; i++) /* first, check if the files UNIGBK, GBK12, GBK16, and GBK24 are normal */
    {
        strcpy((char *)pname, (char *)src);
        strcat((char *)pname, (char *)FONT_GBK_PATH[i]);
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
        lcd_show_string(x, y, 240, 320, size, "Erasing sectors... ", color);    /* erasing sectors */

        for (i = 0; i < FONTSECSIZE; i++)
        {
            fonts_progress_show(x + 20 * size / 2, y, size, FONTSECSIZE, i, color);
            fonts_partition_read((uint8_t *)buf, ((FONTINFOADDR / 4096) + i) * 4096, 4096);

            for (j = 0; j < 1024; j++)
            {
                if (buf[j] != 0XFFFFFFFF)break;
            }

            if (j != 1024)
            {
                fonts_partition_erase_sector(((FONTINFOADDR / 4096) + i) * 4096);
            }
        }

        for (i = 0; i < 4; i++) /* update sequentially UNIGBK,GBK12,GBK16,GBK24 */
        {
            lcd_show_string(x, y, 240, 320, size, FONT_UPDATE_REMIND_TBL[i], color);
            strcpy((char *)pname, (char *)src);
            strcat((char *)pname, (char *)FONT_GBK_PATH[i]);
            res = fonts_update_fontx(x + 20 * size / 2, y, size, pname, i, color);

            if (res)
            {
                free(buf);
                free(pname);
                return 1 + i;
            }
        }

        ftinfo.fontok = 0XAA;
        fonts_partition_write((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo));
    }

    free(pname);
    free(buf);
    return rval;
}

/**
 * @brief       configure font
 * @param       None
 * @retval      0:succeed; !0:failed
 */
uint8_t fonts_init(void)
{
    uint8_t t = 0;

    storage_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY,"storage");
    
    /* flash partition not found */
    if (storage_partition == NULL)
    {
        ESP_LOGE(TAG, "Flash partition not found.");
        return 1;
    }
    
    while (t < 10)
    {
        t++;
        fonts_partition_read((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo));

        if (ftinfo.fontok == 0XAA)
        {
            break;
        }
        
        vTaskDelay(20);
    }

    if (ftinfo.fontok != 0XAA)
    {
        return 1;
    }
    
    return 0;
}

/**
 ****************************************************************************************************
 * @file        bmp.c
 * @author      ALIENTEK
 * @brief       BMP code
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

#include "bmp.h"


/**
 * @brief       BMP image decoding
 * @param       filename      : Image file path(.bmp/.jpg/.jpeg/.gif/.png etc)
 * @param       width         : Display width
 * @param       height        : Display height
 * @retval      Return JPEG decoding speed
 */
TickType_t bmp_decode(const char *filename, int width, int height)
{
    TickType_t startTick, endTick, diffTick;
    startTick = xTaskGetTickCount();

    /* open requested file */
    esp_err_t ret;
    FIL* fp;
    uint16_t br = 0;
    fp = (FIL *)malloc(sizeof(FIL));

    ret = f_open(fp, (const TCHAR *)filename, FA_READ);

    if (fp == NULL)
    {
        ESP_LOGW(__FUNCTION__, "File not found [%s]", filename);
        return 0;
    }

    /* read bmp header */
    bmpfile_t *result = (bmpfile_t*)malloc(sizeof(bmpfile_t));
    ret |= f_read(fp,result->header.magic, 2, (UINT *)&br);

    if (result->header.magic[0]!='B' || result->header.magic[1] != 'M')
    {
        ESP_LOGW(__FUNCTION__, "File is not BMP");
        free(result);
        f_close(fp);
        return 0;
    }

    /* read dib header */
    ret |= f_read(fp,&result->header.filesz, 4, (UINT *)&br);
    ret |= f_read(fp,&result->header.creator1, 2, (UINT *)&br);
    ret |= f_read(fp,&result->header.creator2, 2, (UINT *)&br);
    ret |= f_read(fp,&result->header.offset, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.header_sz, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.width, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.height, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.nplanes, 2, (UINT *)&br);
    ret |= f_read(fp,&result->dib.depth, 2, (UINT *)&br);
    ret |= f_read(fp,&result->dib.compress_type, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.bmp_bytesz, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.hres, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.vres, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.ncolors, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.nimpcolors, 4, (UINT *)&br);

    if ((result->dib.depth == 1) && (result->dib.compress_type == 0))
    {
        /* Unrealized */
    }
    else if((result->dib.depth == 24) && (result->dib.compress_type == 0))
    {
        /* BMP rows are padded (if needed) to 4-byte boundary */
        uint32_t rowSize = (result->dib.width * 3 + 3) & ~3;
        int w = result->dib.width;
        int h = result->dib.height;
        int _x;
        int _w;
        int _cols;
        int _cole;
        int _y;
        int _rows;
        int _rowe;

        if (width >= w)
        {
            _x = (width - w) / 2;
            _w = w;
            _cols = 0;
            _cole = w - 1;
        }
        else
        {
            _x = 0;
            _w = width;
            _cols = (w - width) / 2;
            _cole = _cols + width - 1;
        }

        if (height >= h)
        {
            _y = (height - h) / 2;
            _rows = 0;
            _rowe = h -1;
        }
        else
        {
            _y = 0;
            _rows = (h - height) / 2;
            _rowe = _rows + height - 1;
        }

        uint8_t sdbuffer[3 * 20]; /* pixel buffer (R+G+B per pixel) */
        uint16_t *colors = (uint16_t*)malloc(sizeof(uint16_t) * w);

        /* For each scanline... */
        for (int row = 0; row < h; row++)
        {
            if (row < _rows || row > _rowe)
            {
                continue;
            }

            /*  Seek to start of scan line.	It might seem labor-
                intensive to be doing this on every line, but this
                method covers a lot of gritty details like cropping
                and scanline padding.  Also, the seek only takes
                place if the file position actually needs to change
                (avoids a lot of cluster math in SD library).
                Bitmap is stored bottom-to-top order (normal BMP) */
            int pos = result->header.offset + (h - 1 - row) * rowSize;
            f_lseek(fp, pos);
            int buffidx = sizeof(sdbuffer); /* Force buffer reload */
            int index = 0;
            /* For each pixel... */
            for (int col = 0; col < w; col++)
            {
                if (buffidx >= sizeof(sdbuffer))    /* Indeed */
                {
                    f_read(fp,sdbuffer, sizeof(sdbuffer), (UINT *)&br);
                    buffidx = 0;                    /* Set index to beginning */
                }
                
                if (col < _cols || col > _cole)
                {
                    continue;
                }

                /* Convert pixel from BMP to TFT format, push to display */
                uint8_t b = sdbuffer[buffidx++];
                uint8_t g = sdbuffer[buffidx++];
                uint8_t r = sdbuffer[buffidx++];
                colors[index++] = rgb565(r, g, b);
            }   /* end for col */
            pic_phy.multicolor(_x, _y, _w, colors);
            _y++;
        }   /* end for row */

        free(colors);
    }   /* end if */

    free(result);
    f_close(fp);

    endTick = xTaskGetTickCount();
    diffTick = endTick - startTick;
    ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
    return diffTick;
}

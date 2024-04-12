/**
 ****************************************************************************************************
 * @file        jpeg.c
 * @author      ALIENTEK
 * @brief       JPEG code
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

#include "jpeg.h"


/* The TJPGD outside the ROM code is newer and has different return type in decode callback */
typedef int jpeg_decode_out_t;
JDEC jpeg_dev;                  /* Decompressor object structure */

/**
 * @brief       Input function for jpeg decoder. Just returns bytes from the inData field of the JpegDev structure
 * @param       jd       : Decompressor object structure
 * @param       buf      : Pointer to the source buffer
 * @param       num      : Lenght
 * @retval      Number of bytes read
 */
static unsigned int infunc(JDEC *decoder, uint8_t *buf, unsigned int len)
{
    uint16_t  rb;
    FIL *dev = (FIL *)decoder->device;

    if (buf)                                /* Read nd bytes from the input strem */
    {
        f_read(dev, buf, len, (UINT *)&rb);
        return rb;
    }
    else                                    /* Skip nd bytes on the input stream */
    {
        return (f_lseek(dev, f_tell(dev) + len) == FR_OK) ? len : 0;
    }
}

/**
 * @brief       Output function. Re-encodes the RGB888 data from the decoder as big-endian RGB565 and stores it in the outData array of the JpegDev structure
 * @param       decoder : Decompressor object structure
 * @param       bitmap  : bitmap data 
 * @param       rect    : Rectangular structure
 * @retval      1:success
 */
static jpeg_decode_out_t outfunc(JDEC *decoder, void *bitmap, JRECT *rect)
{
    JDEC *jd = (JDEC *) decoder;
    uint8_t *in = (uint8_t *) bitmap;

    for (int y = rect->top; y <= rect->bottom; y++)
    {
        for (int x = rect->left; x <= rect->right; x++)
        {
            if (y < jd->screenHeight && x < jd->screenWidth)
            {
                jd->outData[y][x] = rgb565(in[0], in[1], in[2]);
            }

            in += 3;
        }
    }

    return 1;
}

/**
 * @brief       Specifies scaling factor N for output. The output image is descaled to 1 / 2 ^ N (N = 0 to 3)
 * @param       screenWidth     : Display width
 * @param       screenHeight    : Display height
 * @param       decodeWidth     : Decoding width
 * @param       decodeHeight    : Decoding height
 * @retval      0:Failed;1:2x ratio;2:4 times the proportion;3:Original size
 */
uint8_t getScale(int screenWidth, int screenHeight, uint16_t decodeWidth, uint16_t decodeHeight)
{
    if (screenWidth >= decodeWidth && screenHeight >= decodeHeight)
    {
        return 0;
    }

    double scaleWidth = (double)decodeWidth / (double)screenWidth;
    double scaleHeight = (double)decodeHeight / (double)screenHeight;
    double scale = scaleWidth;

    if (scaleWidth < scaleHeight)
    {
        scale = scaleHeight;
    }

    if (scale <= 2.0)
    {
        return 1;
    }

    if (scale <= 4.0)
    {
        return 2;
    }

    return 3;
}

/**
 * @brief       Decode the jpeg ``image.jpg`` embedded into the program file into pixel data
 * @param       pixels          : rgb565 format
 * @param       file            : File name
 * @param       screenWidth     : Display width
 * @param       screenHeight    : Display height
 * @param       imageWidth      : image width
 * @param       imageHeight     : image height
 * @retval      0:Failed;1:2x ratio;2:4 times the proportion;3:Original size
 */
esp_err_t decode_jpeg(pixel_jpeg ***pixels, char * file, int screenWidth, int screenHeight, int * imageWidth, int * imageHeight)
{
    char *work = NULL;
    FIL *f_jpeg = NULL;
    *pixels = NULL;
    JRESULT res = JDR_OK;
    esp_err_t ret = ESP_OK;
    uint32_t jd_work_size = 6144 + 4096;

    /* Alocate pixel memory. Each line is an array of IMAGE_W 16-bit pixels; the `*pixels` array itself contains pointers to these lines */
    *pixels = malloc( sizeof(pixel_jpeg *) * screenHeight);

    if (*pixels == NULL)
    {
        ESP_LOGE(__FUNCTION__, "Error allocating memory for lines");
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    for (int i = 0; i < screenHeight; i++)
    {
        (*pixels)[i] = malloc(screenWidth * sizeof(pixel_jpeg));

        if ((*pixels)[i] == NULL)
        {
            ESP_LOGE(__FUNCTION__, "Error allocating memory for line %d", i);
            ret = ESP_ERR_NO_MEM;
            goto err;
        }
    }

    /* Allocate the work space for the jpeg decoder */
    work = malloc(jd_work_size);

    if (work == NULL)
    {
        ESP_LOGE(__FUNCTION__, "Cannot allocate workspace");
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    
    /* Populate fields of the JpegDev struct */
    jpeg_dev.outData = *pixels;
    jpeg_dev.screenWidth = screenWidth;
    jpeg_dev.screenHeight = screenHeight;

    f_jpeg = (FIL *)malloc(sizeof(FIL));

    f_open(f_jpeg, (const TCHAR *)file, FA_READ);

    if (f_jpeg == NULL)
    {
        ESP_LOGW(__FUNCTION__, "Image file not found [%s]", file);
        ret = ESP_ERR_NOT_FOUND;
        goto err;
    }

    /* Prepare and decode the jpeg */
    res = jd_prepare(&jpeg_dev, infunc, work, jd_work_size, f_jpeg);

    if (res != JDR_OK)
    {
        ESP_LOGE(__FUNCTION__, "Image decoder: jd_prepare failed (%d)", res);
        ret = ESP_ERR_NOT_SUPPORTED;
        goto err;
    }

    /* Calculate Scaling factor */
    uint8_t scale = getScale(screenWidth, screenHeight, jpeg_dev.width, jpeg_dev.height);

    /* Calculate image size */
    double factor = 1.0;
    if (scale == 1) factor = 0.5;
    if (scale == 2) factor = 0.25;
    if (scale == 3) factor = 0.125;
    *imageWidth = (double)jpeg_dev.width * factor;
    *imageHeight = (double)jpeg_dev.height * factor;
    /* Start to decompress the JPEG picture */
    res = jd_decomp(&jpeg_dev, outfunc, scale);

    if (res != JDR_OK)
    {
        ESP_LOGE(__FUNCTION__, "Image decoder: jd_decode failed (%d)", res);
        ret = ESP_ERR_NOT_SUPPORTED;
        goto err;
    }

    /* All done! Free the work area (as we don't need it anymore) and return victoriously */
    free(work);
    f_close(f_jpeg);
    return ret;

    /* Something went wrong! Exit cleanly, de-allocating everything we allocated */
    err:
    f_close(f_jpeg);

    if (*pixels != NULL)
    {
        for (int i = 0; i < screenHeight; i++)
        {
            if ((*pixels)[i]) free((*pixels)[i]);
        }

        free(*pixels);
    }

    free(work);
    return ret;
}

/**
 * @brief       Release image memory
 * @param       pixels          : rgb565 format
 * @param       screenWidth     : Display width
 * @param       screenHeight    : Display height
 * @retval      ESP_OK:success;Other:fail
 */
esp_err_t release_image(pixel_jpeg ***pixels, int screenWidth, int screenHeight)
{
    if (*pixels != NULL)
    {
        for (int i = 0; i < screenHeight; i++)
        {
            free((*pixels)[i]);
        }
        free(*pixels);
    }

    return ESP_OK;
}

/**
 * @brief       JPEG image decoding
 * @param       filename      : Image file path(.bmp/.jpg/.jpeg/.gif/.png etc)
 * @param       width         : Display width
 * @param       height        : Display height
 * @retval      Return JPEG decoding speed
 */
TickType_t jpeg_decode(const char *filename, int width, int height)
{
    TickType_t startTick, endTick, diffTick;
    startTick = xTaskGetTickCount();

    pixel_jpeg **pixels;
    int imageWidth;
    int imageHeight;
    esp_err_t err = decode_jpeg(&pixels, (char *)filename, width, height, &imageWidth, &imageHeight);
    ESP_LOGI(__FUNCTION__, "decode_image err=%d imageWidth=%d imageHeight=%d", err, imageWidth, imageHeight);
    if (err == ESP_OK)
    {

        uint16_t _width = width;
        uint16_t _cols = 0;

        if (width > imageWidth)
        {
            _width = imageWidth;
            _cols = (width - imageWidth) / 2;
        }

        ESP_LOGI(__FUNCTION__, "_width=%d _cols=%d", _width, _cols);

        uint16_t _height = height;
        uint16_t _rows = 0;

        if (height > imageHeight)
        {
            _height = imageHeight;
            _rows = (height - imageHeight) / 2;
        }

        ESP_LOGI(__FUNCTION__, "_height=%d _rows=%d", _height, _rows);
        uint16_t *colors = (uint16_t*)malloc(sizeof(uint16_t) * _width);

        for(int y = 0; y < _height; y++)
        {
            for(int x = 0;x < _width; x++)
            {
                colors[x] = pixels[y][x];
            }

            pic_phy.multicolor(_cols, y+_rows, _width, colors);
            vTaskDelay(1);
        }

        free(colors);
        release_image(&pixels, width, height);
        ESP_LOGI(__FUNCTION__, "Finish");
    }
    else
    {
        ESP_LOGE(__FUNCTION__, "decode_jpeg fail=%d", err);
    }

    endTick = xTaskGetTickCount();
    diffTick = endTick - startTick;
    ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
    return diffTick;
}

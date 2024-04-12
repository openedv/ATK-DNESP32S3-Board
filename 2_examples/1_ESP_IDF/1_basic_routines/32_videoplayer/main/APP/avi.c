/**
 ****************************************************************************************************
 * @file        avi.c
 * @author      ALIENTEK
 * @brief       AVI code
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

#include "avi.h"


AVI_INFO g_avix;                                      /* AVI file related information */
char *const AVI_VIDS_FLAG_TBL[2] = {"00dc", "01dc"};  /* Video encoding flag string, 00dc/01dc */
char *const AVI_AUDS_FLAG_TBL[2] = {"00wb", "01wb"};  /* Audio encoding flag string, 00wb/01wb */

/**
 * @brief       AVI decoding initialization
 * @param       buf  : Input buffer
 * @param       size : Buffer size
 * @retval      res
 *    @arg      OK      : AVI file parsing successful
 *    @arg      Other   : error code
 */
AVISTATUS avi_init(uint8_t *buf, uint32_t size)
{
    uint16_t offset;
    uint8_t *tbuf;
    AVISTATUS res = AVI_OK;
    AVI_HEADER *aviheader;
    LIST_HEADER *listheader;
    AVIH_HEADER *avihheader;
    STRH_HEADER *strhheader;

    STRF_BMPHEADER *bmpheader;
    STRF_WAVHEADER *wavheader;

    tbuf = buf;
    aviheader = (AVI_HEADER *)buf;
    if (aviheader->RiffID != AVI_RIFF_ID)
    {
        return AVI_RIFF_ERR;        /* RIFF ID error */
    }

    if (aviheader->AviID != AVI_AVI_ID)
    {
        return AVI_AVI_ERR;         /* AVI ID error */
    }

    buf += sizeof(AVI_HEADER);      /* deviation */
    listheader = (LIST_HEADER *)(buf);
    if (listheader->ListID != AVI_LIST_ID)
    {
        return AVI_LIST_ERR;        /* LIST ID error */
    }

    if (listheader->ListType != AVI_HDRL_ID)
    {
        return AVI_HDRL_ERR;        /* HDRL ID error */
    }

    buf += sizeof(LIST_HEADER);     /* deviation */
    avihheader = (AVIH_HEADER *)(buf);
    if (avihheader->BlockID != AVI_AVIH_ID)
    {
        return AVI_AVIH_ERR;        /* AVIH ID error */
    }

    g_avix.SecPerFrame = avihheader->SecPerFrame;   /* Obtain frame interval time */
    g_avix.TotalFrame = avihheader->TotalFrame;     /* Obtain the total number of frames */
    buf += avihheader->BlockSize + 8;               /* deviation */
    listheader = (LIST_HEADER *)(buf);
    if (listheader->ListID != AVI_LIST_ID)
    {
        return AVI_LIST_ERR;        /* LIST ID error */
    }

    if (listheader->ListType != AVI_STRL_ID)
    {
        return AVI_STRL_ERR;        /* STRL ID error */
    }

    strhheader = (STRH_HEADER *)(buf + 12);
    if (strhheader->BlockID != AVI_STRH_ID)
    {
        return AVI_STRH_ERR;        /* STRH ID error */
    }

    if (strhheader->StreamType == AVI_VIDS_STREAM)
    {
        if (strhheader->Handler != AVI_FORMAT_MJPG)
        {
            return AVI_FORMAT_ERR;
        }

        g_avix.VideoFLAG = AVI_VIDS_FLAG_TBL[0];    /* "00dc" */
        g_avix.AudioFLAG = AVI_AUDS_FLAG_TBL[1];    /* "01wb" */
        bmpheader = (STRF_BMPHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (bmpheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;    /* STRF ID error */
        }

        g_avix.Width = bmpheader->bmiHeader.Width;
        g_avix.Height = bmpheader->bmiHeader.Height;
        buf += listheader->BlockSize + 8;       /* deviation */
        listheader = (LIST_HEADER *)(buf);
        if (listheader->ListID != AVI_LIST_ID)
        {
            g_avix.SampleRate = 0;
            g_avix.Channels = 0;
            g_avix.AudioType = 0;

        }
        else
        {
            if (listheader->ListType != AVI_STRL_ID)
            {
                return AVI_STRL_ERR;    /* STRL ID error */
            }

            strhheader = (STRH_HEADER *)(buf + 12);
            if (strhheader->BlockID != AVI_STRH_ID)
            {
                return AVI_STRH_ERR;    /* STRH ID error */
            }

            if (strhheader->StreamType != AVI_AUDS_STREAM)
            {
                return AVI_FORMAT_ERR;  /* Format error */
            }

            wavheader = (STRF_WAVHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
            if (wavheader->BlockID != AVI_STRF_ID)
            {
                return AVI_STRF_ERR;    /* STRF ID error */
            }

            g_avix.SampleRate = wavheader->SampleRate;      /* Audio sampling rate */
            g_avix.Channels = wavheader->Channels;          /* Number of audio channels */
            g_avix.AudioType = wavheader->FormatTag;        /* Audio Format */
        }
    }
    else if (strhheader->StreamType == AVI_AUDS_STREAM)
    { 
        g_avix.VideoFLAG = AVI_VIDS_FLAG_TBL[1];            /* "01dc" */
        g_avix.AudioFLAG = AVI_AUDS_FLAG_TBL[0];            /* "00wb" */
        wavheader = (STRF_WAVHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (wavheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;                            /* STRF ID error */
        }
 
        g_avix.SampleRate = wavheader->SampleRate;          /* Audio sampling rate */
        g_avix.Channels = wavheader->Channels;              /* Number of audio channels */
        g_avix.AudioType = wavheader->FormatTag;            /* Audio Format */
        buf += listheader->BlockSize + 8;                   /* deviation */
        listheader = (LIST_HEADER *)(buf);
        if (listheader->ListID != AVI_LIST_ID)
        {
            return AVI_LIST_ERR;    /* LIST ID error */
        }

        if (listheader->ListType != AVI_STRL_ID)
        {
            return AVI_STRL_ERR;    /* STRL ID error */
        }

        strhheader = (STRH_HEADER *)(buf + 12);
        if (strhheader->BlockID != AVI_STRH_ID)
        {
            return AVI_STRH_ERR;    /* STRH ID error */
        }

        if (strhheader->StreamType != AVI_VIDS_STREAM)
        {
            return AVI_FORMAT_ERR;  /* Format error */
        }

        bmpheader = (STRF_BMPHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (bmpheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;    /* STRF ID error */
        }

        if (bmpheader->bmiHeader.Compression != AVI_FORMAT_MJPG)
        {
            return AVI_FORMAT_ERR;  /* Format error */
        }

        g_avix.Width = bmpheader->bmiHeader.Width;
        g_avix.Height = bmpheader->bmiHeader.Height;
    }

    offset = avi_srarch_id(tbuf, size, "movi");     /* movi ID */
    if (offset == 0)
    {
        return AVI_MOVI_ERR;        /* MOVI ID error */
    }

    if (g_avix.SampleRate)
    {
        tbuf += offset;
        offset = avi_srarch_id(tbuf, size, g_avix.AudioFLAG);
        if (offset == 0)
        {
            return AVI_STREAM_ERR;
        }
        tbuf += offset + 4;
        g_avix.AudioBufSize = *((uint16_t *)tbuf);
    }

    printf("avi init ok\r\n");
    printf("g_avix.SecPerFrame:%ld\r\n", g_avix.SecPerFrame);
    printf("g_avix.TotalFrame:%ld\r\n", g_avix.TotalFrame);
    printf("g_avix.Width:%ld\r\n", g_avix.Width);
    printf("g_avix.Height:%ld\r\n", g_avix.Height);
    printf("g_avix.AudioType:%d\r\n", g_avix.AudioType);
    printf("g_avix.SampleRate:%ld\r\n", g_avix.SampleRate);
    printf("g_avix.Channels:%d\r\n", g_avix.Channels);
    printf("g_avix.AudioBufSize:%d\r\n", g_avix.AudioBufSize);
    printf("g_avix.VideoFLAG:%s\r\n", g_avix.VideoFLAG);
    printf("g_avix.AudioFLAG:%s\r\n", g_avix.AudioFLAG);

    return res;
}

/**
 * @brief       Find ID
 * @param       buf  : Input buffer
 * @param       size : Buffer size
 * @param       id   : The ID to be searched must be a 4-byte length
 * @retval      results of enforcement
 *   @arg       0     : Not found
 *   @arg       Other : movi ID
 */
uint32_t avi_srarch_id(uint8_t *buf, uint32_t size, char *id)
{
    uint32_t i;
    uint32_t idsize = 0;
    size -= 4;
    for (i = 0; i < size; i++)
    {
        if ((buf[i] == id[0]) &&
            (buf[i + 1] == id[1]) &&
            (buf[i + 2] == id[2]) &&
            (buf[i + 3] == id[3]))
        {
            idsize = MAKEDWORD(buf + i + 4);

            if (idsize > 0X10)return i;
        }
    }

    return 0;
}

/**
 * @brief       Obtain stream information
 * @param       buf  : Stream start address
 * @retval      results of enforcement
 *   @arg       AVI_OK : AVI file parsing successful
 *   @arg       Other  : error code
 */
AVISTATUS avi_get_streaminfo(uint8_t *buf)
{
    g_avix.StreamID = MAKEWORD(buf + 2);
    g_avix.StreamSize = MAKEDWORD(buf + 4);

    if (g_avix.StreamSize > AVI_MAX_FRAME_SIZE)
    {
        printf("FRAME SIZE OVER:%ld\r\n", g_avix.StreamSize);
        g_avix.StreamSize = 0;
        return AVI_STREAM_ERR;
    }
    
    if (g_avix.StreamSize % 2)
    {
        g_avix.StreamSize++;
    }

    if (g_avix.StreamID == AVI_VIDS_FLAG || g_avix.StreamID == AVI_AUDS_FLAG)
    {
        return AVI_OK;
    }

    return AVI_STREAM_ERR;
}

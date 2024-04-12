/**
 ****************************************************************************************************
 * @file        avi.h
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


#ifndef __AVI_H
#define __AVI_H


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "exfuns.h"
#include "ff.h"


#define AVI_MAX_FRAME_SIZE  60 * 1024

/* Error type */
typedef enum
{
    AVI_OK = 0,
    AVI_RIFF_ERR,
    AVI_AVI_ERR,
    AVI_LIST_ERR,
    AVI_HDRL_ERR,
    AVI_AVIH_ERR,
    AVI_STRL_ERR,
    AVI_STRH_ERR,
    AVI_STRF_ERR,
    AVI_MOVI_ERR,
    AVI_FORMAT_ERR,
    AVI_STREAM_ERR,
} AVISTATUS;

#define AVI_RIFF_ID         0X46464952
#define AVI_AVI_ID          0X20495641
#define AVI_LIST_ID         0X5453494C
#define AVI_HDRL_ID         0X6C726468
#define AVI_MOVI_ID         0X69766F6D
#define AVI_STRL_ID         0X6C727473

#define AVI_AVIH_ID         0X68697661
#define AVI_STRH_ID         0X68727473
#define AVI_STRF_ID         0X66727473
#define AVI_STRD_ID         0X64727473

#define AVI_VIDS_STREAM     0X73646976
#define AVI_AUDS_STREAM     0X73647561


#define AVI_VIDS_FLAG       0X6463
#define AVI_AUDS_FLAG       0X7762

#define AVI_FORMAT_MJPG     0X47504A4D


typedef struct
{
    uint32_t SecPerFrame;
    uint32_t TotalFrame;
    uint32_t Width;
    uint32_t Height;
    uint32_t SampleRate;
    uint16_t Channels;
    uint16_t AudioBufSize;
    uint16_t AudioType;
    uint16_t StreamID;
    uint32_t StreamSize;
    char *VideoFLAG;
    char *AudioFLAG;
} AVI_INFO;

extern AVI_INFO g_avix;

typedef struct
{
    uint32_t RiffID;            /* RiffID=='RIFF'==0X61766968 */
    uint32_t FileSize;
    uint32_t AviID;             /* AviID=='AVI '==0X41564920 */
} AVI_HEADER;

typedef struct
{
    uint32_t FrameID;           /* FrameID=='RIFF'==0X61766968 */
    uint32_t FrameSize;         /* Frame Size */
} FRAME_HEADER;


typedef struct
{
    uint32_t ListID;            /* ListID=='LIST'==0X4c495354 */
    uint32_t BlockSize;
    uint32_t ListType;
} LIST_HEADER;

typedef struct
{
    uint32_t BlockID;
    uint32_t BlockSize;
    uint32_t SecPerFrame;
    uint32_t MaxByteSec;
    uint32_t PaddingGranularity;
    uint32_t Flags;
    uint32_t TotalFrame;
    uint32_t InitFrames;
    uint32_t Streams;
    uint32_t RefBufSize;
    uint32_t Width;
    uint32_t Height;
    uint32_t Reserved[4];
} AVIH_HEADER;

typedef struct
{
    uint32_t BlockID;
    uint32_t BlockSize;
    uint32_t StreamType;
    uint32_t Handler;
    uint32_t Flags;
    uint16_t Priority;
    uint16_t Language;
    uint32_t InitFrames;
    uint32_t Scale;
    uint32_t Rate;
    uint32_t Start;
    uint32_t Length;
    uint32_t RefBufSize;
    uint32_t Quality;
    uint32_t SampleSize;
    struct
    {
        short Left;
        short Top;
        short Right;
        short Bottom;
    } Frame;
} STRH_HEADER;

typedef struct
{
    uint32_t BmpSize;
    long Width;
    long Height;
    uint16_t  Planes;
    uint16_t  BitCount;
    uint32_t  Compression;
    uint32_t  SizeImage;
    long XpixPerMeter;
    long YpixPerMeter;
    uint32_t  ClrUsed;
    uint32_t  ClrImportant;
} BMP_HEADER;

typedef struct
{
    uint8_t  rgbBlue;
    uint8_t  rgbGreen;
    uint8_t  rgbRed;
    uint8_t  rgbReserved;
} AVIRGBQUAD;

typedef struct
{
    uint32_t BlockID;
    uint32_t BlockSize;
    BMP_HEADER bmiHeader;
    AVIRGBQUAD bmColors[1];
} STRF_BMPHEADER;

typedef struct
{
    uint32_t BlockID;
    uint32_t BlockSize;
    uint16_t FormatTag;
    uint16_t Channels;
    uint32_t SampleRate;
    uint32_t BaudRate;
    uint16_t BlockAlign;
    uint16_t Size;
} STRF_WAVHEADER;

#define	 MAKEWORD(ptr)	(uint16_t)(((uint16_t)*((uint8_t*)(ptr))<<8)|(uint16_t)*(uint8_t*)((ptr)+1))
#define  MAKEDWORD(ptr)	(uint32_t)(((uint16_t)*(uint8_t*)(ptr)|(((uint16_t)*(uint8_t*)(ptr+1))<<8)|\
                               (((uint16_t)*(uint8_t*)(ptr+2))<<16)|(((uint16_t)*(uint8_t*)(ptr+3))<<24)))

/* function declaration */
AVISTATUS avi_init(uint8_t *buf, uint32_t size);
uint32_t avi_srarch_id(uint8_t *buf, uint32_t size, char *id);
AVISTATUS avi_get_streaminfo(uint8_t *buf);

#endif

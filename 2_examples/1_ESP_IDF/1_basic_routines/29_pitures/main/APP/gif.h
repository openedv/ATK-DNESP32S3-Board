/**
 ****************************************************************************************************
 * @file        gif.h
 * @author      ALIENTEK
 * @brief       GIF code
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

#ifndef __GIF_H
#define __GIF_H

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "ff.h"


/******************************************************************************************/
/* user Configuration Area */

#define GIF_USE_MALLOC          1       /* define whether to use malloc, here we choose to use malloc */

/******************************************************************************************/


#define LCD_MAX_LOG_COLORS      256
#define MAX_NUM_LWZ_BITS        12


#define GIF_INTRO_TERMINATOR    ';'     /* 0X3B */
#define GIF_INTRO_EXTENSION     '!'     /* 0X21 */
#define GIF_INTRO_IMAGE         ','     /* 0X2C */

#define GIF_COMMENT             0xFE
#define GIF_APPLICATION         0xFF
#define GIF_PLAINTEXT           0x01
#define GIF_GRAPHICCTL          0xF9

typedef struct
{
    uint8_t    aBuffer[258];                    /*  Input buffer for data block */
    short aCode  [(1 << MAX_NUM_LWZ_BITS)];     /*  This array stores the LZW codes for the compressed strings */
    uint8_t    aPrefix[(1 << MAX_NUM_LWZ_BITS)];/*  Prefix character of the LZW code */
    uint8_t    aDecompBuffer[3000];             /*  Decompression buffer. The higher the compression, the more bytes are needed in the buffer */
    uint8_t   *sp;                              /*  Pointer into the decompression buffer */
    int   CurBit;
    int   LastBit;
    int   GetDone;
    int   LastByte;
    int   ReturnClear;
    int   CodeSize;
    int   SetCodeSize;
    int   MaxCode;
    int   MaxCodeSize;
    int   ClearCode;
    int   EndCode;
    int   FirstCode;
    int   OldCode;
} LZW_INFO;

typedef struct
{
    uint16_t width;     /* GIF width */
    uint16_t height;    /* GIF height */
    uint8_t flag;       /* identifier */
    uint8_t bkcindex;   /* Index of background color in global color table */
    uint8_t pixratio;   /* Pixel Aspect Ratio */
} LogicalScreenDescriptor;

typedef struct
{
    uint16_t xoff;      /* X-direction offset */
    uint16_t yoff;      /* Y-direction offset */
    uint16_t width;     /* width */
    uint16_t height;    /* height */
    uint8_t flag;       /* identifier */
} ImageScreenDescriptor;

typedef struct
{
    LogicalScreenDescriptor gifLSD;
    ImageScreenDescriptor gifISD;
    uint16_t colortbl[256];
    uint16_t bkpcolortbl[256];
    uint16_t numcolors;
    uint16_t delay;
    LZW_INFO *lzw;
} gif89a;

extern uint8_t g_gif_decoding;      /* GIF is decoding tags */


/* function declaration */
void gif_quit(void);
uint8_t gif_getinfo(FIL *file, gif89a *gif);
uint8_t gif_decode(const char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height);


#endif

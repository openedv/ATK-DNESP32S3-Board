/**
 ****************************************************************************************************
 * @file        exfuns.h
 * @author      ALIENTEK
 * @brief       Exfuns code
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

#ifndef __EXFUNS_H
#define __EXFUNS_H

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "ff.h"


extern FATFS *fs[FF_VOLUMES];
extern FIL *file;
extern FIL *ftemp;
extern UINT br, bw;
extern FILINFO fileinfo;
extern FF_DIR dir;
extern uint8_t *fatbuf;


#define T_BIN   0x00    /* BIN file */
#define T_LRC   0x10    /* LRC file */
#define T_NES   0x20    /* NES file */
#define T_SMS   0x21    /* SMS file */
#define T_TEXT  0x30    /* TXT file */
#define T_C     0x31    /* C file */
#define T_H     0x32    /* H file */
#define T_WAV   0x40    /* WAV file */
#define T_MP3   0x41    /* MP3 file */
#define T_APE   0x42    /* APE file */
#define T_FLAC  0x43    /* FLAC file */
#define T_BMP   0x50    /* BMP file */
#define T_JPG   0x51    /* JPG file */
#define T_JPEG  0x52    /* JPEG file */
#define T_GIF   0x53    /* GIF file */
#define T_PNG   0x54    /* GIF file */
#define T_AVI   0x60    /* AVI file */

/* function declaration */
uint8_t exfuns_init(void);
uint8_t exfuns_file_type(char *fname);
uint8_t exfuns_get_free(uint8_t *pdrv, uint32_t *total, uint32_t *free);
uint32_t exfuns_get_folder_size(uint8_t *fdname);
uint8_t *exfuns_get_src_dname(uint8_t *dpfn);
uint8_t exfuns_file_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t totsize, uint32_t cpdsize, uint8_t fwmode);
uint8_t exfuns_folder_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode);

#endif

























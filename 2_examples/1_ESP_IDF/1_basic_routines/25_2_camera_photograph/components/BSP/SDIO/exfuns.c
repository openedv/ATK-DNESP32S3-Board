/**
 ****************************************************************************************************
 * @file        exfuns.c
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

#include "exfuns.h"


#define FILE_MAX_TYPE_NUM       7       /* number of file types */
#define FILE_MAX_SUBT_NUM       7       /* Number of files */

/* file type */
static const char *FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM] = {
    {"BIN"," "," "," "," "," "," "},                        /* BIN file */
    {"LRC"," "," "," "," "," "," "},                        /* LRC file */
    {"NES", "SMS"," "," "," "," "," "},                     /* NES/SMS file */
    {"TXT", "C", "H"," "," "," "," "},                      /* text file */
    {"WAV", "MP3", "OGG", "FLAC", "AAC", "WMA", "MID"},     /* misuc file */
    {"BMP", "JPG", "JPEG", "GIF","PNG"," "," "},            /* image file */
    {"AVI"," "," "," "," "," "," "},                        /* video file */
};

/******************************************************************************************/

/* Number of volumes (logical drives) to be used. (1-10) */
FATFS *fs[FF_VOLUMES];

/******************************************************************************************/


/**
 * @brief       apply for memory
 * @param       None
 * @retval      0:succeed; !0:failed
 */
uint8_t exfuns_init(void)
{
    uint8_t i;
    uint8_t res = 0;

    for (i = 0; i < FF_VOLUMES; i++)
    {
        fs[i] = (FATFS *)malloc(sizeof(FATFS));

        if (!fs[i])break;
    }
    
    if (i == FF_VOLUMES && res == 0)
    {
        return 0;
    }
    else 
    {
        return 1;
    }
}

/**
 * @brief       Convert lowercase letters to uppercase letters, and if it is a number, keep it unchanged
 * @param       c : The letter to be converted
 * @retval      Converted uppercase letters
 */
uint8_t exfuns_char_upper(uint8_t c)
{
    if (c < 'A')return c;

    if (c >= 'a')
    {
        return c - 0x20;
    }
    else
    {
        return c;
    }
}

/**
 * @brief       Detect the type of file
 * @param       fname : file name
 * @retval      file type
 *   @arg       0XFF  : Unrecognized file type number.
 *   @arg       Other, the high four digits indicate the major category, and the low four digits indicate the minor category
 */
uint8_t exfuns_file_type(char *fname)
{
    uint8_t tbuf[5];
    char *str_tbuf;
    char *attr = 0;
    uint8_t i = 0, j;

    str_tbuf = malloc(5);

    while (i < 250)
    {
        i++;

        if (*fname == '\0')break;

        fname++;
    }

    if (i == 250)return 0XFF;

    for (i = 0; i < 5; i++)
    {
        fname--;

        if (*fname == '.')
        {
            fname++;
            attr = fname;
            break;
        }
    }

    if (attr == 0)return 0XFF;

    strcpy((char *)tbuf, (const char *)attr);

    for (i = 0; i < 4; i++)tbuf[i] = exfuns_char_upper(tbuf[i]);

    for (i = 0; i < FILE_MAX_TYPE_NUM; i++)
    {
        for (j = 0; j < FILE_MAX_SUBT_NUM; j++)
        {
            if (*FILE_TYPE_TBL[i][j] == 0)
            {
                free(str_tbuf);
                break;
            }

            sprintf(str_tbuf,"%s",FILE_TYPE_TBL[i][j]);

            if (strcmp((const char *)str_tbuf, (const char *)tbuf) == 0)
            {
                return (i << 4) | j;
            }
        }
    }

    return 0XFF;
}

/**
 * @brief       Obtain remaining disk capacity
 * @param       pdrv : Disk number ("0:"~"9:")
 * @param       total: Total capacity (KB)
 * @param       free : Remaining capacity (KB)
 * @retval      0:succeed; !0:failed
 */
uint8_t exfuns_get_free(uint8_t *pdrv, uint32_t *total, uint32_t *free)
{
    FATFS *fs1;
    uint8_t res;
    uint32_t fre_clust = 0, fre_sect = 0, tot_sect = 0;
    /* get number of free clusters */
    res = (uint32_t)f_getfree((const TCHAR *)pdrv, (DWORD *)&fre_clust, &fs1);

    if (res == 0)
    {
        tot_sect = (fs1->n_fatent - 2) * fs1->csize;
        fre_sect = fre_clust * fs1->csize;
#if FF_MAX_SS!=512
        tot_sect *= fs1->ssize / 512;
        fre_sect *= fs1->ssize / 512;
#endif
        *total = tot_sect >> 1;
        *free = fre_sect >> 1;
    }

    return res;
}

/**
 * @brief       File copying
 * @param       fcpymsg : Function pointer, used to display information during copying
 *                  pname:File/folder name
 *                  pct:percentage
 *                  mode:
 *                      bit0 : Update file name
 *                      bit1 : Update percentage pct
 *                      bit2 : Update folder
 *                      Other : reserved
 *                  Return value: 0, normal; 1. Forced exit
 *
 * @param       psrc    : source file
 * @param       pdst    : Target file
 * @param       totsize : Total size
 * @param       cpdsize : Copied size
 * @param       fwmode  : File write mode
 *   @arg       0: Do not overwrite existing files
 *   @arg       1: Overwrite existing files
 *
 * @retval      results of enforcement
 *   @arg       0     : normal
 *   @arg       0XFF  : forced return
 *   @arg       Other : error code
 */
uint8_t exfuns_file_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, 
                        uint32_t totsize, uint32_t cpdsize, uint8_t fwmode)
{
    uint8_t res;
    uint16_t br = 0;
    uint16_t bw = 0;
    FIL *fsrc = 0;
    FIL *fdst = 0;
    uint8_t *fbuf = 0;
    uint8_t curpct = 0;
    unsigned long long lcpdsize = cpdsize;
    
    fsrc = (FIL *)malloc(sizeof(FIL));
    fdst = (FIL *)malloc(sizeof(FIL));
    fbuf = (uint8_t *)malloc(8192);

    if (fsrc == NULL || fdst == NULL || fbuf == NULL)
    {
        res = 100;
    }
    else
    {
        if (fwmode == 0)
        {
            fwmode = FA_CREATE_NEW;
        }
        else 
        {
            fwmode = FA_CREATE_ALWAYS;
        }
        
        res = f_open(fsrc, (const TCHAR *)psrc, FA_READ | FA_OPEN_EXISTING);

        if (res == 0)res = f_open(fdst, (const TCHAR *)pdst, FA_WRITE | fwmode);

        if (res == 0)
        {
            if (totsize == 0)
            {
                totsize = fsrc->obj.objsize;
                lcpdsize = 0;
                curpct = 0;
            }
            else
            {
                curpct = (lcpdsize * 100) / totsize;
            }
            
            fcpymsg(psrc, curpct, 0X02);

            while (res == 0)
            {
                res = f_read(fsrc, fbuf, 8192, (UINT *)&br);

                if (res || br == 0)break;

                res = f_write(fdst, fbuf, (UINT)br, (UINT *)&bw);
                lcpdsize += bw;

                if (curpct != (lcpdsize * 100) / totsize)
                {
                    curpct = (lcpdsize * 100) / totsize;

                    if (fcpymsg(psrc, curpct, 0X02))
                    {
                        res = 0XFF;
                        break;
                    }
                }

                if (res || bw < br)break;
            }

            f_close(fsrc);
            f_close(fdst);
        }
    }

    free(fsrc);
    free(fdst);
    free(fbuf);
    return res;
}

/**
 * @brief       get the folder under the path
 * @param       pname   : path
 * @retval      0       : Volume label number
 *              Other   : First address of folder name
 */
uint8_t *exfuns_get_src_dname(uint8_t *pname)
{
    uint16_t temp = 0;

    while (*pname != 0)
    {
        pname++;
        temp++;
    }

    if (temp < 4)return 0;

    while ((*pname != 0x5c) && (*pname != 0x2f))pname--;

    return ++pname;
}

/**
 * @brief       Get folder size(The file size should not exceed 4GB)
 * @param       pname   : path
 * @retval      0       : The folder size is 0, or an error occurred during the reading process
 *              Other   : Folder size
 */
uint32_t exfuns_get_folder_size(uint8_t *fdname)
{
#define MAX_PATHNAME_DEPTH  512 + 1
    uint8_t res = 0;
    FF_DIR *fddir = 0;
    FILINFO *finfo = 0;
    uint8_t *pathname = 0;
    uint16_t pathlen = 0;
    uint32_t fdsize = 0;

    fddir = (FF_DIR *)malloc(sizeof(FF_DIR));
    finfo = (FILINFO *)malloc(sizeof(FILINFO));

    if (fddir == NULL || finfo == NULL)res = 100;

    if (res == 0)
    {
        pathname = malloc(MAX_PATHNAME_DEPTH);

        if (pathname == NULL)res = 101;

        if (res == 0)
        {
            pathname[0] = 0;
            strcat((char *)pathname, (const char *)fdname);
            res = f_opendir(fddir, (const TCHAR *)fdname);

            if (res == 0)
            {
                while (res == 0)
                {
                    res = f_readdir(fddir, finfo);

                    if (res != FR_OK || finfo->fname[0] == 0)break;

                    if (finfo->fname[0] == '.')continue;

                    if (finfo->fattrib & 0X10)
                    {
                        pathlen = strlen((const char *)pathname);
                        strcat((char *)pathname, (const char *)"/");
                        strcat((char *)pathname, (const char *)finfo->fname);
                        //printf("\r\nsub folder:%s\r\n",pathname);
                        fdsize += exfuns_get_folder_size(pathname);
                        pathname[pathlen] = 0;
                    }
                    else
                    {
                        fdsize += finfo->fsize;
                    }
                }
            }

            free(pathname);
        }
    }

    free(fddir);
    free(finfo);

    if (res)
    {
        return 0;
    }
    else 
    {
        return fdsize;
    }
}

/**
 * @brief       Folder copying(The file size should not exceed 4GB)
 * @param       fcpymsg : Function pointer, used to display information during copying
 *                  pname:File/folder name
 *                  pct:percentage
 *                  mode:
 *                      bit0 : Update file name
 *                      bit1 : Update percentage pct
 *                      bit2 : Update folder
 *                      Other: reserved
 *                  Return value: 0:normal,1:Forced exit
 *
 * @param       psrc    : Source folder
 * @param       pdst    : Target folder
 *
 * @param       totsize : Total size
 * @param       cpdsize : The size that has been copied
 * @param       fwmode  : File write mode
 *   @arg       0: Do not overwrite existing files
 *   @arg       1: Overwrite existing files
 *
 * @retval      results of enforcement
 *   @arg       0     : normal
 *   @arg       0XFF  : forced return
 *   @arg       Other : error code
 */
uint8_t exfuns_folder_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, 
                           uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode)
{
#define MAX_PATHNAME_DEPTH 512 + 1
    uint8_t res = 0;
    FF_DIR *srcdir = 0;
    FF_DIR *dstdir = 0;
    FILINFO *finfo = 0;
    uint8_t *fn = 0;

    uint8_t *dstpathname = 0;
    uint8_t *srcpathname = 0;

    uint16_t dstpathlen = 0;
    uint16_t srcpathlen = 0;


    srcdir = (FF_DIR *)malloc(sizeof(FF_DIR));
    dstdir = (FF_DIR *)malloc(sizeof(FF_DIR));
    finfo = (FILINFO *)malloc(sizeof(FILINFO));

    if (srcdir == NULL || dstdir == NULL || finfo == NULL)res = 100;

    if (res == 0)
    {
        dstpathname = malloc(MAX_PATHNAME_DEPTH);
        srcpathname = malloc(MAX_PATHNAME_DEPTH);

        if (dstpathname == NULL || srcpathname == NULL)res = 101;

        if (res == 0)
        {
            dstpathname[0] = 0;
            srcpathname[0] = 0;
            strcat((char *)srcpathname, (const char *)psrc);
            strcat((char *)dstpathname, (const char *)pdst);
            res = f_opendir(srcdir, (const TCHAR *)psrc);

            if (res == 0)
            {
                strcat((char *)dstpathname, (const char *)"/");
                fn = exfuns_get_src_dname(psrc);

                if (fn == 0)
                {
                    dstpathlen = strlen((const char *)dstpathname);
                    dstpathname[dstpathlen] = psrc[0];
                    dstpathname[dstpathlen + 1] = 0;
                }
                else strcat((char *)dstpathname, (const char *)fn);

                fcpymsg(fn, 0, 0X04);
                res = f_mkdir((const TCHAR *)dstpathname);

                if (res == FR_EXIST)res = 0;

                while (res == 0)
                {
                    res = f_readdir(srcdir, finfo);

                    if (res != FR_OK || finfo->fname[0] == 0)break;

                    if (finfo->fname[0] == '.')continue;

                    fn = (uint8_t *)finfo->fname;
                    dstpathlen = strlen((const char *)dstpathname);
                    srcpathlen = strlen((const char *)srcpathname);

                    strcat((char *)srcpathname, (const char *)"/");

                    if (finfo->fattrib & 0X10)
                    {
                        strcat((char *)srcpathname, (const char *)fn);
                        res = exfuns_folder_copy(fcpymsg, srcpathname, dstpathname, totsize, cpdsize, fwmode);
                    }
                    else
                    {
                        strcat((char *)dstpathname, (const char *)"/");
                        strcat((char *)dstpathname, (const char *)fn);
                        strcat((char *)srcpathname, (const char *)fn);
                        fcpymsg(fn, 0, 0X01);
                        res = exfuns_file_copy(fcpymsg, srcpathname, dstpathname, *totsize, *cpdsize, fwmode);
                        *cpdsize += finfo->fsize;
                    }

                    srcpathname[srcpathlen] = 0;
                    dstpathname[dstpathlen] = 0;
                }
            }

            free(dstpathname);
            free(srcpathname);
        }
    }

    free(srcdir);
    free(dstdir);
    free(finfo);
    return res;
}

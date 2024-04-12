/**
 ****************************************************************************************************
 * @file        gif.c
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

#include "gif.h"
#include "piclib.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


const uint16_t _aMaskTbl[16] =
{
    0x0000, 0x0001, 0x0003, 0x0007,
    0x000f, 0x001f, 0x003f, 0x007f,
    0x00ff, 0x01ff, 0x03ff, 0x07ff,
    0x0fff, 0x1fff, 0x3fff, 0x7fff,
};
const uint8_t _aInterlaceOffset[] = {8, 8, 4, 2};
const uint8_t _aInterlaceYPos  [] = {0, 4, 2, 1};

uint8_t g_gif_decoding = 0;


/* Define whether to use malloc, here we choose to use malloc */
#if GIF_USE_MALLOC == 0
gif89a tgif89a;         /* gif89a文件 */
FIL f_gfile;            /* gif 文件 */
LZW_INFO tlzw;          /* lzw */
#endif


/**
 * @brief       Detect GIF header
 * @param       filename : File path
 * @retval      Judging the result
 *   @arg       0     : GIF89a/87a
 *   @arg       Other : not GIF89a/87a
 */
static uint8_t gif_check_head(FIL *filename)
{
    uint8_t gifversion[6];
    uint32_t readed;
    uint8_t res;
    res = f_read(filename, gifversion, 6, (UINT *)&readed);

    if (res)return 1;

    if ((gifversion[0] != 'G') || (gifversion[1] != 'I') || (gifversion[2] != 'F') ||
        (gifversion[3] != '8') || ((gifversion[4] != '7') && (gifversion[4] != '9')) ||
        (gifversion[5] != 'a'))
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief       RGB888 to RGB565
 * @param       ctb : RGB888 color array first address
 * @retval      RGB565 color value
 */
static uint16_t gif_getrgb565(uint8_t *ctb)
{
    uint16_t r, g, b;
    r = (ctb[0] >> 3) & 0X1F;
    g = (ctb[1] >> 2) & 0X3F;
    b = (ctb[2] >> 3) & 0X1F;
    return b + (g << 5) + (r << 11);
}

/**
 * @brief       Read color table
 * @param       filename :  File path
 * @param       gif      : GIF information
 * @param       numcolors: Color Table Size
 * @retval      Operation results
 *   @arg       0        : succeed
 *   @arg       Other    : failed
 */
static uint8_t gif_readcolortbl(FIL *filename, gif89a *gif, uint16_t numcolors)
{
    uint8_t rgb[3];
    uint16_t t;
    uint8_t res;
    uint32_t readed;

    for (t = 0; t < numcolors; t++)
    {
        res = f_read(filename, rgb, 3, (UINT *)&readed);

        if (res)return 1;

        gif->colortbl[t] = gif_getrgb565(rgb);
    }

    return 0;
}

/**
 * @brief       Obtain GIF information
 * @param       filename : File path
 * @param       gif      : GIF information
 * @retval      Operation results
 *   @arg       0        : succeed
 *   @arg       Other    : failed
 */
uint8_t gif_getinfo(FIL *file, gif89a *gif)
{
    uint32_t readed;
    uint8_t res;
    res = f_read(file, (uint8_t *)&gif->gifLSD, 7, (UINT *)&readed);

    if (res)return 1;

    if (gif->gifLSD.flag & 0x80)
    {
        gif->numcolors = 2 << (gif->gifLSD.flag & 0x07);

        if (gif_readcolortbl(file, gif, gif->numcolors))
        {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief       Save Global Color Table
 * @param       gif      : GIF information
 * @retval      None
 */
static void gif_savegctbl(gif89a *gif)
{
    uint16_t i = 0;

    for (i = 0; i < 256; i++)
    {
        gif->bkpcolortbl[i] = gif->colortbl[i];
    }
}

/**
 * @brief       Restore Global Color Table
 * @param       gif      : GIF information
 * @retval      None
 */
static void gif_recovergctbl(gif89a *gif)
{
    uint16_t i = 0;

    for (i = 0; i < 256; i++)
    {
        gif->colortbl[i] = gif->bkpcolortbl[i];
    }
}

/**
 * @brief       Initialize LZW related parameters
 * @param       gif      : GIF information
 * @param       codesize : Lzw code length
 * @retval      None
 */
static void gif_initlzw(gif89a *gif, uint8_t codesize)
{
    memset((uint8_t *)gif->lzw, 0, sizeof(LZW_INFO));
    gif->lzw->SetCodeSize  = codesize;
    gif->lzw->CodeSize     = codesize + 1;
    gif->lzw->ClearCode    = (1 << codesize);
    gif->lzw->EndCode      = (1 << codesize) + 1;
    gif->lzw->MaxCode      = (1 << codesize) + 2;
    gif->lzw->MaxCodeSize  = (1 << codesize) << 1;
    gif->lzw->ReturnClear  = 1;
    gif->lzw->LastByte     = 2;
    gif->lzw->sp           = gif->lzw->aDecompBuffer;
}

/**
 * @brief       Read a data block
 * @param       filename : File path
 * @param       buf      : Pointer to the source buffer
 * @param       maxnum   : Maximum read data
 * @retval      None
 */
static uint16_t gif_getdatablock(FIL *filename, uint8_t *buf, uint16_t maxnum)
{
    uint8_t cnt;
    uint32_t readed;
    uint32_t fpos;
    f_read(filename, &cnt, 1, (UINT *)&readed); /* Lzw code length */

    if (cnt)
    {
        if (buf)
        {
            if (cnt > maxnum)
            {
                fpos = f_tell(filename);
                f_lseek(filename, fpos + cnt);
                return cnt;
            }

            f_read(filename, buf, cnt, (UINT *)&readed);    /* Lzw code length */
        }
        else
        {
            fpos = f_tell(filename);
            f_lseek(filename, fpos + cnt);
        }
    }

    return cnt;
}

/**
 * @brief       Read an extension block
 * @param       filename    : File path
 * @param       gif         : GIF information
 * @param       pTransIndex : Transparent color chart
 * @param       pDisposal   : processing method
 * @retval      Operation results
 *   @arg       0        : succeed
 *   @arg       Other    : failed
 */
static uint8_t gif_readextension(FIL *filename, gif89a *gif, int *pTransIndex, uint8_t *pDisposal)
{
    uint8_t temp;
    uint32_t readed;
    uint8_t buf[4];
    f_read(filename, &temp, 1, (UINT *)&readed);

    switch (temp)
    {
        case GIF_PLAINTEXT:
        case GIF_APPLICATION:
        case GIF_COMMENT:
            while (gif_getdatablock(filename, 0, 256) > 0);

            return 0;

        case GIF_GRAPHICCTL:
            if (gif_getdatablock(filename, buf, 4) != 4)return 1;

            gif->delay = (buf[2] << 8) | buf[1];
            *pDisposal = (buf[0] >> 2) & 0x7;

            if ((buf[0] & 0x1) != 0)*pTransIndex = buf[3];

            f_read(filename, &temp, 1, (UINT *)&readed);

            if (temp != 0)return 1;

            return 0;
    }

    return 1;
}

/**
 * @brief       Retrieve the next LZW code from the LZW cache, each containing 12 bits
 * @param       filename : File path
 * @param       gif      : GIF information
 * @retval      Operation results
 */
static int gif_getnextcode(FIL *filename, gif89a *gif)
{
    int i, j, End;
    long Result;

    if (gif->lzw->ReturnClear)
    {
        /* The first code should be a clearcode. */
        gif->lzw->ReturnClear = 0;
        return gif->lzw->ClearCode;
    }

    End = gif->lzw->CurBit + gif->lzw->CodeSize;

    if (End >= gif->lzw->LastBit)
    {
        int Count;

        if (gif->lzw->GetDone)return -1; /* Error */

        gif->lzw->aBuffer[0] = gif->lzw->aBuffer[gif->lzw->LastByte - 2];
        gif->lzw->aBuffer[1] = gif->lzw->aBuffer[gif->lzw->LastByte - 1];

        if ((Count = gif_getdatablock(filename, &gif->lzw->aBuffer[2], 300)) == 0)gif->lzw->GetDone = 1;

        if (Count < 0)return -1;    /* Error */

        gif->lzw->LastByte = 2 + Count;
        gif->lzw->CurBit = (gif->lzw->CurBit - gif->lzw->LastBit) + 16;
        gif->lzw->LastBit = (2 + Count) * 8;
        End = gif->lzw->CurBit + gif->lzw->CodeSize;
    }

    j = End >> 3;
    i = gif->lzw->CurBit >> 3;

    if (i == j)Result = (long)gif->lzw->aBuffer[i];
    else if (i + 1 == j)Result = (long)gif->lzw->aBuffer[i] | ((long)gif->lzw->aBuffer[i + 1] << 8);
    else Result = (long)gif->lzw->aBuffer[i] | ((long)gif->lzw->aBuffer[i + 1] << 8) | ((long)gif->lzw->aBuffer[i + 2] << 16);

    Result = (Result >> (gif->lzw->CurBit & 0x7))&_aMaskTbl[gif->lzw->CodeSize];
    gif->lzw->CurBit += gif->lzw->CodeSize;
    return (int)Result;
}

/**
 * @brief       Get the next code for LZW
 * @param       filename : File path
 * @param       gif      : GIF information
 * @retval      Operation results
 *   @arg       >=0 : succeed
 *   @arg       -1  : failed
 *   @arg       -2  : Reading the ending symbol
 */
static int gif_getnextbyte(FIL *filename, gif89a *gif)
{
    int i, Code, Incode;

    while ((Code = gif_getnextcode(filename, gif)) >= 0)
    {
        if (Code == gif->lzw->ClearCode)
        {
            /* Corrupt GIFs can make this happen */
            if (gif->lzw->ClearCode >= (1 << MAX_NUM_LWZ_BITS))return -1; /* Error */

            /* Clear the tables */
            memset((uint8_t *)gif->lzw->aCode, 0, sizeof(gif->lzw->aCode));

            for (i = 0; i < gif->lzw->ClearCode; ++i)gif->lzw->aPrefix[i] = i;

            /* Calculate the'special codes' independence of the initial code size */
            /* and initialize the stack pointer */
            gif->lzw->CodeSize = gif->lzw->SetCodeSize + 1;
            gif->lzw->MaxCodeSize = gif->lzw->ClearCode << 1;
            gif->lzw->MaxCode = gif->lzw->ClearCode + 2;
            gif->lzw->sp = gif->lzw->aDecompBuffer;

            /* Read the first code from the stack after clear ingand initializing */
            do
            {
                gif->lzw->FirstCode = gif_getnextcode(filename, gif);
            } while (gif->lzw->FirstCode == gif->lzw->ClearCode);

            gif->lzw->OldCode = gif->lzw->FirstCode;
            return gif->lzw->FirstCode;
        }

        if (Code == gif->lzw->EndCode)return -2; /* End code */

        Incode = Code;

        if (Code >= gif->lzw->MaxCode)
        {
            *(gif->lzw->sp)++ = gif->lzw->FirstCode;
            Code = gif->lzw->OldCode;
        }

        while (Code >= gif->lzw->ClearCode)
        {
            *(gif->lzw->sp)++ = gif->lzw->aPrefix[Code];

            if (Code == gif->lzw->aCode[Code])return Code;

            if ((gif->lzw->sp - gif->lzw->aDecompBuffer) >= sizeof(gif->lzw->aDecompBuffer))return Code;

            Code = gif->lzw->aCode[Code];
        }

        *(gif->lzw->sp)++ = gif->lzw->FirstCode = gif->lzw->aPrefix[Code];

        if ((Code = gif->lzw->MaxCode) < (1 << MAX_NUM_LWZ_BITS))
        {
            gif->lzw->aCode[Code] = gif->lzw->OldCode;
            gif->lzw->aPrefix[Code] = gif->lzw->FirstCode;
            ++gif->lzw->MaxCode;

            if ((gif->lzw->MaxCode >= gif->lzw->MaxCodeSize) && (gif->lzw->MaxCodeSize < (1 << MAX_NUM_LWZ_BITS)))
            {
                gif->lzw->MaxCodeSize <<= 1;
                ++gif->lzw->CodeSize;
            }
        }

        gif->lzw->OldCode = Incode;

        if (gif->lzw->sp > gif->lzw->aDecompBuffer)return *--(gif->lzw->sp);
    }

    return Code;
}

/**
 * @brief       Display GIF image
 * @param       filename    : File path
 * @param       gif         : GIF information
 * @param       x0          : The x-coordinate of the string to be displayed
 * @param       y0          : The y-coordinate of the string to be displayed
 * @param       Transparency: Transparency index
 * @param       Disposal    : Processing method
 * @retval      Operation results
 *   @arg       0        : succeed
 *   @arg       Other    : failed
 */
static uint8_t gif_dispimage(FIL *filename, gif89a *gif, uint16_t x0, uint16_t y0, int Transparency, uint8_t Disposal)
{
    uint32_t readed;
    uint8_t lzwlen;
    int Index, OldIndex, XPos, YPos, YCnt, Pass, Interlace, XEnd;
    int Width, Height, Cnt, ColorIndex;
    uint16_t bkcolor;
    uint16_t *pTrans;

    Width = gif->gifISD.width;
    Height = gif->gifISD.height;
    XEnd = Width + x0 - 1;
    bkcolor = gif->colortbl[gif->gifLSD.bkcindex];
    pTrans = (uint16_t *)gif->colortbl;
    f_read(filename, &lzwlen, 1, (UINT *)&readed);
    gif_initlzw(gif, lzwlen);               /* Initialize the LZW stack with the LZW code size */
    Interlace = gif->gifISD.flag & 0x40;

    for (YCnt = 0, YPos = y0, Pass = 0; YCnt < Height; YCnt++)
    {
        Cnt = 0;
        OldIndex = -1;

        for (XPos = x0; XPos <= XEnd; XPos++)
        {
            if (gif->lzw->sp > gif->lzw->aDecompBuffer)
            {
                Index = *--(gif->lzw->sp);
            }
            else
            {
                Index = gif_getnextbyte(filename, gif);
            }
            
            if (Index == -2)return 0;   /* Endcode */

            if ((Index < 0) || (Index >= gif->numcolors))
            {
                /* IfIndex out of legal range stop decompressing */
                return 1;   /* Error */
            }

            /* If current index equals old index increment counter */
            if ((Index == OldIndex) && (XPos <= XEnd))
            {
                Cnt++;
            }
            else
            {
                if (Cnt)
                {
                    if (OldIndex != Transparency)
                    {
                        pic_phy.draw_hline(XPos - Cnt - 1, YPos, Cnt + 1, *(pTrans + OldIndex));
                    }
                    else if (Disposal == 2)
                    {
                        pic_phy.draw_hline(XPos - Cnt - 1, YPos, Cnt + 1, bkcolor);
                    }

                    Cnt = 0;
                }
                else
                {
                    if (OldIndex >= 0)
                    {
                        if (OldIndex != Transparency)
                        {
                            pic_phy.draw_point(XPos - 1, YPos, *(pTrans + OldIndex));
                        }
                        else if (Disposal == 2)
                        {
                            pic_phy.draw_point(XPos - 1, YPos, bkcolor);
                        }
                    }
                }
            }

            OldIndex = Index;
        }

        if ((OldIndex != Transparency) || (Disposal == 2))
        {
            if (OldIndex != Transparency)ColorIndex = *(pTrans + OldIndex);
            else ColorIndex = bkcolor;

            if (Cnt)
            {
                pic_phy.draw_hline(XPos - Cnt - 1, YPos, Cnt + 1, ColorIndex);
            }
            else
            {
                pic_phy.draw_point(XEnd, YPos, ColorIndex);
            }
        }

        /* Adjust YPos if image is interlaced */
        if (Interlace)
        {
            YPos += _aInterlaceOffset[Pass];

            if ((YPos - y0) >= Height)
            {
                ++Pass;
                YPos = _aInterlaceYPos[Pass] + y0;
            }
        }
        else
        {
            YPos++;
        }
    }

    return 0;
}

/**
 * @brief       Restore to background color
 * @param       x        : The x-coordinate of the string to be displayed
 * @param       y        : The y-coordinate of the string to be displayed
 * @param       gif      : GIF information
 * @param       pimge    : Image description block information
 * @retval      None
 */
static void gif_clear2bkcolor(uint16_t x, uint16_t y, gif89a *gif, ImageScreenDescriptor pimge)
{
    uint16_t x0, y0, x1, y1;
    uint16_t color = gif->colortbl[gif->gifLSD.bkcindex];

    if (pimge.width == 0 || pimge.height == 0)return;

    if (gif->gifISD.yoff > pimge.yoff)
    {
        x0 = x + pimge.xoff;
        y0 = y + pimge.yoff;
        x1 = x + pimge.xoff + pimge.width - 1;;
        y1 = y + gif->gifISD.yoff - 1;

        if (x0 < x1 && y0 < y1 && x1 < 320 && y1 < 320)
        {
            pic_phy.fill(x0, y0, x1, y1, color);
        }
    }

    if (gif->gifISD.xoff > pimge.xoff)
    {
        x0 = x + pimge.xoff;
        y0 = y + pimge.yoff;
        x1 = x + gif->gifISD.xoff - 1;;
        y1 = y + pimge.yoff + pimge.height - 1;

        if (x0 < x1 && y0 < y1 && x1 < 320 && y1 < 320)
        {
            pic_phy.fill(x0, y0, x1, y1, color);
        }
    }

    if ((gif->gifISD.yoff + gif->gifISD.height) < (pimge.yoff + pimge.height))
    {
        x0 = x + pimge.xoff;
        y0 = y + gif->gifISD.yoff + gif->gifISD.height - 1;
        x1 = x + pimge.xoff + pimge.width - 1;;
        y1 = y + pimge.yoff + pimge.height - 1;

        if (x0 < x1 && y0 < y1 && x1 < 320 && y1 < 320)
        {
            pic_phy.fill(x0, y0, x1, y1, color);
        }
    }

    if ((gif->gifISD.xoff + gif->gifISD.width) < (pimge.xoff + pimge.width))
    {
        x0 = x + gif->gifISD.xoff + gif->gifISD.width - 1;
        y0 = y + pimge.yoff;
        x1 = x + pimge.xoff + pimge.width - 1;;
        y1 = y + pimge.yoff + pimge.height - 1;

        if (x0 < x1 && y0 < y1 && x1 < 320 && y1 < 320)
        {
            pic_phy.fill(x0, y0, x1, y1, color);
        }
    }
}

/**
 * @brief       Draw a frame of GIF image
 * @param       filename : File path
 * @param       gif      : GIF information
 * @param       x0       : The x-coordinate of the string to be displayed
 * @param       y0       : The y-coordinate of the string to be displayed
 * @retval      Operation results
 *   @arg       0        : succeed
 *   @arg       Other    : failed
 */
static uint8_t gif_drawimage(FIL *filename, gif89a *gif, uint16_t x0, uint16_t y0)
{
    uint32_t readed;
    uint8_t res, temp;
    uint16_t numcolors;
    ImageScreenDescriptor previmg;

    uint8_t Disposal = 0;
    int TransIndex;
    uint8_t Introducer;
    TransIndex = -1;

    do
    {
        res = f_read(filename, &Introducer, 1, (UINT *)&readed);

        if (res)return 1;

        switch (Introducer)
        {
            case GIF_INTRO_IMAGE:
                previmg.xoff = gif->gifISD.xoff;
                previmg.yoff = gif->gifISD.yoff;
                previmg.width = gif->gifISD.width;
                previmg.height = gif->gifISD.height;

                res = f_read(filename, (uint8_t *)&gif->gifISD, 9, (UINT *)&readed);

                if (res)return 1;

                if (gif->gifISD.flag & 0x80)
                {
                    gif_savegctbl(gif);
                    numcolors = 2 << (gif->gifISD.flag & 0X07);

                    if (gif_readcolortbl(filename, gif, numcolors))return 1;
                }

                if (Disposal == 2)gif_clear2bkcolor(x0, y0, gif, previmg);

                gif_dispimage(filename, gif, x0 + gif->gifISD.xoff, y0 + gif->gifISD.yoff, TransIndex, Disposal);

                while (1)
                {
                    f_read(filename, &temp, 1, (UINT *)&readed);

                    if (temp == 0)break;

                    readed = f_tell(filename);

                    if (f_lseek(filename, readed + temp))break;
                }

                if (temp != 0)return 1; /* Error */

                return 0;

            case GIF_INTRO_TERMINATOR:
                return 2;

            case GIF_INTRO_EXTENSION:
                /* Read image extension*/
                res = gif_readextension(filename, gif, &TransIndex, &Disposal);

                if (res)return 1;

                break;

            default:
                return 1;
        }
    } while (Introducer != GIF_INTRO_TERMINATOR);

    return 0;
}

/**
 * @brief       Exit current decoding
 * @param       None
 * @retval      None
 */
void gif_quit(void)
{
    g_gif_decoding = 0;
}

/**
 * @brief       Decoding a GIF file
 * @param       filename : File path
 * @param       gif      : GIF information
 * @param       x0       : The x-coordinate of the string to be displayed
 * @param       y0       : The y-coordinate of the string to be displayed
 * @param       width    : Width
 * @param       height   : Height
 * @retval      Operation results
 *   @arg       0        : succeed
 *   @arg       Other    : failed
 */
uint8_t gif_decode(const char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint8_t res = 0;
    uint16_t dtime = 0;
    gif89a *mygif89a;
    FIL *gfile;
    
#if GIF_USE_MALLOC == 1
    gfile = (FIL *)piclib_mem_malloc(sizeof(FIL));

    if (gfile == NULL)res = PIC_MEM_ERR;

    mygif89a = (gif89a *)piclib_mem_malloc(sizeof(gif89a));

    if (mygif89a == NULL)res = PIC_MEM_ERR;

    mygif89a->lzw = (LZW_INFO *)piclib_mem_malloc(sizeof(LZW_INFO));

    if (mygif89a->lzw == NULL)res = PIC_MEM_ERR;

#else
    gfile = &f_gfile;
    mygif89a = &tgif89a;
    mygif89a->lzw = &tlzw;
#endif

    if (res == 0)       /* OK */
    {
        res = f_open(gfile, (TCHAR *)filename, FA_READ);

        if (res == 0)
        {
            if (gif_check_head(gfile))res = PIC_FORMAT_ERR;

            if (gif_getinfo(gfile, mygif89a))res = PIC_FORMAT_ERR;

            if (mygif89a->gifLSD.width > width || mygif89a->gifLSD.height > height)res = PIC_SIZE_ERR;
            else
            {
                x = (width - mygif89a->gifLSD.width) / 2 + x;
                y = (height - mygif89a->gifLSD.height) / 2 + y;
            }

            g_gif_decoding = 1;

            while (g_gif_decoding && res == 0)
            {
                res = gif_drawimage(gfile, mygif89a, x, y);

                if (mygif89a->gifISD.flag & 0x80)gif_recovergctbl(mygif89a);

                if (mygif89a->delay)
                {
                    dtime = mygif89a->delay;
                }
                else 
                {
                    dtime = 10;
                }
                
                while (dtime-- && g_gif_decoding)
                {
                    vTaskDelay(10);
                }
                
                if (res == 2)
                {
                    res = 0;
                    break;
                }
            }
        }

        f_close(gfile);
    }

#if GIF_USE_MALLOC == 1
    piclib_mem_free(gfile);
    piclib_mem_free(mygif89a->lzw);
    piclib_mem_free(mygif89a);
#endif
    return res;
}

/**
 ****************************************************************************************************
 * @file        mjpeg.c
 * @author      ALIENTEK
 * @brief       MJPEG code
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

#include "mjpeg.h"


struct jpeg_decompress_struct *cinfo;
struct my_error_mgr *jerr;
int Windows_Width = 0;
int Windows_Height = 0;
uint16_t imgoffx, imgoffy;                  /* The offset of the image in the x and y directions */
typedef struct my_error_mgr* my_error_ptr;

/**
 * @brief       Error Exiting
 * @param       cinfo   : JPEG encoding and decoding control structure
 * @retval      None
 */
METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

/**
 * @brief       Send a message
 * @param       cinfo       : JPEG encoding and decoding control structure
 * @param       msg_level   : Message level
 * @retval      None
 */
METHODDEF(void) my_emit_message(j_common_ptr cinfo, int msg_level)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    if (msg_level < 0)
    {
        printf("emit msg:%d\r\n", msg_level);
        longjmp(myerr->setjmp_buffer, 1);
    }
}

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;

/**
 * @brief       Decoding JPEG images
 * @param       buf: Jpeg data stream array
 * @param       bsize: Array size
 * @retval      0:succeed; !0:failed
 */
uint8_t mjpegdec_decode(uint8_t* buf, uint32_t bsize)
{

    JSAMPARRAY buffer;
    if (bsize == 0) return 1;
    int row_stride = 0;
    int j = 0;
    int lineR = 0;
    cinfo->err = jpeg_std_error(&jerr->pub);
    jerr->pub.error_exit = my_error_exit;
    jerr->pub.emit_message = my_emit_message;
    cinfo->out_color_space = JCS_RGB;

    if (setjmp(jerr->setjmp_buffer))
    {
        jpeg_abort_decompress(cinfo);
        jpeg_destroy_decompress(cinfo);
        return 2;
    }

    jpeg_create_decompress(cinfo);

    jpeg_mem_src(cinfo, buf, bsize);
    jpeg_read_header(cinfo, TRUE);

    jpeg_start_decompress(cinfo); 

    row_stride = cinfo->output_width * cinfo->output_components;

    buffer = (*cinfo->mem->alloc_sarray)
        ((j_common_ptr)cinfo, JPOOL_IMAGE, row_stride, 1);
    
    while (cinfo->output_scanline < cinfo->output_height)
    {
        int i = 0;

        jpeg_read_scanlines(cinfo, buffer, 1);
        unsigned short tmp_color565;

        for (int k = 0; k < Windows_Width * 2; k += 2)
        {
            tmp_color565 = rgb565(buffer[0][i],buffer[0][i + 1],buffer[0][i + 2]);
            lcd_buf[lineR + k] = (tmp_color565 & 0xFF00) >> 8;
            lcd_buf[lineR + k + 1] =  tmp_color565 & 0x00FF;

            i += 3;
        }
        
        j++;
        lineR = j * Windows_Width * 2;
    }

    lcd_set_window(imgoffx, imgoffy - 30, imgoffx + cinfo->output_width - 1, imgoffy - 30 + cinfo->output_height - 1);
    taskENTER_CRITICAL(&my_spinlock);

    for(int x = 0; x < (cinfo->output_width * cinfo->output_height * 2 / LCD_BUF_SIZE); x++)
    {

        lcd_write_data(&lcd_buf[x * LCD_BUF_SIZE] , LCD_BUF_SIZE);
    }
    
    taskEXIT_CRITICAL(&my_spinlock);
    lcd_set_window(0, 0, lcd_self.width, lcd_self.height);
    jpeg_finish_decompress(cinfo);
    jpeg_destroy_decompress(cinfo);
    return 0;
}

/**
 * @brief       mjpegdec init
 * @param       offx,offy:deviation
 * @retval      0:succeed; !0:failed
 */
char mjpegdec_init(uint16_t offx, uint16_t offy)
{
    cinfo = (struct jpeg_decompress_struct *)malloc(sizeof(struct jpeg_decompress_struct));
    jerr = (struct my_error_mgr *)malloc(sizeof(struct my_error_mgr));

    if (cinfo == NULL || jerr == NULL)
    {
        printf("[E][mjpeg.cpp] mjpegdec_init(): malloc failed to apply for memory\r\n");
        mjpegdec_free();
        return -1;
    }

    imgoffx = offx;
    imgoffy = offy;

    return 0;
}

/**
 * @brief       Mjpeg decoding completed, freeing memory
 * @param       None
 * @retval      None
 */
void mjpegdec_free(void)
{
    free(cinfo);
    free(jerr);
}

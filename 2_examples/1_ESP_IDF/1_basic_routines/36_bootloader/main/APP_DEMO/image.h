/**
 ****************************************************************************************************
 * @file        image.h
 * @author      ALIENTEK
 * @brief       IMAGE code
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

#ifndef __IMAGE_H
#define __IMAGE_H

#include "esp_partition.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"

/* font library storage first address */
extern uint32_t IMAGEINFOADDR;


typedef struct
{
    uint8_t imageok;
    
    uint32_t lvgl_camera_addr;
    uint32_t lvgl_camera_size;
  
    uint32_t lvgl_file_addr;
    uint32_t lvgl_file_size;
    
    uint32_t lvgl_video_addr;
    uint32_t lvgl_video_size;
    
    uint32_t lvgl_setting_addr;
    uint32_t lvgl_setting_size;
    
    uint32_t lvgl_weather_addr;
    uint32_t lvgl_weather_size;
    
    uint32_t lvgl_measure_addr;
    uint32_t lvgl_measure_size;

    uint32_t lvgl_photo_addr;
    uint32_t lvgl_photo_size;
    
    uint32_t lvgl_music_addr;
    uint32_t lvgl_music_size;

    uint32_t lvgl_calendar_addr;
    uint32_t lvgl_calendar_size;

    uint32_t lvgl_background_addr;
    uint32_t lvgl_background_size;
} _image_info;

extern _image_info g_ftinfo;

/* function declaration */
uint8_t images_init(void);
uint8_t images_update_image(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color);
esp_err_t images_partition_write(void *buffer, uint32_t offset, uint32_t length);
esp_err_t images_partition_read(void *buffer, uint32_t offset, uint32_t length);
esp_err_t images_partition_erase_sector(uint32_t offset);

#endif

/**
 ****************************************************************************************************
 * @file        mian.c
 * @author      ALIENTEK
 * @brief       SPIFFS example
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "led.h"
#include "iic.h"
#include "spi.h"
#include "lcd.h"
#include "xl9555.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "nvs_flash.h"


i2c_obj_t i2c0_master;

#define DEFAULT_FD_NUM          5
#define DEFAULT_MOUNT_POINT     "/spiffs"
#define WRITE_DATA              "ALIENTEK ESP32-S3\r\n"
static const char               *TAG = "spiffs";

/**
 * @brief       Initialize spiffs
 * @param       partition_label : Same label as passed to esp_vfs_spiffs_register
 * @param       mount_point     : File path prefix associated with the filesystem
 * @param       max_files       : Maximum files that could be open at the same time
 * @retval      None
 */
esp_err_t spiffs_init(char *partition_label, char *mount_point, size_t max_files)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = mount_point,           /* File path prefix associated with the filesystem */
        .partition_label = partition_label, /* Optional, label of SPIFFS partition to use. If set to NULL, first partition with subtype=spiffs will be used */
        .max_files = max_files,             /* Maximum files that could be open at the same time */
        .format_if_mount_failed = true,     /* If true, it will format the file system if it fails to mount */
    };

    /* Register and mount SPIFFS to VFS with given path prefix */
    esp_err_t ret_val = esp_vfs_spiffs_register(&conf);

    if (ret_val != ESP_OK)
    {
        if (ret_val == ESP_FAIL)
        {
            printf("Failed to mount or format filesystem\n");
        }
        else if (ret_val == ESP_ERR_NOT_FOUND)
        {
            printf("Failed to find SPIFFS partition\n");
        }
        else
        {
            printf("Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret_val));
        }

        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret_val = esp_spiffs_info(conf.partition_label, &total, &used);

    if (ret_val != ESP_OK)
    {
        ESP_LOGI(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret_val));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ret_val;
}

/**
 * @brief       Unregister and unmount SPIFFS from VFS
 * @param       partition_label:Same label as passed to esp_vfs_spiffs_register
 * @retval      None
 */
esp_err_t spiffs_deinit(char *partition_label)
{
    return esp_vfs_spiffs_unregister(partition_label);
}

/**
 * @brief       spiffs Test
 * @param       None
 * @retval      None
 */
void spiffs_test(void)
{

    ESP_LOGI(TAG, "Opening file");
    FILE* f = fopen("/spiffs/hello.txt", "w");

    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    fprintf(f, WRITE_DATA);

    fclose(f);
    ESP_LOGI(TAG, "File written");
 
    struct stat st;

    if (stat("/spiffs/foo.txt", &st) == 0)
    {
        unlink("/spiffs/foo.txt");
    }
 
    ESP_LOGI(TAG, "Renaming file");

    if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0)
    {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }
 
    ESP_LOGI(TAG, "Reading file");
    f = fopen("/spiffs/foo.txt", "r");

    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    
    char* pos = strchr(line, '\n');

    if (pos)
    {
        *pos = '\0';
    }

    ESP_LOGI(TAG, "Read from file: '%s'", line);

    lcd_show_string(90, 110, 200, 16, 16, line, RED);
}

/**
 * @brief       The application entry point
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;

    ret = nvs_flash_init();                                         /* Initialize NVS */
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    led_init();                                                     /* Initialize LED */
    i2c0_master = iic_init(I2C_NUM_0);                              /* Initialize IIC0 */
    spi2_init();                                                    /* Initialize SPI2 */
    xl9555_init(i2c0_master);                                       /* Initialize XL9555 */
    lcd_init();                                                     /* Initialize LCD */
    spiffs_init("storage", DEFAULT_MOUNT_POINT, DEFAULT_FD_NUM);    /* Initialize SPIFFS */

    lcd_show_string(10, 50, 200, 16, 16, "ESP32-S3", RED);
    lcd_show_string(10, 70, 200, 16, 16, "SPIFFS TEST", RED);
    lcd_show_string(10, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(10, 110, 200, 16, 16, "Read file:", BLUE);

    spiffs_test();                                                  /* SPIFFS Test */

    while (1)
    {
        LED_TOGGLE();
        vTaskDelay(500);
    }
}

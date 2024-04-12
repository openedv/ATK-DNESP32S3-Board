/**
 ****************************************************************************************************
 * @file        camera.c
 * @author      ALIENTEK
 * @brief       CAMERA code
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

#include "camera.h"


/* configuration structure for camera initialization */
static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,
    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    .xclk_freq_hz = 24000000,               /* EXPERIMENTAL: Set to 16MHz on ESP32-S2 or ESP32-S3 to enable EDMA mode */
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .fb_location = CAMERA_FB_IN_PSRAM,
    .pixel_format = PIXFORMAT_JPEG,         /* YUV422,GRAYSCALE,RGB565,JPEG */
    /* QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S 
       series has improved a lot, but JPEG mode always gives better frame rates */
    .frame_size = FRAMESIZE_QVGA,
    .jpeg_quality = 12,                     /* 0-63, for OV series camera sensors, lower */
    /* When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode */
    .fb_count = 2,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,    /* CAMERA_GRAB_LATEST. Sets when buffers should be filled */
};

/**
 * @brief       initialize the camera
 * @param       None
 * @retval      None
 */
uint8_t camera_init(void)
{
    esp_err_t err = ESP_OK;

    /* power up the camera if PWDN pin is defined */
    if (CAM_PIN_PWDN == GPIO_NUM_NC)
    {
        CAM_PWDN(0);
    } 

    if (CAM_PIN_RESET == GPIO_NUM_NC)
    { 
        CAM_RST(0);
        vTaskDelay(20);
        CAM_RST(1);
        vTaskDelay(20);
    }

    /* initialize the camera */
    err = esp_camera_init(&camera_config);

    if (err != ESP_OK)
    {
        return 1;
    }

    sensor_t * s = esp_camera_sensor_get();

    if (s->id.PID == OV3660_PID)
    {
        s->set_vflip(s, 1);
        s->set_brightness(s, 1);
        s->set_saturation(s, -2);
    }
    else if (s->id.PID == OV5640_PID)
    {
        s->set_vflip(s, 1);
    }

    return err;
}

unsigned long i = 0;
unsigned long j = 0;
camera_fb_t *fb = NULL;

/**
 * @brief       Display camera data (RGB565)
 * @param       x : The x-coordinate of the string to be displayed
 * @param       y : The y-coordinate of the string to be displayed
 * @retval      None
 */
void camera_show(uint16_t x, uint16_t y)
{
    fb = esp_camera_fb_get();

    if (x + fb->width > lcd_self.width || y + fb->height > lcd_self.height)
    {
        goto err;
    }

    lcd_set_window(x, y, x + fb->width - 1, y + fb->height - 1);

    for (j = 0; j < fb->width * fb->height; j++)
    {
        lcd_buf[2 * j] = (fb->buf[2 * i]) ;
        lcd_buf[2 * j + 1] =  (fb->buf[2 * i + 1]);
        i ++;
    }
    
    for(j = 0; j < (fb->width * fb->height * 2 / LCD_BUF_SIZE); j++)
    {
        lcd_write_data(&lcd_buf[j * LCD_BUF_SIZE] , LCD_BUF_SIZE);
    }
err:
    esp_camera_fb_return(fb);
    i = 0;
    fb = NULL;
}

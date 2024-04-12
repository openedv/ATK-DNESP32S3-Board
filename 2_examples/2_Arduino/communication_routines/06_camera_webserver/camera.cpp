/**
 ****************************************************************************************************
 * @file        camera.cpp
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
#include "esp_camera.h"
#include "spilcd.h"
#include "xl9555.h"

camera_fb_t *fb = NULL;   /* define a pointer to the camera fb t variable */

/**
 * @brief       camera initialization(OV5640 / OV2640) 
 * @param       None
 * @retval      0:succeed / 1:failed 
 */
uint8_t camera_init(void)
{
    camera_config_t camera_config;

    camera_config.ledc_channel = LEDC_CHANNEL_0;  
    camera_config.ledc_timer = LEDC_TIMER_0;      
    camera_config.xclk_freq_hz = 20000000;        

    camera_config.pin_d7 = OV_D7_PIN;             
    camera_config.pin_d6 = OV_D6_PIN;             
    camera_config.pin_d5 = OV_D5_PIN;            
    camera_config.pin_d4 = OV_D4_PIN;            
    camera_config.pin_d3 = OV_D3_PIN;           
    camera_config.pin_d2 = OV_D2_PIN;          
    camera_config.pin_d1 = OV_D1_PIN;          
    camera_config.pin_d0 = OV_D0_PIN;             

    camera_config.pin_xclk  = OV_XCLK_PIN;        
    camera_config.pin_pclk  = OV_PCLK_PIN;       
    camera_config.pin_vsync = OV_VSYNC_PIN;      
    camera_config.pin_href  = OV_HREF_PIN;      

    camera_config.pin_sscb_sda = OV_SDA_PIN;     
    camera_config.pin_sscb_scl = OV_SCL_PIN;    

    camera_config.pin_pwdn  = OV_PWDN_PIN;      
    camera_config.pin_reset = OV_RESET_PIN;    
    
    camera_config.frame_size   = FRAMESIZE_QVGA;  
    camera_config.pixel_format = PIXFORMAT_RGB565;
    camera_config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;  
    camera_config.fb_location  = CAMERA_FB_IN_PSRAM;     

    camera_config.jpeg_quality = 12;             
    camera_config.fb_count     = 1;        

    if (spilcd_dir == 0)        /* in portrait, only 240*240 resolution images can be displayed */
    {
        camera_config.frame_size   = FRAMESIZE_240X240; 
    }

    if (OV_PWDN_PIN == -1)      /* Camera power */  
    {
        xl9555_io_config(OV_PWDN, IO_SET_OUTPUT);   /* PWDN pin uses XL9555_IO */
        xl9555_pin_set(OV_PWDN, IO_SET_LOW);        /* power on */
    } 

    if (OV_RESET_PIN == -1)     /* hardware fault */
    { 
        xl9555_io_config(OV_RESET, IO_SET_OUTPUT);  /* RESET pin uses XL9555_IO */
        xl9555_pin_set(OV_RESET, IO_SET_LOW);
        delay(20);
        xl9555_pin_set(OV_RESET, IO_SET_HIGH);
        delay(20);
    } 

    esp_err_t err =  esp_camera_init(&camera_config);   /* camera initialization */
    if (err != ESP_OK) 
    {
        Serial.printf("camera init failed,err:0x%x", err);
        return 1;
    }

    sensor_t * s = esp_camera_sensor_get();             /* get camera information */
    Serial.printf("camera ID:%#x  \r\n", s->id.PID);
    
    s->set_brightness(s, 0);           
    s->set_contrast(s, 0);        
    s->set_saturation(s, 0);      
    s->set_hmirror(s, 0);       
    s->set_vflip(s, 1);         

    if (s->id.PID == OV2640_PID)
    {
        s->set_vflip(s, 0);      
    }

    return 0;
}

/**
 * @brief       capture the camera image and display
 * @param       None
 * @retval      0:succeed / 1:failed 
 */
uint8_t camera_capture_show(void)
{
  fb = esp_camera_fb_get();                     /* capture a frame */
  if (!fb) 
  {
      Serial.printf("failed to capture the camera image  \r\n");
      return 1;
  }

  if (spilcd_dir == 1)                          /* In landscape, lcd display 320*240 resolution image */
  {
      lcd_show_pic(0, 0, 320, 240, fb->buf);    
  }
  else                                          /* In portrait, lcd display 240*240 resolution image */
  {
      lcd_show_pic(0, 39, 240, 240, fb->buf);   
  }

  esp_camera_fb_return(fb);                     /* return the frame buffer back to be reused */
  fb = NULL;

  return 0;
}



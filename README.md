# 1 Framework

The file framework of the data disk is as follows:

| Root  directory | Description                                                  |
| --------------- | ------------------------------------------------------------ |
| 1_docs          | To store hardware-related documents and images.              |
| 2_examples      | To store three types code：IDF,Arduino and micropython.      |
| 3_firmware      | To store firmware.                                           |
| 4_tools         | To store development tools such as flash_download_tool, CH340_Drivers, and serial port terminal, etc. |
| README.md       | To introduce ALIENTEK ESP32-S3 Development Board.            |

# 2 DNESP32S3 Board Introduction

## 2.1 Background

With the rapid development of the Internet of Things (IoT), embedded system development has become a bridge connecting the physical world and the digital world. In this era of change, Espressif's ESP series microcontrollers, with their low power consumption, high performance, and wide range of applications, have become star products in the field of the Internet of Things. To assist developers in mastering the development of ESP series chips, we provide examples and tutorials in ESP_IDF, Arduino, and MicroPython versions, allowing them to choose a suitable development environment based on their preferences and project requirements.

## 2.1 Brief Introduction

The DNESP32S3 development board is designed by China's leader in embedded development platforms, [**Guangzhou YuanziDianzi Electronic Technology Co., Ltd**](https://www.alientek.com/).This board offers exceptional value for money, supports 2.4 GHz Wi-Fi, and low-power Bluetooth wireless communication technology. What's more, it is equipped with a Neural Network Processing Unit (KPU), providing you with a perfect combination of high performance, security, and cost-effectiveness.

## 2.3 Board Resources

### 2.3.1 Hardware

![](./1_docs/3_figures/image/1_01_DNESP32S3_Board.png)

**1 Module Resources**

The module on the DNESP32S3 board utilizes the **ESP32-S3R8** chip, which comes with 16MB of external Flash. When using the OPI PSRAM, it should be noted that GPIO35-GPIO37 on the DNESP32S3 board will not be available for other purposes. However, when OPI PSRAM is not used, GPIO35-GPIO37 on the board can be used as normal GPIO.

For more information, please visit: https://www.espressif.com.cn/sites/default/files/documentation/esp32s3-wroom-1_wroom-1u_datasheet_en.pdf.

**2 Onboard Resources**

The onboard resources of the development board are shown in the table below:

|          Resources           | NUM   | Describe                                                     |
| :--------------------------: | ----- | ------------------------------------------------------------ |
|            Module            | 1     | ATK-MWS3S, ROM:384KB, SRAM:512KB,  FLASH:16MB,PSRAM:8MB;     |
|            EEPROM            | 1     | 2Kb（256 Bytes）                                             |
|         Power light          | 1     | BLUE                                                         |
|          User light          | 1     | LED                                                          |
|         Reset button         | 1     | Module and LCD Reset                                         |
|       Function buttons       | 5     | KEY0,KEY1,KEY2,KEY3,BOOT                                     |
|         Power switch         | 1     | Control the entire board power supply                        |
|             BEEP             | 1     | Prompt sound                                                 |
|      Infrared reception      | 1     | Used for infrared reception, equipped with an infrared remote control |
|    Infrared transmission     | 1     | Used for infrared transmission                               |
|           AP3216C            | 1     | Used for measuring light intensity, proximity distance, and infrared light intensity |
|            ES8388            | 1     | Used for audio encoding and decoding                         |
|             MIC              | 1     | Used for recording                                           |
|           Speaker            | 1     | Used to play sound                                           |
|        headphone jack        | 1     | Used to external headphones                                  |
|            SPILCD            | 1     | ALIENTEK 1.3/2.4 inch SPILCD                                 |
|        DS18B20/DHT11         | 1     | Supports digital temperature and humidity sensors such as DS18B20 and DHT11 |
|          ATK MODULE          | 1     | Supports serial port modules                                 |
|       Camera interface       | 1     | ALIENTEK  2640/5640 Camera  Module                           |
|          USB serial          | 1     | Used for debugging and downloading code                      |
|  USB slave interface (JTAG)  | 1     | Used for USB SLAVE (slave) communication, debugging, and downloading |
|      TF card interface       | 1     | TF card                                                      |
|  Multifunctional interface   | 1 set | Used for interconnections such as ADC/RMT/RV1                |
|  5V power input/output port  | 1 set | Used for 5V power supply connection/external supply of 5V voltage |
| 3.3V power input/output port | 1 set | Used for 3.3V power supply connection/external supply of 3.3V voltage |
|           QMA6100P           | 1     | Used to measure X, Y, Z coordinates                          |
|          Export IO           | 28    | For extended use                                             |
|            RGBLCD            | 1     | LCD screen for RGB interface (RGB565 format)                 |

### 2.3.2 Software

**1 ESP-IDF Examples**

| Num                   | Example Name                         | Num  | Example Name                                     |
| --------------------- | ------------------------------------ | ---- | ------------------------------------------------ |
| **Basic Examples**    |                                      |      |                                                  |
| 00                    | 00_basic                             | 19   | 19_ds18b20                                       |
| 01                    | 01_led                               | 20   | 20_dht11                                         |
| 02                    | 02_key                               | 21   | 21_rng                                           |
| 03                    | 03_exit                              | 22   | 22_qma6100p                                      |
| 04                    | 04_uart                              | 23   | 23_rgb                                           |
| 05                    | 05_esp_timer                         | 24   | 24_touch                                         |
| 06                    | 06_gp_timer                          | 25-1 | 25_1_camera                                      |
| 07                    | 07_wdt                               | 25-2 | 25_2_camera_photograph                           |
| 08-1                  | 08-1_sw_pwm                          | 26   | 26_sd                                            |
| 08-2                  | 08-2_hw_pwm                          | 27   | 27_spiffs                                        |
| 09                    | 09_iic_exio                          | 28   | 28_chinese_display                               |
| 10                    | 10_iic_eeprom                        | 29   | 29_pictures                                      |
| 11                    | 11_oled                              | 30   | 30_music                                         |
| 12                    | 12_spilcd                            | 31   | 31_recoding                                      |
| 13                    | 13_rtc                               | 32   | 32_videoplayer                                   |
| 14                    | 14_adc                               | 33   | 33_usb_uart                                      |
| 15                    | 15_ap3216c                           | 34   | 34_usb_flash_u                                   |
| 16                    | 16_infrared_reception                | 35   | 35_usb_sd_u                                      |
| 17                    | 17_infrared_transmission             | 36   | 36_bootloader                                    |
| 18                    | 18_internal_Temperature              |      |                                                  |
| **Advanced Examples** |                                      |      |                                                  |
| **FreeRTOS Examples** |                                      |      |                                                  |
| 01                    | 01_Trends_task_creation_and_deletion | 10   | 10_Counting_type_semaphore_operation             |
| 02                    | 02_Static_task_creation_and_deletion | 11   | 11_Priority_inversion                            |
| 03                    | 03_Task_suspension_and_recovery      | 12   | 12_Mutex_semaphore                               |
| 04                    | 04_Insertion_and_deletion_list_items | 13   | 13_Software_Timers                               |
| 05                    | 05_Time_slice_scheduling             | 14   | 14_Event_flag                                    |
| 06                    | 06_Queue_operation                   | 15   | 15_Task_notification_simulation_binary_semaphore |
| 07                    | 07_Queue_Set                         | 16   | 16_Task_notification_analog_count_semaphore      |
| 08                    | 08_Queue_Set_Simulation_Event_Flags  | 17   | 17_Task_notification_simulation_message_email    |
| 09                    | 09_Binary_semaphore_operation        | 18   | 18_Task_notification_simulation_event_flag_group |
| **WiFi Examples**     |                                      |      |                                                  |
| 01                    | 01_WiFi_SCAN                         | 05   | 05_WiFi_UDP                                      |
| 02                    | 02_WiFi_STA                          | 06   | 06_WiFi_TCPClient                                |
| 03                    | 03_WiFi_AP                           | 07   | 07_WiFi_TCPServer                                |
| 04                    | 04_WiFi_SmartConfig                  | 08   | 09_WiFi_CAMERA                                   |
| **AI Examples**       |                                      |      |                                                  |
| 01                    | 01_Face_detection_project            | 04   | 04_qr_detection_project                          |
| 02                    | 02_Face_recognition_project          | 05   | 05_color_detection_project                       |
| 03                    | 03_catface_detection_project         | 06   | 06_motion_detection_project                      |
| **LVGL Examples**     |                                      |      |                                                  |
| 01                    | 01_lvgl_transplant                   | 29   | 29_lvgl_led                                      |
| 02                    | 02_PC_lvgl                           | 30   | 30_lvgl_list                                     |
| 03                    | 03_lvgl_add_mouse                    | 31   | 31_lvgl_meter                                    |
| 04                    | 04_lvgl_fs_use                       | 32   | 32_lvgl_msgbox                                   |
| 05                    | 05_lvgl_font                         | 33   | 33_lvgl_span                                     |
| 06                    | 06_lvgl_spiflash_font                | 34   | 34_lvgl_spinbox                                  |
| 07                    | 07_lvgl_ttf_font                     | 35   | 35_lvgl_spinner                                  |
| 08                    | 08_lvgl_obj                          | 36   | 36_lvgl_tabview                                  |
| 09                    | 09_lvgl_arc                          | 37   | 37_lvgl_titleview                                |
| 10                    | 10_lvgl_bar                          | 38   | 38_lvgl_win                                      |
| 11                    | 11_lvgl_btn                          | 39   | 39_lvgl_animimg                                  |
| 12                    | 12_lvgl_btnmatrix                    | 40   | 40_lvgl_menu                                     |
| 13                    | 13_lvgl_canvas                       | 41   | 41_lvgl_spiflash_pic                             |
| 14                    | 14_lvgl_checkbox                     | 42   | 42_lvgl_show_bmp                                 |
| 15                    | 15_lvgl_dropdown                     | 43   | 43_lvgl_show_png                                 |
| 16                    | 16_lvgl_img                          | 44   | 44_lvgl_show_gif                                 |
| 17                    | 17_lvgl_label                        | 45   | 45_lvgl_show_qrcode                              |
| 18                    | 18_lvgl_line                         | 46   | 46_lvgl_show_jpeg                                |
| 19                    | 19_lvgl_roller                       | 47   | 47_lvgl_flex                                     |
| 20                    | 20_lvgl_slider                       | 48   | 48_lvgl_grid                                     |
| 21                    | 21_lvgl_switch                       | 49   | 49_lvgl_radar_demo                               |
| 22                    | 22_lvgl_table                        | 50   | 50_lvgl_calculator                               |
| 23                    | 23_lvgl_textarea                     | 51   | 51_lvgl_smart_home                               |
| 24                    | 24_lvgl_calendar                     | 52   | 52_lvgl_generator_qrcode                         |
| 25                    | 25_lvgl_chart                        | 53   | 53_lvgl_paint                                    |
| 26                    | 26_lvgl_colorwheel                   | 54   | 54_lvgl_file_system                              |
| 27                    | 27_lvgl_imgbtn                       | 55   | 55_lvgl_conversion_of_number                     |
| 28                    | 28_lvgl_keyboard                     |      |                                                  |

**2 Arduino Examples**

| Num                   | Example Name      | Num  | Example Name             |
| --------------------- | ----------------- | ---- | ------------------------ |
| **Basic Examples**    |                   |      |                          |
| 01                    | 01_led            | 12   | 12_rtc                   |
| 02                    | 02_key            | 13   | 13_adc                   |
| 03                    | 03_exti           | 14   | 14_iic_ap3216c           |
| 04                    | 04_uart           | 15   | 15_infrared_reception    |
| 05                    | 05_timer_it       | 16   | 16_infrared_transmission |
| 06                    | 06_watch_dog      | 17   | 17_ds18b20               |
| 07                    | 07_led_pwm        | 18   | 18_dht11                 |
| 08                    | 08_iic_eeprom     | 19   | 19_iic_qma6100p          |
| 09                    | 09_iic_exio       | 20   | 20_camera                |
| 10                    | 10_iic_oled       | 21   | 21_spi_sdcard            |
| 11                    | 11_spi_lcd        |      |                          |
| **Advanced Examples** |                   |      |                          |
| **WiFi&BLE Examples** |                   |      |                          |
| 01                    | 01_wifi_scan      | 04   | 04_ble_scan              |
| 02                    | 02_wifi_webserver | 05   | 05_ble_uart              |
| 03                    | 03_wifi_client    | 06   | 06_camera_webserver      |

**3 MicroPython Examples**

| Num                   | Example Name         | Num  | Example Name             |
| --------------------- | -------------------- | ---- | ------------------------ |
| **Basic Examples**    |                      |      |                          |
| 00                    | 00_basic             | 12   | 12_adc                   |
| 01                    | 01_led               | 13   | 13_ap3216c               |
| 02                    | 02_key               | 14   | 14_Infrared_reception    |
| 03                    | 03_exti              | 15   | 15_Infrared_transmission |
| 04                    | 04_timer_it          | 16   | 16_Internal_temperature  |
| 05                    | 05_wdt               | 17   | 17_dht11                 |
| 06                    | 06_led_pwm           | 18   | 18_ds18b20               |
| 07                    | 07_iic_exio          | 19   | 19_camera                |
| 08                    | 08_oled              | 20   | 20_qma6100p              |
| 09                    | 09_spilcd            | 21   | 21_sd                    |
| 10                    | 10_iic_eeprom        | 22   | 22_rgb                   |
| 11                    | 11_rtc               | 23   | 23_touch                 |
| **Advanced Examples** |                      |      |                          |
| **WiFi&BLE Examples** |                      |      |                          |
| 01                    | 01_UDP               | 04   | 04_WebCAM                |
| 02                    | 02_TCPClient         | 05   | 05_BLE_project           |
| 03                    | 03_TCPServer         |      |                          |
| **AI Examples**       |                      |      |                          |
| 01                    | 01_face_detection    | 04   | 04_qr_detection          |
| 02                    | 02_face_recongnition | 05   | 05_color_detection       |
| 03                    | 03_catface_detection |      |                          |

# 3 Preparation

## 3.1 Hardware

- An **DNESP32S3** board
- **USB cable** - USB A / type USB C
- **Computer** running Windows

## 3.2 CH340 Drivers installation

DNESP32S3 board uses CH340C to download code. Therefore, before using the device, it is necessary to install the CH340C driver on your computer.

[How to Install CH340 Drivers](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers)

# 4 Environment Setup

- [Developing With ESP-IDF](./1_docs/Developing_With_ESP_IDF.md)
- [Developing With Arduino](./1_docs/Developing_With_Arduino.md) 

- [Developing With MicroPython](./1_docs/Developing_With_MicroPython.md)

# 5 Accessories

When using the DNESP32S3 Board, you can use the following accessories.

| 1.3-inch LCD module                                          | 2.4-inch LCD module                                          | 4.3-inch RGBLCD module                                       |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| <a href="https://github.com/alientek-module/LCD-module_1.3-inch_ATK-MD0130"><img src="1_docs/3_figures/image/1_77_ATK_MD0130_Module.png" style="zoom: 50%;" /></a> | <a href="https://github.com/alientek-module/LCD-module_2.4-inch_ATK-MD0240"><img src="1_docs/3_figures/image/1_78_ATK_MD0240_Module.png" style="zoom:18%;" /></a> | <a href="https://github.com/alientek-module/LCD-module_4.3-inch_ATK-MD0430R"><img src="1_docs/3_figures/image/1_81_ATK_MD0430R_Module.png" style="zoom:10%;" /></a> |

| OV5640 CAMERA module                                         | OV2640 CAMERA module                                         | OLED module                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| <a href="https://github.com/alientek-module/CAMERA-module_ATK-MC5640"><img src="1_docs/3_figures/image/1_80_ATK_MC5640_Module.png" style="zoom:25%;" /></a> | <a href="https://github.com/alientek-module/CAMERA-module_ATK-MC2640"><img src="1_docs/3_figures/image/1_79_ATK_MC2640_Module.png" style="zoom:25%;" /></a> | <a href="https://github.com/alientek-module/OLED-module_ATK-OLED"><img src="1_docs/3_figures/image/1_82_ATK_OLED_Module.png" style="zoom:25%;" /></a> |




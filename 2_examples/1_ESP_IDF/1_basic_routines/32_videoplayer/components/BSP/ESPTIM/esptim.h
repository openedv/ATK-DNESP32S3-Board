/**
 ****************************************************************************************************
 * @file        esptim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       高分辨率定时器（ESP定时器）驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#ifndef __ESPTIM_H_
#define __ESPTIM_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "soc/timer_group_struct.h"
#include "driver/timer.h"
#include "esp_timer.h"
#include "led.h"


extern esp_timer_handle_t esp_tim_handle;                          /* 定时器回调函数句柄 */
/* 函数声明 */
void esptim_int_init(uint16_t arr, uint64_t tp);    /* 初始化初始化高分辨率定时器 */
void TIM_PeriodElapsedCallback(void *arg);          /* 定时器回调函数 */

#endif
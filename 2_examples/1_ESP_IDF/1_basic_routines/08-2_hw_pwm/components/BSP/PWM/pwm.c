/**
 ****************************************************************************************************
 * @file        pwm.c
 * @author      ALIENTEK
 * @brief       PWM code
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

#include "pwm.h"


/**
 * @brief       configure pwm
 * @param       resolution: 1-16 bits
 * @param       freq: freq limits depend on resolution
 * @retval      None
 */
void pwm_init(uint8_t resolution, uint16_t freq)
{
    /* zero-initialize the config structure */
    ledc_timer_config_t ledc_timer = {0};
    ledc_channel_config_t ledc_channel = {0};

    ledc_timer.duty_resolution = resolution;        /* LEDC channel duty resolution */
    ledc_timer.freq_hz = freq;                      /* Configure LEDC source clock */
    ledc_timer.speed_mode = LEDC_PWM_MODE;          /* LEDC speed speed_mode, high-speed mode or low-speed mode */
    ledc_timer.timer_num = LEDC_PWM_TIMER;          /* The timer source of channel (0 - LEDC_TIMER_MAX-1) */
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;             /* LEDC timer frequency (Hz) */
    ledc_timer_config(&ledc_timer);                 /* LEDC timer configuration */

    ledc_channel.gpio_num = LEDC_PWM_CH0_GPIO;      /* the LEDC output gpio_num, if you want to use gpio16, gpio_num = 16 */
    ledc_channel.speed_mode = LEDC_PWM_MODE;        /* LEDC speed speed_mode, high-speed mode or low-speed mode */
    ledc_channel.channel = LEDC_PWM_CH0_CHANNEL;    /* LEDC channel (0 - LEDC_CHANNEL_MAX-1) */
    ledc_channel.intr_type = LEDC_INTR_DISABLE;     /* configure interrupt, Fade interrupt enable  or Fade interrupt disable */
    ledc_channel.timer_sel = LEDC_PWM_TIMER;        /* Select the timer source of channel (0 - LEDC_TIMER_MAX-1) */
    ledc_channel.duty = 0;                          /* LEDC channel duty, the range of duty setting is [0, (2**duty_resolution)] */
    ledc_channel_config(&ledc_channel);             /* LEDC channel configuration */

    ledc_fade_func_install(0);                      /* Initialize fade service */
}

/**
 * @brief       Set duty
 * @param       duty:Set duty
 * @retval      None
 */
void pwm_set_duty(uint16_t duty)
{
    ledc_set_fade_with_time(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, duty, LEDC_PWM_FADE_TIME);
    ledc_fade_start(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, LEDC_FADE_NO_WAIT);

    ledc_set_fade_with_time(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, 0, LEDC_PWM_FADE_TIME);
    ledc_fade_start(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, LEDC_FADE_NO_WAIT);
}
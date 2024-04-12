/**
 ****************************************************************************************************
 * @file        led.c
 * @author      ALIENTEK
 * @brief       LED code
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

#include "led.h"


/**
 * @brief       configure led pin
 * @param       None
 * @retval      None
 */
void led_init(void)
{
    /* zero-initialize the config structure */
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* disable interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT_OUTPUT;         /* set as output mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* enable pull-up mode */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* disable pull-down mode */
    gpio_init_struct.pin_bit_mask = 1ull << LED_GPIO_PIN;   /* bit mask of the pins that you want to set,e.g.GPIO1/2 */
    gpio_config(&gpio_init_struct);

    LED(1);
}
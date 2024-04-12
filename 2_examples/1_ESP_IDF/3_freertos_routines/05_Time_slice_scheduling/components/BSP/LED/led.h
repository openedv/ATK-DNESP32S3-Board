/**
 ****************************************************************************************************
 * @file        led.h
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

#ifndef __LED_H_
#define __LED_H_

#include "driver/gpio.h"


/* pin definition */
#define LED_GPIO_PIN    GPIO_NUM_1

/* GPIO status */
enum GPIO_OUTPUT_STATE
{
    PIN_RESET,
    PIN_SET
};

/* led operate definition */
#define LED(x)          do { x ?                                      \
                             gpio_set_level(LED_GPIO_PIN, PIN_SET) :  \
                             gpio_set_level(LED_GPIO_PIN, PIN_RESET); \
                        } while(0)

#define LED_TOGGLE()    do { gpio_set_level(LED_GPIO_PIN, !gpio_get_level(LED_GPIO_PIN)); } while(0)

/* function declaration */
void led_init(void);

#endif

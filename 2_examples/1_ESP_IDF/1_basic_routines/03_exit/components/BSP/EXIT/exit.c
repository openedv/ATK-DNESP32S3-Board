/**
 ****************************************************************************************************
 * @file        exti.c
 * @author      ALIENTEK
 * @brief       EXTI code
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

#include "exit.h"


/**
 * @brief       GPIO interrupt handler
 * @param       arg:User registered data
 * @retval      None
 */
static void IRAM_ATTR exit_gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    
    if (gpio_num == BOOT_INT_GPIO_PIN)
    {
        LED_TOGGLE();
    }
}

/**
 * @brief       configure pin with interrupts
 * @param       None
 * @retval      None
 */
void exit_init(void)
{
    /* zero-initialize the config structure */
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.mode = GPIO_MODE_INPUT;                    /* set as intput mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;           /* enable pull-up mode */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;      /* disable pull-down mode */
    gpio_init_struct.intr_type = GPIO_INTR_NEGEDGE;             /* interrupt of falling edge */
    gpio_init_struct.pin_bit_mask = 1ull << BOOT_INT_GPIO_PIN;  /* bit mask of the pins that you want to set,e.g.GPIO1/2 */
    gpio_config(&gpio_init_struct);
    
    /* install gpio isr service */
    gpio_install_isr_service(0);
    
    /* hook isr handler for specific gpio pin */
    gpio_isr_handler_add(BOOT_INT_GPIO_PIN, exit_gpio_isr_handler, (void*) BOOT_INT_GPIO_PIN);
}

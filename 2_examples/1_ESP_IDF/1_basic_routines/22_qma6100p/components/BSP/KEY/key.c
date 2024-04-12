/**
 ****************************************************************************************************
 * @file        key.c
 * @author      ALIENTEK
 * @brief       KEY code
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

#include "key.h"


/**
 * @brief       configure key pin
 * @param       None
 * @retval      None
 */
void key_init(void)
{
    /* zero-initialize the config structure */
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* disable interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT;                /* set as intput mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* enable pull-up mode */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* disable pull-down mode */
    gpio_init_struct.pin_bit_mask = 1ull << BOOT_GPIO_PIN;  /* bit mask of the pins that you want to set,e.g.GPIO1/2 */
    gpio_config(&gpio_init_struct);
}

/**
 * @brief       key scan function
 * @param       mode:0 / 1
 *              0:continuous pressing is not supported
 *              1:supports continuous pressing
 * @retval      key value:BOOT_PRES
 */
uint8_t key_scan(uint8_t mode)
{
    uint8_t keyval = 0;
    static uint8_t key_boot = 1;    /* release flag */

    if(mode)
    {
        key_boot = 1;
    }

    if (key_boot && (BOOT == 0))
    {
        vTaskDelay(10);
        key_boot = 0;

        if (BOOT == 0)
        {
            keyval = BOOT_PRES;
        }
    }
    else if (BOOT == 1)
    {
        key_boot = 1;
    }

    return keyval;
}

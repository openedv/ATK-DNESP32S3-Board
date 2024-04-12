/**
 ****************************************************************************************************
 * @file        dht11.h
 * @author      ALIENTEK
 * @brief       DHT11 code
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

#ifndef __DHT11_H
#define __DHT11_H

#include <driver/rmt.h>
#include <soc/rmt_reg.h>
#include "driver/gpio.h" 
#include <esp_log.h>

/* pin definition */
#define DHT11_DQ_GPIO_PIN       GPIO_NUM_0

typedef enum 
{
    DHT11_PIN_RESET = 0u,
    DHT11_PIN_SET
}DHT11_GPIO_PinState;

#define DHT11_DQ_IN     gpio_get_level(DHT11_DQ_GPIO_PIN)

/* dht11 operate definition */
#define DHT11_DQ_OUT(x) do{ x ?                                                 \
                            gpio_set_level(DHT11_DQ_GPIO_PIN, DHT11_PIN_SET) :  \
                            gpio_set_level(DHT11_DQ_GPIO_PIN, DHT11_PIN_RESET); \
                        }while(0)

/* function declaration */
void dht11_reset(void);
uint8_t dht11_init(void);
uint8_t dht11_check(void);
uint8_t dht11_read_data(uint8_t *temp,uint8_t *humi);

#endif
/**
 ****************************************************************************************************
 * @file        ds18b20.h
 * @author      ALIENTEK
 * @brief       DS18B20 code
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

#ifndef __DS18B20_H
#define __DS18B20_H

#include "driver/gpio.h" 
#include <esp_log.h>

/* pin definition */
#define DS18B20_DQ_GPIO_PIN       GPIO_NUM_0

typedef enum 
{
    DS18B20_PIN_RESET = 0u,
    DS18B20_PIN_SET
}DS18B20_GPIO_PinState;

/* ds18b20 operate definition */
#define DS18B20_DQ_IN     gpio_get_level(DS18B20_DQ_GPIO_PIN)

#define DS18B20_DQ_OUT(x) do{ x ?                                                   \
                            gpio_set_level(DS18B20_DQ_GPIO_PIN, DS18B20_PIN_SET) :  \
                            gpio_set_level(DS18B20_DQ_GPIO_PIN, DS18B20_PIN_RESET); \
                        }while(0)

/* function declaration */
void ds18b20_reset(void);
uint8_t ds18b20_check(void);
uint8_t ds18b20_read_bit(void);
uint8_t ds18b20_read_byte(void);
void ds18b20_write_byte(uint8_t data);
void ds18b20_start(void);
uint8_t ds18b20_init(void);
short ds18b20_get_temperature(void);

#endif

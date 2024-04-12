/**
 ****************************************************************************************************
 * @file        dht11.c
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

#include "dht11.h"


/**
* @brief       dht11 reset
* @param       None
* @retval      None
*/
void dht11_reset(void)
{
    DHT11_DQ_OUT(0);
    vTaskDelay(25);
    DHT11_DQ_OUT(1);
    esp_rom_delay_us(30);
}

/**
 * @brief       waitting for dht11 respond
 * @param       None
 * @retval      0:normal; 1:abnormal/nonentity
 */
uint8_t dht11_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;

    while (DHT11_DQ_IN && retry < 100)
    {
        retry++;
        esp_rom_delay_us(1);
    }

    if (retry >= 100)
    {
        rval = 1;
    }
    else
    {
        retry = 0;

        while (!DHT11_DQ_IN && retry < 100)
        {
            retry++;
            esp_rom_delay_us(1);
        }
        
        if (retry >= 100)
        {
            rval = 1;
        }
    }
    
    return rval;
}

/**
 * @brief       read one bit data
 * @param       None
 * @retval      0; 1
 */
uint8_t dht11_read_bit(void)
{
    uint8_t retry = 0;

    while (DHT11_DQ_IN && retry < 100)
    {
        retry++;
        esp_rom_delay_us(1);
    }

    retry = 0;

    while (!DHT11_DQ_IN && retry < 100)
    {
        retry++;
        esp_rom_delay_us(1);
    }

    esp_rom_delay_us(40);

    if (DHT11_DQ_IN)
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

/**
 * @brief       read one byte data
 * @param       None
 * @retval      read data
 */
static uint8_t dht11_read_byte(void)
{
    uint8_t i, data = 0;

    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        data |= dht11_read_bit();
    }

    return data;
}

/**
 * @brief       read from dht11
 * @param       temp: temperature,range:-20~60°
 * @param       humi: humidity,range:5%~95%
 * @retval      0:succeed; 1:failed
 */
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;

    dht11_reset();

    if (dht11_check() == 0)
    {
        for (i = 0; i < 5; i++)
        {
            buf[i] = dht11_read_byte();
        }

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else
    {
        return 1;
    }
    
    return 0;
}

/**
* @brief       DHT11 initialization
* @param       None
* @retval      0:normal; 1:abnormal/nonentity
*/
uint8_t dht11_init(void)
{
    gpio_config_t gpio_init_struct;

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;             /* disable interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT_OUTPUT_OD;          /* set as intput/output mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;           /* enable pull-up mode */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;      /* disable pull-down mode */
    gpio_init_struct.pin_bit_mask = 1ull << DHT11_DQ_GPIO_PIN;  /* bit mask of the pins that you want to set,e.g.GPIO1/2 */
    gpio_config(&gpio_init_struct);

    dht11_reset();
    return dht11_check();
}


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

#include "Arduino.h"
#include "esp_system.h"

/* pin definition */
#define DHT11_DQ_PIN      GPIO_NUM_0       

/* dht11 operate definition */
#define DHT11_DQ_OUT(x)   gpio_set_level(DHT11_DQ_PIN, x)       //digitalWrite(DHT11_DQ, x) 
#define DHT11_DQ_IN       gpio_get_level(DHT11_DQ_PIN)          //digitalRead(DHT11_DQ)

#define DHT11_MODE_IN     gpio_set_direction(DHT11_DQ_PIN, GPIO_MODE_INPUT)   //pinMode(DHT11_DQ, INPUT_PULLUP)
#define DHT11_MODE_OUT    gpio_set_direction(DHT11_DQ_PIN, GPIO_MODE_OUTPUT)  //pinMode(DHT11_DQ, OUTPUT)

/* function declaration */
uint8_t dht11_init(void);                               
uint8_t dht11_check(void);                              
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi);  

#endif

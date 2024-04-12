/**
 ****************************************************************************************************
 * @file        dht11.cpp
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
    DHT11_MODE_OUT;         
    DHT11_DQ_OUT(0);        
    delay(20);              
    DHT11_DQ_OUT(1);        
    delayMicroseconds(30);  
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

    DHT11_MODE_IN;                        

    while (DHT11_DQ_IN && retry < 100)    
    {
        retry++;
        delayMicroseconds(1);
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
            delayMicroseconds(1);
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
        delayMicroseconds(1);
    }

    retry = 0;

    while (!DHT11_DQ_IN && retry < 100) 
    {
        retry++;
        delayMicroseconds(1);
    }

    delayMicroseconds(40);              

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
uint8_t dht11_read_byte(void)
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
    dht11_reset();
    return dht11_check();
}



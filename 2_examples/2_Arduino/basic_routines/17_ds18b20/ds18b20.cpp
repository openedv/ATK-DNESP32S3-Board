/**
 ****************************************************************************************************
 * @file        ds18b20.cpp
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

#include "ds18b20.h"

/**
 * @brief       ds18b20 reset
 * @param       none
 * @retval      none
 */
static void ds18b20_reset(void)
{
    DS18B20_MODE_OUT;
    DS18B20_DQ_OUT(0);        
    delayMicroseconds(750);   
    DS18B20_DQ_OUT(1);        
    delayMicroseconds(15);    
}

/**
 * @brief       waitting for ds18b20 respond
 * @param       None
 * @retval      0:normal; 1:abnormal/nonentity
 */
uint8_t ds18b20_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;

    DS18B20_MODE_IN;
    while (DS18B20_DQ_IN && retry < 200)    
    {
        retry++;
        delayMicroseconds(1);
    }

    if (retry >= 240)
    {
        rval = 1;
    }
    else
    {
        retry = 0;

        while (!DS18B20_DQ_IN && retry < 240)  
        {
            retry++;
            delayMicroseconds(1);
        }

        if (retry >= 240)
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
static uint8_t ds18b20_read_bit(void)
{
    uint8_t data = 0;

    DS18B20_MODE_OUT;
    DS18B20_DQ_OUT(0);
    delayMicroseconds(2);
    DS18B20_DQ_OUT(1);
    delayMicroseconds(12);

    DS18B20_MODE_IN;
    if (DS18B20_DQ_IN)
    {
        data = 1;
    }

    delayMicroseconds(50);
    return data;
}

/**
 * @brief       read one byte data
 * @param       None
 * @retval      read data
 */
static uint8_t ds18b20_read_byte(void)
{
    uint8_t i, b, data = 0;

    for (i = 0; i < 8; i++)
    {
        b = ds18b20_read_bit(); 
        
        data |= b << i;
    }

    return data;
}

/**
 * @brief       write one byte data to ds18b20
 * @param       data: write data
 * @retval      None
 */
static void ds18b20_write_byte(uint8_t data)
{
    uint8_t j;

    DS18B20_MODE_OUT;

    for (j = 1; j <= 8; j++)
    {
        if (data & 0x01)
        {
            DS18B20_DQ_OUT(0);  
            delayMicroseconds(2);
            DS18B20_DQ_OUT(1);
            delayMicroseconds(60);
        }
        else
        {
            DS18B20_DQ_OUT(0);  
            delayMicroseconds(60);
            DS18B20_DQ_OUT(1);
            delayMicroseconds(2);
        }

        data >>= 1;             
    }
}

/**
 * @brief       temperature conversion
 * @param       None
 * @retval      None
 */
static void ds18b20_start(void)
{
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   /*  skip rom */
    ds18b20_write_byte(0x44);   /*  convert */
}

/**
* @brief       configure ds18B20
* @param       None
* @retval      0:normal; 1:abnormal/nonentity
*/
uint8_t ds18b20_init(void) 
{
    ds18b20_reset();
    return ds18b20_check();
}

/**
 * @brief       get temperature
 * @param       None
 * @retval      temperature （-550~1250）
 *   @note      The returned temperature value is magnified by a factor of 10.
 *              When you actually use it, you divide it by 10 to get the actual temperature.
 */
short ds18b20_get_temperature(void)
{
    uint8_t flag = 1;           /* default temperature is positive */
    uint8_t TL, TH;
    short temp;

    ds18b20_start();            /*  ds1820 start convert */
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   /*  skip rom */
    ds18b20_write_byte(0xbe);   /*  convert */
    TL = ds18b20_read_byte();   /*  LSB */
    TH = ds18b20_read_byte();   /*  MSB */

    if (TH > 7)     /* if the temperature is negative, the temperature of DS18B20 is expressed as follows */
    {   
        TH = ~TH;
        TL = ~TL;
        flag = 0;   
    }

    temp = TH;      
    temp <<= 8;
    temp += TL;     

    /* convert to the actual temperature */
    if (flag == 0)
    {   /* to convert the temperature to a negative temperature, +1 here refers to the previous instructions */
        temp = (double)(temp + 1) * 0.625;
        temp = -temp;   
    }
    else
    {
        temp = (double)temp * 0.625;				
    }

    return temp;
}



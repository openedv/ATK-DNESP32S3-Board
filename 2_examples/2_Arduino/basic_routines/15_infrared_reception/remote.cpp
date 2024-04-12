/**
 ****************************************************************************************************
 * @file        remote.cpp
 * @author      ALIENTEK
 * @brief       REMOTE code
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

#include "remote.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "spilcd.h"

IRrecv irrecv(RMT_RX_PIN);
decode_results results;

/**
 * @brief       remote initialization
 * @param       None
 * @retval      None
 */
void remote_init(void)
{
    irrecv.enableIRIn();
}

/**
 * @brief       remote scan
 * @param       None
 * @retval      0:no keys pressed
 *              other:value of Pressed key 
 */
uint8_t remote_scan(void)
{  
    uint8_t rmt_data = 0;         
    uint8_t remote_key = 0;      
    uint32_t raw_data = 0;        

    if (irrecv.decode(&results))                        /* decode infrared data */
    {
        raw_data = results.value;
        remote_key = (uint8_t)(raw_data >> 8);          /* control code */
        
        if ((remote_key != 0xFF) && (remote_key != 0))  /* get valid key value */
        {
            rmt_data = remote_key;
        }

        irrecv.resume();                                /* receive the next value */
        
        return rmt_data;
    }
    
    return 0;
}

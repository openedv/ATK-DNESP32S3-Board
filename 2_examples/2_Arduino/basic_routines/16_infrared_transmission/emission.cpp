/**
 ****************************************************************************************************
 * @file        emission.cpp
 * @author      ALIENTEK
 * @brief       EMISSION code
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

#include "emission.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "spilcd.h"

IRsend irsend(RMT_TX_PIN);

/**
 * @brief       configure emission
 * @param       None
 * @retval      None
 */
void emission_init(void)
{
    irsend.begin();
}

/**
 * @brief       send infrared data
 * @param       data:control code
 * @retval      None
 */
void emission_send(uint8_t data)
{  
    uint32_t send_data = 0;
    uint8_t data_oppo = 0;

    data_oppo = ~data;
    send_data = 0x00FF0000UL | data << 8 | data_oppo;
    irsend.sendNEC(send_data);
}




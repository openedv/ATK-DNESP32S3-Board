/**
 ****************************************************************************************************
 * @file        emission.h
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

#ifndef __REMOTE_H__
#define __REMOTE_H__

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_idf_version.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"
#include "renc.h"
#include "key.h"
#include "xl9555.h"


#define RMT_RX_PIN                  GPIO_NUM_2
#define RMT_TX_PIN                  GPIO_NUM_8
#define RMT_RESOLUTION_HZ           1000000     /* 1MHz, 1 tick = 1us */
#define RMT_NEC_DECODE_MARGIN       200

#define NEC_LEADING_CODE_DURATION_0 9000
#define NEC_LEADING_CODE_DURATION_1 4500
#define NEC_PAYLOAD_ZERO_DURATION_0 560
#define NEC_PAYLOAD_ZERO_DURATION_1 560
#define NEC_PAYLOAD_ONE_DURATION_0  560
#define NEC_PAYLOAD_ONE_DURATION_1  1690
#define NEC_REPEAT_CODE_DURATION_0  9000
#define NEC_REPEAT_CODE_DURATION_1  2250

/* function declaration */
void emission_init(void);
void example_parse_nec_frame(rmt_symbol_word_t *rmt_nec_symbols, size_t symbol_num);
bool RMT_Rx_Done_Callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data);

#endif
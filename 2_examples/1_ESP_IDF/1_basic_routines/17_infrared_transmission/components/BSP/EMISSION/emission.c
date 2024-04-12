/**
 ****************************************************************************************************
 * @file        emission.c
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

/**
 * @brief Saving NEC decode results
 */
uint16_t s_nec_code_address;
uint16_t s_nec_code_command;

QueueHandle_t receive_queue;
uint8_t tbuf[40];

/**
 * @brief       EMISSION initialization
 * @param       None
 * @retval      None
 */
void emission_init(void)
{
    uint8_t t = 0;

    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                 /* Clock source of RMT RX channel, channels in the same group must use the same clock source */
        .resolution_hz = RMT_RESOLUTION_HZ,             /* Channel clock resolution, in Hz */
        .mem_block_symbols = 64,                        /* Size of memory block */
        .gpio_num = RMT_RX_PIN,                    		/* GPIO number used by RMT RX channel. Set to -1 if unused */
    };
    rmt_channel_handle_t rx_channel = NULL;
    /* create RMT RX channel */
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

    QueueHandle_t receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
    assert(receive_queue);
    /* register RX done callback */
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = RMT_Rx_Done_Callback,
    };
    /* Set callbacks for RMT RX channel */
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));                                  /* 为RMT RX信道设置回调 */

    /* the following timing requirement is based on NEC protocol */
    rmt_receive_config_t receive_config = {
    .signal_range_min_ns = 1250,                /* the shortest duration for NEC signal is 560us, 1250ns < 560us, valid signal won't be treated as noise */
    .signal_range_max_ns = 12000000,            /* the longest duration for NEC signal is 9000us, 12000000ns > 9000us, the receive won't stop early */
    };

    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,         /* Clock source of RMT TX channel, channels in the same group must use the same clock source */
        .resolution_hz = RMT_RESOLUTION_HZ,     /* Channel clock resolution, in Hz */
        .mem_block_symbols = 64,                /* Size of memory block */
        .trans_queue_depth = 4,                 /* Depth of internal transfer queue, increase this value can support more transfers pending in the background */
        .gpio_num = RMT_TX_PIN,                 /* GPIO number used by RMT TX channel. Set to -1 if unused */
    };
    rmt_channel_handle_t tx_channel = NULL;
    /* create RMT TX channel */
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));

    /* modulate carrier to TX channel */
    rmt_carrier_config_t carrier_cfg = {
        .frequency_hz = 38000,  /* 38KHz */
        .duty_cycle = 0.33,
    };
    ESP_ERROR_CHECK(rmt_apply_carrier(tx_channel, &carrier_cfg));

    /* this example won't send NEC frames in a loop */
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0,
    };

    ir_nec_encoder_config_t nec_encoder_cfg = {
        .resolution = RMT_RESOLUTION_HZ,
    };
    rmt_encoder_handle_t nec_encoder = NULL;
    /* install IR NEC encoder */
    ESP_ERROR_CHECK(rmt_new_ir_nec_encoder(&nec_encoder_cfg, &nec_encoder));

    /* enable RMT TX and RX channels */
    ESP_ERROR_CHECK(rmt_enable(tx_channel));
    ESP_ERROR_CHECK(rmt_enable(rx_channel));

    /* save the received RMT symbols */
    rmt_symbol_word_t raw_symbols[64];
    rmt_rx_done_event_data_t rx_data;
    /* ready to receive */
    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));

    while (1)
    {
        /* wait for RX done signal */
        if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS)
        {
            /* parse the receive symbols and print the result */
            example_parse_nec_frame(rx_data.received_symbols, rx_data.num_symbols);
            /* start receive again */
            ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));
        }
        else
        {
            t++;

            if (t == 0)
            {
                t = 1;
            }
            /* timeout, transmit predefined IR NEC packets */
            const ir_nec_scan_code_t scan_code = {
                .command = t,
            };

            lcd_fill(116, 110, 176, 150, WHITE);
            sprintf((char *)tbuf, "%d", scan_code.command);
            printf("TX KEYVAL = %d\n", scan_code.command);
            lcd_show_string(116, 110, 200, 16, 16, (char *)tbuf, BLUE);
            ESP_ERROR_CHECK(rmt_transmit(tx_channel, nec_encoder, &scan_code, sizeof(scan_code), &transmit_config));
        }
    }
}

/**
 * @brief Check whether a duration is within expected range
 */
inline bool nec_check_in_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + RMT_NEC_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - RMT_NEC_DECODE_MARGIN));
}

/**
 * @brief Check whether a RMT symbol represents NEC logic zero
 */
bool nec_parse_logic0(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ZERO_DURATION_1);
}

/**
 * @brief Check whether a RMT symbol represents NEC logic one
 */
bool nec_parse_logic1(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ONE_DURATION_1);
}

/**
 * @brief Decode RMT symbols into NEC address and command
 */
bool nec_parse_frame(rmt_symbol_word_t *rmt_nec_symbols)
{
    rmt_symbol_word_t *cur = rmt_nec_symbols;
    uint16_t address = 0;
    uint16_t command = 0;

    bool valid_leading_code = nec_check_in_range(cur->duration0, NEC_LEADING_CODE_DURATION_0) &&
                              nec_check_in_range(cur->duration1, NEC_LEADING_CODE_DURATION_1);

    if (!valid_leading_code) 
    {
        return false;
    }

    cur++;

    for (int i = 0; i < 16; i++)
    {
        if (nec_parse_logic1(cur)) 
        {
            address |= 1 << i;
        } 
        else if (nec_parse_logic0(cur))
        {
            address &= ~(1 << i);
        } 
        else 
        {
            return false;
        }
        cur++;
    }

    for (int i = 0; i < 16; i++)
    {
        if (nec_parse_logic1(cur))
        {
            command |= 1 << i;
        }
        else if (nec_parse_logic0(cur))
        {
            command &= ~(1 << i);
        }
        else
        {
            return false;
        }
        cur++;
    }

    /* save address and command */
    s_nec_code_address = address;
    s_nec_code_command = command;

    return true;
}

/**
 * @brief Check whether the RMT symbols represent NEC repeat code
 */
bool nec_parse_frame_repeat(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_REPEAT_CODE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_REPEAT_CODE_DURATION_1);
}

/**
 * @brief Decode RMT symbols into NEC scan code and print the result
 */
void example_parse_nec_frame(rmt_symbol_word_t *rmt_nec_symbols, size_t symbol_num)
{
    switch (symbol_num) /* decode RMT symbols */
    {
        case 34:        /* NEC normal frame */
        {
            if (nec_parse_frame(rmt_nec_symbols) )
            {
                lcd_fill(116, 130, 176, 150, WHITE);
                sprintf((char *)tbuf, "%d", s_nec_code_command);
                printf("RX KEYCNT = %d\n", s_nec_code_command);
                lcd_show_string(116, 130, 200, 16, 16, (char *)tbuf, BLUE);
            }
            break;
        }
        
        case 2:         /* NEC repeat frame */
        {
            if (nec_parse_frame_repeat(rmt_nec_symbols))
            {
                printf("RX KEYCNT = %d, repeat\n", s_nec_code_command);
            }
            break;
        }

        default:        /* Unknown NEC frame */
        {
            printf("Unknown NEC frame\r\n\r\n");
            break;
        }
    }
}

/**
 * @brief       Prototype of RMT event callback
 * @param       None
 * @retval      None
 */
bool RMT_Rx_Done_Callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;

    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup); /* send the received RMT symbols to the parser task */
    return high_task_wakeup == pdTRUE;
}

/**
 ****************************************************************************************************
 * @file        lwip_demo.c
 * @author      ALIENTEK
 * @brief       lwip code
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

#include "lwip_demo.h"


/* remote ip */
#define IP_ADDR   "192.168.101.33"

#define LWIP_DEMO_RX_BUFSIZE         100                        /* rx buff size */
#define LWIP_DEMO_PORT               8080                       /* host port */
#define LWIP_SEND_THREAD_PRIO       ( tskIDLE_PRIORITY + 3 )

uint8_t g_lwip_demo_recvbuf[LWIP_DEMO_RX_BUFSIZE]; 

uint8_t g_lwip_demo_sendbuf[] = "ALIENTEK DATA \r\n";
uint8_t g_lwip_send_flag;
int g_sock = -1;
int g_lwip_connect_state = 0;
static void lwip_send_thread(void *arg);

/**
 * @brief       send data task
 * @param       None
 * @retval      None
 */
void lwip_data_send(void)
{
    xTaskCreate(lwip_send_thread, "lwip_send_thread", 4096, NULL, LWIP_SEND_THREAD_PRIO, NULL);
}

/**
 * @brief       lwip_demo
 * @param       None
 * @retval      None
 */
void lwip_demo(void)
{
    struct sockaddr_in atk_client_addr;
    err_t err;
    int recv_data_len;
    char *tbuf;
    
    lwip_data_send();
    
    while (1)
    {
sock_start:
        /* config ipv4 */
        g_lwip_connect_state = 0;
        atk_client_addr.sin_family = AF_INET;                   /* IPv4 */
        atk_client_addr.sin_port = htons(LWIP_DEMO_PORT);
        atk_client_addr.sin_addr.s_addr = inet_addr(IP_ADDR);
        g_sock = socket(AF_INET, SOCK_STREAM, 0);
        memset(&(atk_client_addr.sin_zero), 0, sizeof(atk_client_addr.sin_zero));
        
        tbuf = malloc(200);
        sprintf((char *)tbuf, "Port:%d", LWIP_DEMO_PORT);
        lcd_show_string(5, 170, 200, 16, 16, tbuf, MAGENTA);
        
        /* connect remote ip */
        err = connect(g_sock, (struct sockaddr *)&atk_client_addr, sizeof(struct sockaddr));

        if (err == -1)
        {
            lcd_show_string(5, 190, 200, 16, 16, "State:Disconnect", MAGENTA);
            g_sock = -1;
            closesocket(g_sock);
            free(tbuf);
            vTaskDelay(10);
            goto sock_start;
        }

        lcd_show_string(5, 190, 200, 16, 16, "State:Connection Successful", MAGENTA);
        g_lwip_connect_state = 1;
        
        while (1)
        {
            recv_data_len = recv(g_sock,g_lwip_demo_recvbuf,
                                 LWIP_DEMO_RX_BUFSIZE,0);
            if (recv_data_len <= 0 )
            {
                closesocket(g_sock);
                g_sock = -1;
                lcd_fill(5, 190, lcd_self.width,320, WHITE);
                lcd_show_string(5, 190, 200, 16, 16, "State:Disconnect", MAGENTA);
                free(tbuf);
                goto sock_start;
            }
            
            printf("%s\r\n",g_lwip_demo_recvbuf);
            vTaskDelay(10);
        }
    }
}

/**
 * @brief       send task
 * @param       pvParameters : None
 * @retval      None
 */
void lwip_send_thread(void *pvParameters)
{
    pvParameters = pvParameters;
    
    err_t err;
    
    while (1)
    {
        while (1)
        {
            if(((g_lwip_send_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) && (g_lwip_connect_state == 1))
            {
                err = write(g_sock, g_lwip_demo_sendbuf, sizeof(g_lwip_demo_sendbuf));
                
                if (err < 0)
                {
                    break;
                }
                
                g_lwip_send_flag &= ~LWIP_SEND_DATA;
            }
            
            vTaskDelay(10);
        }
        
        closesocket(g_sock);
    }
}

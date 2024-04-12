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


#define LWIP_DEMO_RX_BUFSIZE         200
#define LWIP_DEMO_PORT               8080
#define LWIP_SEND_THREAD_PRIO       ( tskIDLE_PRIORITY + 3 )


uint8_t g_lwip_demo_recvbuf[LWIP_DEMO_RX_BUFSIZE]; 
uint8_t g_lwip_demo_sendbuf[] = "ALIENTEK DATA \r\n";

uint8_t g_lwip_send_flag;
int g_sock_conn;
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
    struct sockaddr_in server_addr;
    struct sockaddr_in conn_addr;
    socklen_t addr_len;
    int err;
    int length;
    int sock_fd;
    char *tbuf;
    lwip_data_send();
    /* config ipv4 */
    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(LWIP_DEMO_PORT);

    tbuf = malloc(200);
    sprintf((char *)tbuf, "Port:%d", LWIP_DEMO_PORT);
    lcd_show_string(5, 170, 200, 16, 16, tbuf, BLUE);
    
    err = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (err < 0)
    {
        closesocket(sock_fd);
        free(tbuf);
    }

    err = listen(sock_fd, 4);

    if (err < 0)
    {
        closesocket(sock_fd);
    }
    
    while(1)
    {
        g_lwip_connect_state = 0;
        addr_len = sizeof(struct sockaddr_in);

        g_sock_conn = accept(sock_fd, (struct sockaddr *)&conn_addr, &addr_len);

        if (g_sock_conn < 0)
        {
            closesocket(sock_fd);
        }
        else 
        {
            lcd_show_string(5, 190, 200, 16, 16, "State:Connection...", BLUE);
            g_lwip_connect_state = 1;
        }

        while (1)
        {
            memset(g_lwip_demo_recvbuf,0,LWIP_DEMO_RX_BUFSIZE);
            length = recv(g_sock_conn, (unsigned int *)g_lwip_demo_recvbuf, sizeof(g_lwip_demo_recvbuf), 0);
            
            if (length <= 0)
            {
                lcd_show_string(5, 190, 200, 16, 16, "State:no Connection", BLUE);
                break;
            }
            
            printf("%s\r\n",g_lwip_demo_recvbuf);
        }


        if (g_sock_conn >= 0)
        {
            closesocket(g_sock_conn);
        }

        g_sock_conn = -1;
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
    
    while (1)
    {
        if(((g_lwip_send_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) && (g_lwip_connect_state == 1))
        {
            send(g_sock_conn, g_lwip_demo_sendbuf, sizeof(g_lwip_demo_sendbuf), 0);
            g_lwip_send_flag &= ~LWIP_SEND_DATA;
        }
        
        vTaskDelay(1);
    }
}

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

#define LWIP_DEMO_RX_BUFSIZE         200    /* rx buff size */
#define LWIP_DEMO_PORT               8080   /* host port */
#define LWIP_SEND_THREAD_PRIO       ( tskIDLE_PRIORITY + 3 )

uint8_t g_lwip_demo_recvbuf[LWIP_DEMO_RX_BUFSIZE]; 

char g_lwip_demo_sendbuf[] = "ALIENTEK DATA \r\n";

uint8_t g_lwip_send_flag;
static struct sockaddr_in dest_addr;
struct sockaddr_in g_local_info;
socklen_t g_sock_fd;
static void lwip_send_thread(void *arg);
extern QueueHandle_t g_display_queue;


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
    char *tbuf;
    lwip_data_send();
    /* config ipv4 */
    dest_addr.sin_addr.s_addr = inet_addr(IP_ADDR);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(LWIP_DEMO_PORT);
    
    g_local_info.sin_family = AF_INET;
    g_local_info.sin_port = htons(LWIP_DEMO_PORT);
    g_local_info.sin_addr.s_addr = htons(INADDR_ANY);

    g_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    tbuf = malloc(200);
    sprintf((char *)tbuf, "Port:%d", LWIP_DEMO_PORT);
    lcd_show_string(0, 170, 200, 16, 16, tbuf, MAGENTA);
    
    /* bind */
    bind(g_sock_fd, (struct sockaddr *)&g_local_info, sizeof(g_local_info));

    while (1)
    {
        memset(g_lwip_demo_recvbuf, 0, sizeof(g_lwip_demo_recvbuf));
        recv(g_sock_fd, (void *)g_lwip_demo_recvbuf, sizeof(g_lwip_demo_recvbuf), 0);
        printf("%s\r\n",g_lwip_demo_recvbuf);
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
        if ((g_lwip_send_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA)
        {
            printf("send\r\n");
            sendto(g_sock_fd,
                  (char *)g_lwip_demo_sendbuf,
                  sizeof(g_lwip_demo_sendbuf), 0,
                  (struct sockaddr *)&dest_addr,
                  sizeof(dest_addr));

            g_lwip_send_flag &= ~LWIP_SEND_DATA;
        }
        
        vTaskDelay(100);
   }
}

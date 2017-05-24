/**
* @file D:\Documents\Projects\chargersdk\src\app\task_tcp_client.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-04
*/
#include "includes.h"
#include "lwip/sockets.h"
#include "ifconfig.h"
#include "lwip_init.h"
#include "libEcharge/ech_protocol.h"
#include "evse_globals.h"
#include "task_tcp_client.h"

uint8_t  tcp_client_sendbuf[TCP_CLIENT_BUFSIZE]; //TCP客户端发送数据缓冲
uint8_t  tcp_client_recvbuf[TCP_CLIENT_BUFSIZE]; //TCP客户端接收数据缓冲区

u32_t recv_len = 0;
u32_t send_len = 0;
void vTaskTCPClient(void *pvParameters)
{
    EventBits_t uxBitsTCP;

    int sock;
    int ret;
    struct sockaddr_in server_addr;

    int maxfdp;
    struct fd_set readfds;
    struct fd_set writefds;
    struct fd_set exceptfds;
    struct timeval timeout = {5, 0};

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(ifconfig.server_port);
    server_addr.sin_addr.s_addr = ifconfig.server_ip.addr;
    server_addr.sin_len = sizeof(server_addr);
    int i;

    while(1)
    {
        uxBitsTCP = xEventGroupWaitBits(xHandleEventLwIP, defEventBitPPPup, pdFALSE, pdTRUE, 0);
        if((uxBitsTCP & defEventBitPPPup) == defEventBitPPPup)
        {
            sock = lwip_socket(AF_INET, SOCK_STREAM, 0);
            ret = lwip_connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
            switch(ret) //sock 错误判断
            {
            case -1:

                break;
            case 0:
                while(1)
                {
                    FD_ZERO(&readfds);
                    FD_ZERO(&writefds);
                    FD_ZERO(&exceptfds);
                    FD_SET(sock, &readfds);
                    FD_SET(sock, &writefds);
                    FD_SET(sock, &exceptfds);
                    maxfdp = sock + 1;
                    switch(lwip_select(maxfdp, &readfds, &writefds, &exceptfds, &timeout))
                    {
                    case -1:
                        break;//select错误
                    case 0:
                        break;
                    default:
                        if(FD_ISSET(sock, &exceptfds))
                        {
                            printf_safe("exceptfds exceptfds exceptfds!\n");
                            pppClose(ppp);
                        }
                        if(FD_ISSET(sock, &readfds))//测试网络是否有数据
                        {
                            recv_len = lwip_read(sock, tcp_client_recvbuf, TCP_CLIENT_BUFSIZE);
                            printf_safe("\nTCP Recv: ");
                            for(i = 0; i < recv_len; i++)
                            {
                                printf_safe("%02X ", tcp_client_recvbuf[i]);
                            }
                            printf_safe("\n");
                            pechProto->recvResponse(pechProto, tcp_client_recvbuf, recv_len, 3);
                        }
                        if(FD_ISSET(sock, &writefds))
                        {
                            uxBitsTCP = xEventGroupWaitBits(xHandleEventLwIP,
                                                            defEventBitTCPClientSendReq,
                                                            pdTRUE, pdTRUE, 0);
                            if((uxBitsTCP & defEventBitTCPClientSendReq) == defEventBitTCPClientSendReq) //有数据要发送
                            {
                                ret = lwip_write(sock,
                                                 tcp_client_sendbuf,
                                                 send_len);
                                if(ret < 0)
                                {
                                    printf_safe("发送失败\r\n");
                                }
                                else
                                {
                                    xEventGroupSetBits(xHandleEventLwIP, defEventBitTCPClientSendOK);
                                }
                            }
                        }
                        break;
                    }
                    vTaskDelay(100);
                }//end of while(1)
                break;
            default:
                break;
            }
        }
        vTaskDelay(1000);
    }
    while(1);
}

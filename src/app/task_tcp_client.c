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

#define TCP_CLIENT_BUFSIZE   1500
uint8_t  tcp_client_sendbuf[TCP_CLIENT_BUFSIZE] ; //TCP�ͻ��˷������ݻ���
uint8_t  tcp_client_recvbuf[TCP_CLIENT_BUFSIZE]; //TCP�ͻ��˽������ݻ�����

u32_t recv_len;
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
            switch(ret) //sock �����ж�
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
                        break;//select����
                    case 0:
                        break;
                    default:
                        if(FD_ISSET(sock, &exceptfds))
                        {
                            printf_safe("exceptfds exceptfds exceptfds!\n");
                            pppClose(ppp);
                        }
                        if(FD_ISSET(sock, &readfds))//���������Ƿ�������
                        {
                            recv_len = lwip_read(sock, tcp_client_recvbuf, TCP_CLIENT_RX_BUFSIZE);
//                            printf_safe("\nTCP Recv: ");
//                            for(i = 0; i < recv_len; i++)
//                            {
//                                printf_safe("%c", tcp_client_recvbuf[i]);
//                            }
//                            printf_safe("\n");
                            //memset(tcp_client_recvbuf, 0, sizeof(tcp_client_recvbuf));
                            xEventGroupSetBits(xHandleEventLwIP, defEventBitTCPRxEcho);
                        }
                        if(FD_ISSET(sock, &writefds))
                        {
                            uxBitsTCP = xEventGroupWaitBits(xHandleEventLwIP,
                                                            defEventBitTCPClientSendReq,
                                                            pdTRUE, pdTRUE, 0);

                            if((uxBitsTCP & defEventBitTCPClientSendReq) == defEventBitTCPClientSendReq) //������Ҫ����
                            {
                                ret = lwip_write(sock,
                                                 tcp_client_sendbuf,
                                                 send_len);
                                if(ret < 0)
                                {
                                    printf_safe("����ʧ��\r\n");
                                }
                                else
                                {
                                    xEventGroupSetBits(xHandleEventLwIP, defEventBitTCPClientSendOK);
                                }
                            }
                            uxBitsTCP = xEventGroupWaitBits(xHandleEventLwIP,
                                                            defEventBitTCPRxEcho,
                                                            pdTRUE, pdTRUE, 0);
                            if((uxBitsTCP & defEventBitTCPRxEcho) == defEventBitTCPRxEcho) //������Ҫ����
                            {
                                ret = lwip_write(sock,
                                                 tcp_client_recvbuf,
                                                 recv_len); //����tcp_server_sentbuf�е�����
                                if(ret < 0)
                                {
                                    printf_safe("����ʧ��\r\n");
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
}

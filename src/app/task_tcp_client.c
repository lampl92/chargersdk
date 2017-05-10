/**
* @file D:\Documents\Projects\chargersdk\src\app\task_tcp_client.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-04
*/
#include "includes.h"
#include "lwip/api.h"
#include "ifconfig.h"
#include "lwip/sockets.h"

#define TCP_CLIENT_RX_BUFSIZE   1500
uint8_t  tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE]; //TCP�ͻ��˽������ݻ�����
void vTaskTCPClient(void *pvParameters)
{
    uint8_t  tcp_client_sendbuf[128] ; //TCP�ͻ��˷������ݻ���

    u32_t data_len = 0;

    err_t err;
    int recv_len;

    EventBits_t uxBitsTCP;


////socket

    int sock;
    struct sockaddr_in server_ip;

    int maxfdp;
    struct fd_set readfds;
    struct fd_set writefds;
    struct fd_set exceptfds;
    struct timeval timeout = {0, 0};

    memset(&server_ip, 0, sizeof(server_ip));
    server_ip.sin_family = AF_INET;
    server_ip.sin_port = htons(ifconfig.server_port);
    server_ip.sin_addr.s_addr = ifconfig.server_ip.addr;

    int i;
    for(i = 0; i < 128;i ++)
    {
        tcp_client_sendbuf[i] = '0'+i;
    }
    while(1)
    {
        uxBitsTCP = xEventGroupWaitBits(xHandleEventlwIP, defEventBitPPPup, pdFALSE, pdTRUE, 0);
        if((uxBitsTCP & defEventBitPPPup) == defEventBitPPPup)
        {
            sock = lwip_socket(AF_INET, SOCK_STREAM, 0);
            err = lwip_connect(sock, (struct sockaddr *)&server_ip, sizeof(server_ip));
            switch(err) //sock �����ж�
            {
            case -1:

                break;
            case 0:
                while(1)
                {
                    FD_ZERO(&readfds);
                    FD_ZERO(&writefds);
                    FD_SET(sock, &readfds);
                    FD_SET(sock, &writefds);
                    maxfdp = sock + 1;
                    switch(lwip_select(maxfdp, &readfds, &writefds, NULL, &timeout))
                    {
                    case -1:
                        break;//select����
                    case 0:
                        break;
                    default:
                        if(FD_ISSET(sock, &readfds))//���������Ƿ�������
                        {
                            recv_len = lwip_read(sock, tcp_client_recvbuf, TCP_CLIENT_RX_BUFSIZE);
                            printf_safe("\nTCP Recv: ");
                            for(i = 0; i < recv_len; i++)
                            {
                                printf_safe("%c", tcp_client_recvbuf[i]);
                            }
                            printf_safe("\n");
                            memset(tcp_client_recvbuf, 0, sizeof(tcp_client_recvbuf));
                        }
                        if(FD_ISSET(sock, &writefds))
                        {
                            uxBitsTCP = xEventGroupWaitBits(xHandleEventlwIP,
                                                            defEventBitTCPClientSendReq,
                                                            pdTRUE, pdTRUE, 0);

                            if((uxBitsTCP & defEventBitTCPClientSendReq) == defEventBitTCPClientSendReq) //������Ҫ����
                            {
                                err = lwip_write(sock,
                                                 tcp_client_sendbuf,
                                                 70); //����tcp_server_sentbuf�е�����
                                if(err < 0)
                                {
                                    printf_safe("����ʧ��\r\n");
                                }
                            }
                        }
                        break;
                    }
                    vTaskDelay(100);
                }//while(1)
                break;
            default:
                break;
            }
        }
        vTaskDelay(1000);
    }
}

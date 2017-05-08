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
#include "sockets.h"

#define TCP_CLIENT_RX_BUFSIZE   1500
void vTaskTCPClient(void *pvParameters)
{
    uint8_t  tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE]; //TCP客户端接收数据缓冲区
    uint8_t  *tcp_client_sendbuf = "hello server im rgw\r\n"; //TCP客户端发送数据缓冲区

    u32_t data_len = 0;

    err_t err;
    int recv_len;

    EventBits_t uxBitsTCP;


////socket

    int s;
    struct sockaddr_in server_ip;

    memset(&server_ip, 0, sizeof(server_ip));
    server_ip.sin_family = AF_INET;
    server_ip.sin_port = htons(ifconfig.server_port);
    server_ip.sin_addr.s_addr = ifconfig.server_ip.addr;

    while(1)
    {
        uxBitsTCP = xEventGroupWaitBits(xHandleEventlwIP, defEventBitPPPup, pdFALSE, pdTRUE, 0);
        if((uxBitsTCP & defEventBitPPPup) == defEventBitPPPup)
        {
            s = lwip_socket(AF_INET, SOCK_STREAM, 0);
            err = lwip_connect(s, (struct sockaddr *)&server_ip, sizeof(server_ip));
            if(err < 0)
            {
            }
            else if (err == ERR_OK)    //处理新连接的数据
            {
//                tcp_clientconn->recv_timeout = 10;
//                netconn_getaddr(tcp_clientconn, &loca_ipaddr, &loca_port, 1); //获取本地IP主机IP地址和端口号
//                printf_safe("服务器地址:%s, 服务器端口:%d\r\n", ipaddr_ntoa(&server_ipaddr), server_port);
//                printf_safe("本地地址:  %s, 本机端口  :%d\r\n", ipaddr_ntoa(&loca_ipaddr), loca_port);
                while(1)
                {
                    uxBitsTCP = xEventGroupWaitBits(xHandleEventlwIP,
                                                    defEventBitTCPClientSendReq,
                                                    pdTRUE, pdTRUE, 0);

                    if((uxBitsTCP & defEventBitTCPClientSendReq) == defEventBitTCPClientSendReq) //有数据要发送
                    {
                        err = lwip_write(s,
                                        tcp_client_sendbuf,
                                        strlen((char *)tcp_client_sendbuf)); //发送tcp_server_sentbuf中的数据
                        if(err < 0)
                        {
                            printf_safe("发送失败\r\n");
                        }
                    }

                    if((recv_len = lwip_read(s, tcp_client_recvbuf, TCP_CLIENT_RX_BUFSIZE)) > 0) //接收到数据
                    {
                        printf_safe("%s\r\n", tcp_client_recvbuf);
                        memset(tcp_client_recvbuf, 0, sizeof(tcp_client_recvbuf));
                    }
                    vTaskDelay(100);
                }
            }
        }
        vTaskDelay(1000);
    }
}

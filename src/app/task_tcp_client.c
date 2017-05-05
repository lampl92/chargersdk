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

#define TCP_CLIENT_RX_BUFSIZE   2000    //���ջ���������

void vTaskTCPClient(void *pvParameters)
{
    uint8_t  tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE]; //TCP�ͻ��˽������ݻ�����
    uint8_t  *tcp_client_sendbuf = "hello server im rgw\r\n"; //TCP�ͻ��˷������ݻ�����

    struct netconn *tcp_clientconn;
    ip_addr_t server_ipaddr, loca_ipaddr;
    u16_t  server_port, loca_port;

    u32_t data_len = 0;
    struct pbuf *q;
    err_t err, recv_err;

    EventBits_t uxBitsTCP;

    server_ipaddr.addr = ifconfig.server_ip.addr;
    server_port = ifconfig.server_port;
    while(1)
    {
        uxBitsTCP = xEventGroupWaitBits(xHandleEventlwIP, defEventBitPPPup, pdFALSE, pdTRUE, 0);
        if((uxBitsTCP & defEventBitPPPup) == defEventBitPPPup)
        {
            tcp_clientconn = netconn_new(NETCONN_TCP);  //����һ��TCP����
            err = netconn_connect(tcp_clientconn, &server_ipaddr, server_port);//���ӷ�����
            if(err != ERR_OK)
            {
                netconn_delete(tcp_clientconn);    //����ֵ������ERR_OK,ɾ��tcp_clientconn����
            }
            else if (err == ERR_OK)    //���������ӵ�����
            {
                struct netbuf *recvbuf;
                tcp_clientconn->recv_timeout = 10;
                //netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
                printf_safe("�����Ϸ�����%s,�����˿ں�Ϊ:%d\r\n", ipaddr_ntoa(&server_port), loca_port);
                while(1)
                {
                    uxBitsTCP = xEventGroupWaitBits(xHandleEventlwIP,
                                                    defEventBitTCPClientSendReq,
                                                    pdTRUE, pdTRUE, 0);

                    if((uxBitsTCP & defEventBitTCPClientSendReq) == defEventBitTCPClientSendReq) //������Ҫ����
                    {
                        err = netconn_write(tcp_clientconn,
                                            tcp_client_sendbuf,
                                            strlen((char *)tcp_client_sendbuf),
                                            NETCONN_COPY); //����tcp_server_sentbuf�е�����
                        if(err != ERR_OK)
                        {
                            printf_safe("����ʧ��\r\n");
                        }
                    }

                    if((recv_err = netconn_recv(tcp_clientconn, &recvbuf)) == ERR_OK) //���յ�����
                    {
                        taskENTER_CRITICAL(); //���ж�
                        memset(tcp_client_recvbuf, 0, TCP_CLIENT_RX_BUFSIZE); //���ݽ��ջ���������
                        for(q = recvbuf->p; q != NULL; q = q->next) //����������pbuf����
                        {
                            //�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
                            //�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
                            if(q->len > (TCP_CLIENT_RX_BUFSIZE - data_len))
                            {
                                memcpy(tcp_client_recvbuf + data_len, q->payload, (TCP_CLIENT_RX_BUFSIZE - data_len));    //��������
                            }
                            else
                            {
                                memcpy(tcp_client_recvbuf + data_len, q->payload, q->len);
                            }
                            data_len += q->len;
                            if(data_len > TCP_CLIENT_RX_BUFSIZE)
                            {
                                break;    //����TCP�ͻ��˽�������,����
                            }
                        }
                        taskEXIT_CRITICAL();  //���ж�
                        data_len = 0; //������ɺ�data_lenҪ���㡣
                        printf_safe("%s\r\n", tcp_client_recvbuf);
                        netbuf_delete(recvbuf);
                    }
                    else if(recv_err == ERR_CLSD)   //�ر�����
                    {
                        netconn_close(tcp_clientconn);
                        netconn_delete(tcp_clientconn);
                        printf_safe("������%s�Ͽ�����\r\n", ipaddr_ntoa(&server_port));
                        break;
                    }
                    vTaskDelay(100);
                }
            }
        }
        vTaskDelay(1000);
    }



}

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

#define TCP_CLIENT_RX_BUFSIZE   2000    //接收缓冲区长度

void vTaskTCPClient(void *pvParameters)
{
    uint8_t  tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE]; //TCP客户端接收数据缓冲区
    uint8_t  *tcp_client_sendbuf = "hello server im rgw\r\n"; //TCP客户端发送数据缓冲区

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
            tcp_clientconn = netconn_new(NETCONN_TCP);  //创建一个TCP链接
            err = netconn_connect(tcp_clientconn, &server_ipaddr, server_port);//连接服务器
            if(err != ERR_OK)
            {
                netconn_delete(tcp_clientconn);    //返回值不等于ERR_OK,删除tcp_clientconn连接
            }
            else if (err == ERR_OK)    //处理新连接的数据
            {
                struct netbuf *recvbuf;
                tcp_clientconn->recv_timeout = 10;
                //netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); //获取本地IP主机IP地址和端口号
                printf_safe("连接上服务器%s,本机端口号为:%d\r\n", ipaddr_ntoa(&server_port), loca_port);
                while(1)
                {
                    uxBitsTCP = xEventGroupWaitBits(xHandleEventlwIP,
                                                    defEventBitTCPClientSendReq,
                                                    pdTRUE, pdTRUE, 0);

                    if((uxBitsTCP & defEventBitTCPClientSendReq) == defEventBitTCPClientSendReq) //有数据要发送
                    {
                        err = netconn_write(tcp_clientconn,
                                            tcp_client_sendbuf,
                                            strlen((char *)tcp_client_sendbuf),
                                            NETCONN_COPY); //发送tcp_server_sentbuf中的数据
                        if(err != ERR_OK)
                        {
                            printf_safe("发送失败\r\n");
                        }
                    }

                    if((recv_err = netconn_recv(tcp_clientconn, &recvbuf)) == ERR_OK) //接收到数据
                    {
                        taskENTER_CRITICAL(); //关中断
                        memset(tcp_client_recvbuf, 0, TCP_CLIENT_RX_BUFSIZE); //数据接收缓冲区清零
                        for(q = recvbuf->p; q != NULL; q = q->next) //遍历完整个pbuf链表
                        {
                            //判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
                            //的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
                            if(q->len > (TCP_CLIENT_RX_BUFSIZE - data_len))
                            {
                                memcpy(tcp_client_recvbuf + data_len, q->payload, (TCP_CLIENT_RX_BUFSIZE - data_len));    //拷贝数据
                            }
                            else
                            {
                                memcpy(tcp_client_recvbuf + data_len, q->payload, q->len);
                            }
                            data_len += q->len;
                            if(data_len > TCP_CLIENT_RX_BUFSIZE)
                            {
                                break;    //超出TCP客户端接收数组,跳出
                            }
                        }
                        taskEXIT_CRITICAL();  //开中断
                        data_len = 0; //复制完成后data_len要清零。
                        printf_safe("%s\r\n", tcp_client_recvbuf);
                        netbuf_delete(recvbuf);
                    }
                    else if(recv_err == ERR_CLSD)   //关闭连接
                    {
                        netconn_close(tcp_clientconn);
                        netconn_delete(tcp_clientconn);
                        printf_safe("服务器%s断开连接\r\n", ipaddr_ntoa(&server_port));
                        break;
                    }
                    vTaskDelay(100);
                }
            }
        }
        vTaskDelay(1000);
    }



}

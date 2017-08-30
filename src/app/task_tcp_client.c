/**
* @file D:\Documents\Projects\chargersdk\src\app\task_tcp_client.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-04
*/
#include "includes.h"
#include "netif/ppp/ppp.h"
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

static int tcp_write(uint8_t *pbuff, uint32_t len)
{
    DR_MODEM_e ret;
    ret = modem_write(pModem, pbuff, len);
    if(ret == DR_MODEM_OK)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

/** @brief
 *
 * @param pbuff uint8_t*
 * @param len uint32_t
 * @return uint32_t   实际接收长度
 *
 */
static uint32_t tcp_read(uint8_t *pbuff, uint32_t len)
{

}

void vTaskTCPClient(void *pvParameters)
{
    EventBits_t uxBitsTCP;
    int i;

    while(1)
    {
        switch(pModem->state)
        {
        case DS_MODEM_TCP_KEEP:
            if(FD_ISSET(sock, &readfds))//测试网络是否有数据
            {
                recv_len = tcp_read(tcp_client_recvbuf, TCP_CLIENT_BUFSIZE);
                printf_safe("\nTCP Recv: ");
                for(i = 0; i < recv_len; i++)
                {
                    printf_safe("%02X ", tcp_client_recvbuf[i]);
                }
                printf_safe("\n");
                pechProto->recvResponse(pechProto, pEVSE, tcp_client_recvbuf, recv_len, 3);
            }

                uxBitsTCP = xEventGroupWaitBits(xHandleEventTCP,
                                                defEventBitTCPClientSendReq,
                                                pdTRUE, pdTRUE, 0);
                if((uxBitsTCP & defEventBitTCPClientSendReq) == defEventBitTCPClientSendReq) //有数据要发送
                {
                    ret = tcp_write(tcp_client_sendbuf, send_len);
                    if(ret < 0)
                    {
                        printf_safe("发送失败\r\n");
                    }
                    else
                    {
                        xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientSendOK);
                    }
                }

            break;
        }
        vTaskDelay(100);
    }
}

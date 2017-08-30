#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "esp8266.h"
#include "bsp.h"
#include "user_app.h"

#include "FreeRTOS.h"
#include "event_groups.h"

#ifdef EVSE_DEBUG

#undef   GPRS_set
#undef   GPRS_reset
#define GPRS_set        PBout(3) = 1
#define GPRS_reset      PBout(3) = 0

#endif

#define DEVDEBUG(msg)  do{printf_safe(msg);}while(0);
#define TCP_CLIENT_BUFSIZE   1500
DevWIFI_t *pWIFI;

uint8_t  tcp_client_sendbuf[TCP_CLIENT_BUFSIZE]; //TCP�ͻ��˷������ݻ���
uint8_t  tcp_client_recvbuf[TCP_CLIENT_BUFSIZE]; //TCP�ͻ��˽������ݻ�����

uint32_t recv_len = 0;
uint32_t send_len = 0;


static uint32_t wifi_UART_puts(uint8_t *pbuff, uint32_t len)
{
    return uart_write(UART_PORT_WIFI, pbuff, len);
}

static uint32_t wifi_UART_gets(DevWIFI_t *pWIFI, uint8_t *line, uint32_t len)
{
    uint32_t   cnt  = 0;
//    if(xSemaphoreTake(pWIFI->xMutex, 10000) == pdTRUE)
//    {
    cnt = uart_read(UART_PORT_WIFI, line, len, 100);
//        xSemaphoreGive(pWIFI->xMutex);
    return cnt;
//    }
//    else
    {
        //û�ȵ����뿪��
    }
    return cnt;
}
static uint32_t wifi_send_at(uint8_t *format, ...)
{
    uint8_t     cmd[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    va_list  va;
    uint32_t      n;


    va_start(va, format);
    n  = vsnprintf(cmd, sizeof(cmd) - 1, format, va);
    va_end(va);

    cmd[strlen(cmd)] = '\n';
    wifi_UART_puts(cmd, strlen(cmd));

    cmd[strlen(cmd) - 1]  = '\0';
    DEVDEBUG(("%s\r\n", cmd));

    return n;
}


/**
 * modem_get_at_reply
 *
 * @param      reply   The reply
 * @param      len     The length
 * @param      key     �ؼ���
 * @param      second  ��ʱʱ��
 *
 * @return     { description_of_the_return_value }
 */
static DR_WIFI_e wifi_get_at_reply(uint8_t *reply, uint32_t len, const uint8_t *key, uint32_t second)
{
    uint8_t  *p;
    uint32_t  time;
    uint32_t n;
    DR_WIFI_e    ret;

    time  = 0;
    while (1)
    {
        n  = wifi_UART_gets(pWIFI, reply, len);
        if ( n > 0 )
        {
            //�����ж�������ģ�����������������
            p  = strstr(reply, "CLOSED");
            if ( p )
            {
                pWIFI->state = DS_WIFI_TCP_OPEN;
                ret = DR_WIFI_CLOSED;
                break;
            }
            p  = strstr(reply, "+IPD");
            if ( p )
            {
                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientRecvValid);
                ret = DR_WIFI_READ;
                break;
            }
            if(key == NULL)
            {
                ret = DR_WIFI_OK;
                break;
            }
            p  = strstr(reply, key);
            if ( p )
            {
                ret  = DR_WIFI_OK;
                break;
            }
            p  = strstr(reply, "ERROR");
            if ( p )
            {
                ret  = DR_WIFI_ERROR;
                break;
            }
            memset(reply, '\0', n);
            n  = 0;
        }

        time += 100;
        if ( time > second * 1000 )
        {
            ret  = DR_WIFI_TIMEOUT;
            break;
        }

        vTaskDelay(100);
    }

    DEVDEBUG(("%s\r\n\r\n", reply));
    return ret;
}

DR_WIFI_e wifi_RESET(DevWIFI_t *pWIFI)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_WIFI_e ret;

    wifi_send_at("AT+RST\r", 1, 1);
    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "ready", 20);

    return ret;
}

/** @brief ��ʼ��modem����Key����ģ�飬���AT��������
 *
 * @param void
 * @return int
 *
 */
DR_WIFI_e wifi_open(DevWIFI_t *pWIFI)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    DR_WIFI_e ret;

    DEVDEBUG("modem open: \r\n");
    wifi_send_at("AT\r");
    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if(ret == DR_WIFI_OK)
    {
        pWIFI->state = DS_WIFI_ON;
        return ret;
    }
    ret = wifi_RESET(pWIFI);
    switch(ret)
    {
    case DR_WIFI_OK:
        wifi_send_at("AT\r");
        ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
        pWIFI->state = DS_WIFI_ON;
        break;
    case DR_WIFI_ERROR:
    case DR_WIFI_TIMEOUT:
        break;
    }

    return ret;
}

/** @brief �رջ���
 *
 * @param void
 * @return int
 *
 */
static DR_WIFI_e wifi_disable_echo(void)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    DR_WIFI_e ret;

    wifi_send_at("ATE0\r");
    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}

/** @brief ����ģʽ 1 STA�� 2 AP�� 3 STA+AP
 *
 * @param pWifi DevWIFI_t*
 * @return DR_WIFI_e
 *
 */
static DR_WIFI_e wifi_CWMODE(DevWIFI_t *pWIFI)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_WIFI_e ret;

    wifi_send_at("AT+CWMODE=%d\r", 1);

    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    switch(ret)
    {
    case DR_WIFI_OK:
        break;
    }

    return ret;
}

/** @brief ����ģʽ 1 ͸���� 0 ��ͨ
 *
 * @param pWifi DevWIFI_t*
 * @return DR_WIFI_e
 *
 */
static DR_WIFI_e wifi_CIPMODE(DevWIFI_t *pWIFI)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_WIFI_e ret;

    wifi_send_at("AT+CIPMODE=%d\r", 1);

    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    switch(ret)
    {
    case DR_WIFI_OK:
        break;
    }

    return ret;
}

/** @brief ��ȡlocIP
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
static DR_WIFI_e modem_get_LOCIP(DevWIFI_t *pWIFI)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    uint8_t *p;
    DR_WIFI_e ret;

    wifi_send_at("AT+QILOCIP\r");

    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, NULL, 3);
    if(ret == DR_WIFI_OK)
    {
        sscanf(reply, "%s", pWIFI->status.strLocIP);
    }
    return ret;
}

DR_WIFI_e wifi_set_tcp_open(DevWIFI_t *pWIFI, echProtocol_t *pProto)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_WIFI_e ret;

    wifi_send_at("AT+CIPSTART=\"TCP\",\"%s\",%d\r",
                 pProto->info.strServerIP,
                 pProto->info.usServerPort);

    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "OK", 75);
    if(ret == DR_WIFI_OK)
    {
        sscanf(reply, "%*s%s", s);
        if(strcmp(s, "OK") == 0) //"CONNECT OK"
        {
            pWIFI->status.eConnect = WIFI_AP_CONNECT_OK;
        }
        else if(strcmp(s, "CONNECT") == 0)  //"ALREADY CONNECT"
        {
            pWIFI->status.eConnect = WIFI_AP_CONNECT_OK;
        }
    }
    else if(ret == DR_WIFI_ERROR)
    {
        pWIFI->status.eConnect = WIFI_AP_CONNECT_FAIL;
    }
    return ret;
}

DR_WIFI_e modem_CIPSENDEX(DevWIFI_t *pWIFI, uint32_t len)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    DR_WIFI_e ret;

    wifi_send_at("AT+CIPSENDEX=%d\r", len);
    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, ">", 3);

    return ret;
}
static uint32_t wifi_IPD(DevWIFI_t *pWIFI, uint8_t *pbuff, uint32_t len)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    uint32_t recv_len = 0;
    int i = 0;
    DR_WIFI_e ret;

    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "+IPD", 1);
    if(ret == DR_WIFI_OK)
    {
        sscanf(reply, "%*[^,],%d", &recv_len);
        for(i = 0; i < WIFI_MAX_COMMAND_LEN; i++)
        {
            if(reply[i] == 0x68)
            {
                memmove(pbuff, &reply[i], recv_len);
                break;
            }
        }
    }
    return recv_len;
}

static DR_WIFI_e wifi_set_transparent(DevWIFI_t *pWIFI)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    DR_WIFI_e ret;

    wifi_send_at("AT+CIPSEND\r");
    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, ">", 3);

    return ret;
}
/** @brief �������ݣ���ʱʱ��20s
 *
 * @param pModem DevModem_t*
 * @param pbuff uint8_t*
 * @param len uint32_t
 * @return DR_MODEM_e
 *
 */
DR_WIFI_e wifi_write(DevWIFI_t *pWIFI, uint8_t *pbuff, uint32_t len)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    uint32_t n;
    DR_WIFI_e ret;

//    n = 0;
//    ret = DR_WIFI_ERROR;
//
//    ret = modem_CIPSENDEX(pWIFI, len);
//    if(ret != DR_WIFI_OK)
//    {
//        return ret;
//    }
//    esp_UART_puts(pbuff, len);
//    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "OK", 120);
//    if(ret != DR_WIFI_OK)
//    {
//        return ret;
//    }

//    return ret;

    n = wifi_UART_puts(pbuff, len);
    if(n == len)
    {
        ret = DR_WIFI_OK;
    }
    else
    {
        ret = DS_WIFI_ERR;
    }
    return ret;

}
uint32_t wifi_read(DevWIFI_t *pWIFI, uint8_t *pbuff, uint32_t len)
{
    uint32_t n;
//    return wifi_IPD(pWIFI, pbuff, len);
    n  = wifi_UART_gets(pWIFI, pbuff, len);
    return n;
}

DR_WIFI_e wifi_tcp_close(DevWIFI_t *pWIFI)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_WIFI_e ret;

    wifi_send_at("AT+CIPCLOSE\r");
    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "CLOSED", 3);

    return ret;
}

DR_WIFI_e wifi_CWJAP(DevWIFI_t *pWIFI)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_WIFI_e ret;

    wifi_send_at("AT+CWJAP=\"%s\",\"%s\"\r",
                 pWIFI->info.strSSID,
                 pWIFI->info.strPWD);

    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "OK", 75);

    return ret;
}

DR_WIFI_e wifi_CWQAP(DevWIFI_t *pWIFI)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_WIFI_e ret;

    wifi_send_at("AT+CWQAP\r");

    ret = wifi_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}

void wifi_get_info(DevWIFI_t *pWIFI)
{
//    modem_CPIN(pWifi);
//    modem_CSQ(pWifi);
//    modem_get_STATE(pWifi);
}

DR_WIFI_e wifi_init(DevWIFI_t *pWIFI)
{
    uint8_t  reply[WIFI_MAX_COMMAND_LEN + 1]  = {0};
    DR_WIFI_e ret;

    /** @todo (rgw#1#): ���ļ���ȡ���� */

    DEVDEBUG("modem init: \r\n");
    ret = wifi_disable_echo();
    if(ret != DR_WIFI_OK)
    {
        return ret;
    }
    ret = wifi_CWMODE(pWIFI);
    if(ret != DR_WIFI_OK)
    {
        return ret;
    }
    ret = wifi_CIPMODE(pWIFI);
    if(ret != DR_WIFI_OK)
    {
        return ret;
    }

    return ret;
}

DevWIFI_t *DevWifiCreate(void)
{
    DevWIFI_t *pMod;
    pMod = (DevWIFI_t *)malloc(sizeof(DevWIFI_t));
    return pMod;
}

void Wifi_Poll(DevWIFI_t *pWIFI)
{
    EventBits_t uxBits;
    DR_WIFI_e ret;
    int i;
    while(1)
    {
        switch(pWIFI->state)
        {
        case DS_WIFI_OFF:
            ret = wifi_open(pWIFI);
            if(ret != DR_WIFI_OK)
            {
                //WIFI_reset;
                //DEVDEBUG("modem Key reset!: \r\n");
                vTaskDelay(10000);
            }
            break;
        case DS_WIFI_ON:
            ret = wifi_init(pWIFI);
            if(ret == DR_WIFI_OK)
            {
                pWIFI->state = DS_WIFI_CONNECT_AP;
            }
            else
            {
                pWIFI->state = DS_WIFI_ERR;
            }
            break;
        case DS_WIFI_CONNECT_AP:
            ret = wifi_CWJAP(pWIFI);
            if(ret == DR_WIFI_OK)
            {
                pWIFI->state = DS_WIFI_TCP_OPEN;
            }
            else
            {
                pWIFI->state = DS_WIFI_ERR;
            }
            break;
        case DS_WIFI_DISCONNECT_AP:
            ret = wifi_CWQAP(pWIFI);
            if(ret == DR_WIFI_OK)
            {
                pWIFI->state = DS_WIFI_CONNECT_AP;
            }
            else
            {
                pWIFI->state = DS_WIFI_ERR;
            }
            break;
        case DS_WIFI_TCP_OPEN:
            xEventGroupClearBits(xHandleEventTCP, defEventBitTCPConnectOK);
            ret = wifi_set_tcp_open(pWIFI, pechProto);
            switch(pWIFI->status.eConnect)
            {
            case WIFI_AP_CONNECT_OK:
                ret = wifi_set_transparent(pWIFI);
                if(ret != DR_WIFI_OK)
                {
                    pWIFI->state = DS_WIFI_ERR;
                    break;
                }
                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectOK);
                pWIFI->state = DS_WIFI_TCP_KEEP;
                break;
            case WIFI_AP_CONNECT_FAIL:
                pWIFI->state = DS_WIFI_TCP_CLOSE;
                break;
            default:
                break;
            }
            break;
        case DS_WIFI_TCP_KEEP:
//            modem_get_STATE(pWifi);
//            if(pWifi->state == PDP_DEACT)
//            {
//                pWifi->state = DS_WIFI_CONNECT_AP;
//            }
//            if(pWifi->state == IP_CLOSE)
//            {
//                pWifi->state = DS_WIFI_TCP_OPEN;
//            }
            //�ȴ��������󣬴�remote����
            uxBits = xEventGroupWaitBits(xHandleEventTCP,
                                         defEventBitTCPClientSendReq,
                                         pdTRUE, pdTRUE, 0);
            if((uxBits & defEventBitTCPClientSendReq) == defEventBitTCPClientSendReq) //������Ҫ����
            {
                ret = wifi_write(pWIFI, tcp_client_sendbuf, send_len);
                if(ret == DR_WIFI_OK)
                {
                    xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientSendOK);
                }
                else if(ret == DR_WIFI_TIMEOUT)
                {

                }
                else
                {
                    xEventGroupClearBits(xHandleEventTCP, defEventBitTCPConnectOK);
                    xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectFail);
                    pWIFI->state = DS_WIFI_TCP_CLOSE;
                    printf_safe("����ʧ��\r\n");
                }
            }

            /*=== read���� ===*/
            recv_len = wifi_read(pWIFI, tcp_client_recvbuf, WIFI_MAX_COMMAND_LEN);
            if(recv_len > 0)
            {
                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientRecvValid);
            }
            uxBits = xEventGroupWaitBits(xHandleEventTCP,
                                         defEventBitTCPClientRecvValid,
                                         pdTRUE, pdTRUE, 0);
            if((uxBits & defEventBitTCPClientRecvValid) == defEventBitTCPClientRecvValid)
            {
                printf_safe("\nTCP Recv: ");
                for(i = 0; i < recv_len; i++)
                {
                    printf_safe("%02X ", tcp_client_recvbuf[i]);
                }
                printf_safe("\n");

                pechProto->recvResponse(pechProto, pEVSE, tcp_client_recvbuf, recv_len, 3);
                memset(tcp_client_recvbuf, 0, recv_len);
                recv_len = 0;
            }

            break;
        case DS_WIFI_TCP_CLOSE:
            ret = wifi_tcp_close(pWIFI);
            if(ret == DR_WIFI_OK)
            {
                pWIFI->state = DS_WIFI_TCP_OPEN;
            }
            else
            {
                pWIFI->state = DS_WIFI_ERR;
            }
            break;
        case DS_WIFI_ERR:
            //bsp_Uart_Init();
            ret = wifi_RESET(pWIFI);
            if(ret == DR_WIFI_OK)
            {
                pWIFI->state = DS_WIFI_ON;
            }
            else
            {
                pWIFI->state = DS_WIFI_OFF;
            }
            break;
        default:
            break;
        }

        wifi_get_info(pWIFI);

        vTaskDelay(500);
    }
}

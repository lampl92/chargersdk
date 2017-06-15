#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "modem.h"
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
DevModem_t *pModem;

uint8_t  tcp_client_sendbuf[TCP_CLIENT_BUFSIZE]; //TCP客户端发送数据缓冲
uint8_t  tcp_client_recvbuf[TCP_CLIENT_BUFSIZE]; //TCP客户端接收数据缓冲区

uint32_t recv_len = 0;
uint32_t send_len = 0;


static uint32_t modem_UART_puts(uint8_t *pbuff, uint32_t len)
{
    return uart_write(UART_PORT_GPRS, pbuff, len);
}

static uint32_t modem_UART_gets(DevModem_t *pModem, uint8_t *line, uint32_t len)
{
    uint32_t   cnt  = 0;
//    if(xSemaphoreTake(pModem->xMutex, 10000) == pdTRUE)
//    {
    cnt = uart_read(UART_PORT_GPRS, line, len, 0);
//        xSemaphoreGive(pModem->xMutex);
    return cnt;
//    }
//    else
    {
        //没等到，离开了
    }
    return cnt;
}
static uint32_t modem_send_at(uint8_t *format, ...)
{
    uint8_t     cmd[MAX_COMMAND_LEN + 1]  = {0};
    va_list  va;
    uint32_t      n;


    va_start(va, format);
    n  = vsnprintf(cmd, sizeof(cmd) - 1, format, va);
    va_end(va);

//    cmd[strlen(cmd)] = '\n';
    modem_UART_puts(cmd, strlen(cmd));

    cmd[strlen(cmd) - 1]  = '\0';
    DEVDEBUG(("%s\r\n", cmd));

    return n;
}


/**
 * modem_get_at_reply
 *
 * @param      reply   The reply
 * @param      len     The length
 * @param      key     关键词
 * @param      second  延时时间
 *
 * @return     { description_of_the_return_value }
 */
static DR_MODEM_e modem_get_at_reply(uint8_t *reply, uint32_t len, const uint8_t *key, uint32_t second)
{
    uint8_t  *p;
    uint32_t  time;
    uint32_t n;
    DR_MODEM_e    ret;

    time  = 0;
    while (1)
    {
        n  = modem_UART_gets(pModem, reply, len);
        if ( n > 0 )
        {
            //优先判断这两个模块会主动发出的命令
            p  = strstr(reply, "CLOSED");
            if ( p )
            {
                pModem->state = DS_MODEM_TCP_CLOSE;
                ret = DR_MODEM_CLOSED;
                break;
            }
            p  = strstr(reply, "+QIRDI:");
            if ( p )
            {
                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientRecvValid);
                ret = DR_MODEM_READ;
                break;
            }
            if(key == NULL)
            {
                ret = DR_MODEM_OK;
                break;
            }
            p  = strstr(reply, key);
            if ( p )
            {
                ret  = DR_MODEM_OK;
                break;
            }
            p  = strstr(reply, "ERROR");
            if ( p )
            {
                ret  = DR_MODEM_ERROR;
                break;
            }
            memset(reply, '\0', n);
            n  = 0;
        }

        time += 100;
        if ( time > second * 1000 )
        {
            ret  = DR_MODEM_TIMEOUT;
            break;
        }

        vTaskDelay(100);
    }

    DEVDEBUG(("%s\r\n\r\n", reply));
    return ret;
}

/** @brief 初始化modem，置Key开启模块，检测AT返回命令
 *
 * @param void
 * @return int
 *
 */
DR_MODEM_e modem_open(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    DR_MODEM_e ret;

    DEVDEBUG("modem open: \r\n");
    modem_send_at("AT\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if(ret == DR_MODEM_OK)
    {
        pModem->state = DS_MODEM_ON;
        return ret;
    }
    GPRS_set; //上电启动
    DEVDEBUG("modem Key set!: \r\n");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "Ready", 20);
    switch(ret)
    {
    case DR_MODEM_OK:
        modem_send_at("AT\r");
        ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
        pModem->state = DS_MODEM_ON;
        break;
    case DR_MODEM_ERROR:
    case DR_MODEM_TIMEOUT:
        break;
    }

    return ret;
}

/** @brief 关闭回显
 *
 * @param void
 * @return int
 *
 */
static DR_MODEM_e modem_disable_echo(void)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("ATE0V1\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}


/** @brief PIN检测
 *
 * @param void
 * @return int
 *
 */
static DR_MODEM_e modem_CPIN(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+CPIN?\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+CPIN:", 3);
    switch(ret)
    {
    case DR_MODEM_OK:
        if(strstr(reply, "READY") != NULL)
        {
            pModem->status.eSimStat = CPIN_READY;
        }
        else
        {
            pModem->status.eSimStat = CPIN_OTHER;
        }
        break;
    default:
        ret = DR_MODEM_ERROR;
        break;
    }
    return ret;
}


/** @brief 信号强度检测
 *
 * @param void
 * @return uint8_t 返回信号强度  0-31，值越大信号越好；99，无信号或不可检测
 *
 */
static DR_MODEM_e modem_CSQ(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+CSQ\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+CSQ:", 3);
    switch(ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*[^ ] %[0-9]", s);
        pModem->status.ucSignalQuality = atoi(s);
        break;
    }
    return ret;
}

/** @brief 网络状态检测
 *
 * @param void
 * @return uint8_t
 *
 */
static DR_MODEM_e modem_get_net_reg(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+CREG?\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+CREG:", 3);
    switch(ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*[^,],%[0-9]", s);
        switch(atoi(s))
        {
        case 1:
            pModem->status.eNetReg = REG_LOCAl;
            break;
        case 2:
            pModem->status.eNetReg = REG_SEARCH;
            break;
        case 5:
            pModem->status.eNetReg = REG_ROAMING;
            break;
        default:
            pModem->status.eNetReg = REG_UNKNOWN;
            break;
        }
        break;
    }

    return ret;
}

/** @brief
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
static DR_MODEM_e modem_get_gprs_reg(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+CGREG?\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+CGREG:", 3);
    switch(ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*[^,],%[0-9]", s);
        switch(atoi(s))
        {
        case 1:
            pModem->status.eGprsReg = REG_LOCAl;
            break;
        case 2:
            pModem->status.eGprsReg = REG_SEARCH;
            break;
        case 5:
            pModem->status.eGprsReg = REG_ROAMING;
            break;
        default:
            pModem->status.eGprsReg = REG_UNKNOWN;
            break;
        }
        break;
    }

    return ret;
}

/** @brief 配置前置场景
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e modem_set_context(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QIFGCNT=%d\r", pModem->info.ucContext);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}

DR_MODEM_e modem_QIMODE(DevModem_t *pModem, uint8_t tpmode)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QIMODE=%d\r", tpmode);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}
DR_MODEM_e modem_QITCFG(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QITCFG=%d,%d,%d,%d\r", 3, 2, 512, 1);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}

/** @brief 设置透传模式参数
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e modem_set_Transparent(DevModem_t *pModem, uint8_t tpmode)
{
    DR_MODEM_e ret;
    ret = modem_QIMODE(pModem, tpmode);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = modem_QITCFG(pModem);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    return ret;
}

/** @brief 设置TCP/UDP接收显示方式
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e modem_set_RecvType(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QISHOWRA=%d\r", 0);  //在接收到的数据头位置增加数据来源的地址和端口号。具体的格式为：RECV FROM:<IP ADDRESS>:<PORT>
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    modem_send_at("AT+QISHOWPT=%d\r", 0); //在接收到的数据之前增加传输层的协议类型，TCP或者UDP。这个应用不是很多
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    modem_send_at("AT+QIHEAD=%d\r", 0);  //在接收到的数据之前增加头信息"IPD<len>:"
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    modem_send_at("AT+QINDI=%d\r", 1);
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    return ret;
}

/** @brief 执行APN
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e modem_set_QIREGAPP(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QIREGAPP\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}

/** @brief 激活移动场景，发起GPRS连接
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e modem_set_QIACT(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QIACT\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 150);

    return ret;
}
/** @brief 关闭GPRS连接
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e modem_set_QIDEACT(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QIDEACT\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 150);

    return ret;
}
/** @brief 获取locIP
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e modem_get_LOCIP(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    uint8_t *p;
    DR_MODEM_e ret;

    modem_send_at("AT+QILOCIP\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, NULL, 3);
    if(ret == DR_MODEM_OK)
    {
        sscanf(reply, "%s", pModem->status.strLocIP);
    }
    return ret;
}

DR_MODEM_e modem_set_TCPOPEN(DevModem_t *pModem, echProtocol_t *pProto)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QIOPEN=\"TCP\",\"%s\",\"%d\"\r",
                  pProto->info.strServerIP,
                  pProto->info.usServerPort);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "CONNECT", 75);
    if(ret == DR_MODEM_OK)
    {
        sscanf(reply, "%*s%s", s);
        if(strcmp(s, "OK") == 0) //"CONNECT OK"
        {
            pModem->status.eConnect = CONNECT_OK;
        }
        else if(strcmp(s, "FAIL") == 0) //"CONNECT FAIL"
        {
            pModem->status.eConnect = CONNECT_FAIL;
        }
        else if(strcmp(s, "CONNECT") == 0)  //"ALREADY CONNECT"
        {
            pModem->status.eConnect = CONNECT_OK;
        }
    }
    return ret;
}
DR_MODEM_e modem_QISACK(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QISACK\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QISACK:", 3);
    if(ret == DR_MODEM_OK)
    {
        sscanf(reply, "%*s%d,%d,%d", &(pModem->flag.sent), &(pModem->flag.acked), &(pModem->flag.nAcked));
    }
    return ret;
}
DR_MODEM_e modem_QISEND(DevModem_t *pModem, uint32_t len)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QISEND=%d\r", len);
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, ">", 3);

    return ret;
}
static uint32_t modem_QIRD(DevModem_t *pModem, uint8_t *pbuff, uint32_t len)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint32_t recv_len = 0;
    int i = 0;
    DR_MODEM_e ret;

    modem_send_at("AT+QIRD=%d,%d,%d,%d\r", 0, 1, 0, len);
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QIRD:", 1);
    switch(ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*s %*[^,],%*[^,],%d", &recv_len);
        for (i = 0; i < MAX_COMMAND_LEN; i++)
        {
            if (reply[i] == 0x68)
            {
                memmove(pbuff, &reply[i], recv_len);
                break;
            }
        }
        break;
    case DR_MODEM_TIMEOUT:
        printf_safe("Read 超时！！\n");
        break;
    case DR_MODEM_ERROR:
        printf_safe("Read 错误！！\n");
        break;
    case DR_MODEM_READ:
        printf_safe("Read READ!!！！\n");
        break;
    case DR_MODEM_CLOSED:
        printf_safe("Read 时发现Close\n");
        break;
    }

    return recv_len;
}
/** @brief 发送数据，超时时间20s
 *
 * @param pModem DevModem_t*
 * @param pbuff uint8_t*
 * @param len uint32_t
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e modem_write(DevModem_t *pModem, uint8_t *pbuff, uint32_t len)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint32_t n;
    DR_MODEM_e ret;

    n = 0;
    ret = DR_MODEM_ERROR;

    do
    {
        ret = modem_QISACK(pModem);
        if(ret == DR_MODEM_READ)
        {
            return ret;
        }
        if(pModem->flag.sent == pModem->flag.acked)
        {
            ret = DR_MODEM_OK;
            break;
        }
        n++;
        if(n >= 240)
        {
            ret = DR_MODEM_TIMEOUT;
            break;
        }
        vTaskDelay(500);
    }
    while(pModem->flag.sent != pModem->flag.acked);

    if(ret == DR_MODEM_OK)
    {
        ret = modem_QISEND(pModem, len);
        if(ret != DR_MODEM_OK)
        {
            return ret;
        }
        taskENTER_CRITICAL();
        modem_UART_puts(pbuff, len);
        taskEXIT_CRITICAL();
        ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
        if(ret != DR_MODEM_OK)
        {
            ret = DR_MODEM_TIMEOUT;
        }
    }

    return ret;
}
uint32_t modem_read(DevModem_t *pModem, uint8_t *pbuff, uint32_t len)
{
    return modem_QIRD(pModem, pbuff, len);
}

DR_MODEM_e modem_QICLOSE(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QICLOSE\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}

DR_MODEM_e modem_RESET(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+CFUN=%d,%d\r", 1, 1);
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "Ready", 20);

    return ret;
}
/** @brief 获取接入STATE  M26命令集 P156
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e modem_get_STATE(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[16 + 1]  = {0};
    DR_MODEM_e ret;

    modem_send_at("AT+QISTATE\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "STATE", 3);
    if(ret == DR_MODEM_OK)
    {
        sscanf(reply, "%*s%*s%s", s);///** @todo (rgw#1#): 查看一下内存中真实数据 */
        if(strcmp(s, "INITIAL") == 0)
        {
            pModem->status.statConStat = IP_INITIAL;
        }
        if(strcmp(s, "START") == 0)
        {
            pModem->status.statConStat = IP_START;
        }
        if(strcmp(s, "CONFIG") == 0)
        {
            pModem->status.statConStat = IP_CONFIG;
        }
        if(strcmp(s, "IND") == 0)
        {
            pModem->status.statConStat = IP_IND;
        }
        if(strcmp(s, "GPRSACT") == 0)
        {
            pModem->status.statConStat = IP_GPRSACT;
        }
        if(strcmp(s, "STATUS") == 0)
        {
            pModem->status.statConStat = IP_STATUS;
        }
        if(strcmp(s, "CONNECTING") == 0)
        {
            pModem->status.statConStat = TCP_CONNECTING;
        }
        if(strcmp(s, "CLOSE") == 0)
        {
            pModem->status.statConStat = IP_CLOSE;
        }
        if(strcmp(s, "OK") == 0)
        {
            pModem->status.statConStat = TCP_CONNECT_OK;
        }
        if(strcmp(s, "DEACT") == 0)
        {
            pModem->status.statConStat = PDP_DEACT;
        }
    }
    return ret;
}

DR_MODEM_e modem_set_PDP(DevModem_t *pModem)
{
    DR_MODEM_e ret;
    ret = modem_set_QIREGAPP(pModem);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = modem_set_QIACT(pModem);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = modem_get_LOCIP(pModem);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    return ret;
}

DR_MODEM_e modem_get_info(DevModem_t *pModem)
{
    DR_MODEM_e ret;

    do
    {
        ret = modem_CPIN(pModem);
        if(ret != DR_MODEM_OK)
        {
            return ret;
        }
        vTaskDelay(1000);
    }
    while(pModem->status.eSimStat != CPIN_READY);
    do
    {
        ret = modem_get_net_reg(pModem);
        if(ret != DR_MODEM_OK)
        {
            return ret;
        }
        vTaskDelay(1000);
    }
    while(pModem->status.eNetReg == REG_SEARCH );

    do
    {
        ret = modem_get_gprs_reg(pModem);
        if(ret != DR_MODEM_OK)
        {
            return ret;
        }
        vTaskDelay(1000);
    }
    while(pModem->status.eGprsReg == REG_SEARCH );
    do
    {
        ret = modem_CSQ(pModem);
        if(ret != DR_MODEM_OK)
        {
            return ret;
        }
        vTaskDelay(1000);
    }
    while(pModem->status.ucSignalQuality < 5 ||
            pModem->status.ucSignalQuality >= 99);

    return ret;
    //modem_get_STATE(pModem);
}

DR_MODEM_e modem_init(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    DR_MODEM_e ret;

    /** @todo (rgw#1#): 从文件获取配置 */

    DEVDEBUG("modem init: \r\n");
    ret = modem_disable_echo();
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }

    ret = modem_get_info(pModem);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }

    ret = modem_set_context(pModem);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = modem_set_RecvType(pModem);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = modem_set_Transparent(pModem, 0);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    return ret;
}

DevModem_t *DevModemCreate(void)
{
    DevModem_t *pMod;
    pMod = (DevModem_t *)malloc(sizeof(DevModem_t));
    return pMod;
}

void Modem_Poll(DevModem_t *pModem)
{
    EventBits_t uxBits;
    DR_MODEM_e ret;
    int i;
    while(1)
    {
        switch(pModem->state)
        {
        case DS_MODEM_OFF:
            ret = modem_open(pModem);
            if(ret != DR_MODEM_OK)
            {
                GPRS_reset;
                DEVDEBUG("modem Key reset!: \r\n");
                vTaskDelay(10000);
            }
            break;
        case DS_MODEM_ON:
            ret = modem_init(pModem);
            if(ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_TCP_ACT_PDP;
            }
            else
            {
                pModem->state = DS_MODEM_ERR;
            }
            break;
        case DS_MODEM_TCP_ACT_PDP:
            ret = modem_set_PDP(pModem);
            if(ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_TCP_OPEN;
            }
            else
            {
                pModem->state = DS_MODEM_ERR;
            }
            break;
        case DS_MODEM_TCP_DEACT_PDP:
            ret = modem_set_QIDEACT(pModem);
            if(ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_TCP_ACT_PDP;
            }
            else
            {
                pModem->state = DS_MODEM_ERR;
            }
            break;
        case DS_MODEM_TCP_OPEN:
            ret = modem_set_TCPOPEN(pModem, pechProto);
            switch(pModem->status.eConnect)
            {
            case CONNECT_OK:
                xEventGroupClearBits(xHandleEventTCP, defEventBitTCPConnectFail); //rgw OK
                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectOK); //rgw OK
                pModem->state = DS_MODEM_TCP_KEEP;
                break;
            case CONNECT_FAIL:
                modem_get_STATE(pModem);
                if(pModem->status.statConStat == TCP_CONNECTING)
                {
                    pModem->state = DS_MODEM_TCP_CLOSE;
                    printf_safe("State TCP open Fail ,Call TCP close!!\n");
                }
                else
                {
                    pModem->state = DS_MODEM_TCP_DEACT_PDP;
                }
                break;
            default:
                break;
            }
            break;
        case DS_MODEM_TCP_KEEP: //临时注释，不要删
//            modem_get_STATE(pModem);
//            if(pModem->state == PDP_DEACT)
//            {
//                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectFail);
//                pModem->state = DS_MODEM_TCP_ACT_PDP;
//            }
//            if(pModem->state == IP_CLOSE)
//            {
//                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectFail);
//                pModem->state = DS_MODEM_TCP_OPEN;
//            }
            //等待remote发送请求
            uxBits = xEventGroupWaitBits(xHandleEventTCP,
                                         defEventBitTCPClientSendReq,
                                         pdTRUE, pdTRUE, 0);
            if((uxBits & defEventBitTCPClientSendReq) == defEventBitTCPClientSendReq) //有数据要发送
            {
                ret = modem_write(pModem, tcp_client_sendbuf, send_len);
                if(ret == DR_MODEM_OK)
                {
                    printf_safe("\nTCP Send: ");
                    for(i = 0; i < send_len; i++)
                    {
                        printf_safe("%02X ", tcp_client_sendbuf[i]);
                    }
                    printf_safe("\n");
                    xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientSendOK);
                }
                else if(ret == DR_MODEM_READ)
                {
                    xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientSendOK);
// 别删             xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientRecvValid); //DR_MODEM_READ处已经发送读事件，此处作为提示用途。
                }
                else if(ret == DR_MODEM_TIMEOUT)
                {

                }
                else
                {
                    pModem->state = DS_MODEM_TCP_CLOSE;
                    printf_safe("发送失败\r\n");
                }
            }

            /*=== read处理 ===*/
            ret = modem_get_at_reply(tcp_client_recvbuf, sizeof(tcp_client_recvbuf) - 1, "+QIRDI:", 1);
            if(ret == DR_MODEM_READ)
            {
                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientRecvValid);
            }
            uxBits = xEventGroupWaitBits(xHandleEventTCP,
                                         defEventBitTCPClientRecvValid,
                                         pdTRUE, pdTRUE, 0);
            if((uxBits & defEventBitTCPClientRecvValid) == defEventBitTCPClientRecvValid)
            {
                //读取串口数据
                recv_len = modem_read(pModem, tcp_client_recvbuf, MAX_COMMAND_LEN);
                if(recv_len > 0)
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
            }

            break;
        case DS_MODEM_TCP_CLOSE:
            xEventGroupSetBits(xHandleEventTCP, defEventBitTCPConnectFail); //rgw OK
            xEventGroupClearBits(xHandleEventTCP, defEventBitTCPConnectOK); //rgw OK
            ret = modem_QICLOSE(pModem);
            if(ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_TCP_OPEN;
            }
            else
            {
                pModem->state = DS_MODEM_TCP_DEACT_PDP;
            }
            break;
        case DS_MODEM_ERR:
            bsp_Uart_Init();
            ret = modem_RESET(pModem);
            if(ret == DR_MODEM_OK)
            {
                pModem->state = DS_MODEM_ON;
            }
            else
            {
                pModem->state = DS_MODEM_OFF;
            }
            break;
        default:
            break;
        }

//        uxBits = xEventGroupWaitBits(xHandleEventTCP,
//                                     defEventBitTCPClientFlushBuff,
//                                     pdTRUE, pdTRUE, 0); //定时请一次缓存
//        if((uxBits & defEventBitTCPClientFlushBuff) == defEventBitTCPClientFlushBuff)
//        {
//            //printf_safe("看看是不是定时清缓存这里出问题了\n");
//            recv_len = modem_read(pModem, tcp_client_recvbuf, MAX_COMMAND_LEN);
//            if(recv_len > 0)
//            {
//                printf_safe("\nTCP Recv: ");
//                for(i = 0; i < recv_len; i++)
//                {
//                    printf_safe("%02X ", tcp_client_recvbuf[i]);
//                }
//                printf_safe("\n");
//
//                pechProto->recvResponse(pechProto, pEVSE, tcp_client_recvbuf, recv_len, 3);
//                memset(tcp_client_recvbuf, 0, recv_len);
//                recv_len = 0;
//            }
//        }

        //modem_get_info(pModem);

        vTaskDelay(500);
    }
}

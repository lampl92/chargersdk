#include "modem.h"
#include "retarget.h"
#include "user_app.h"
#include <stdlib.h>
#include <string.h>

DR_MODEM_e M26_quit(void *pvModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("++++++");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "\r\n", 3);

    return ret;
}

/** @brief 关闭回显
 *
 * @param void
 * @return int
 *
 */
static DR_MODEM_e M26_disable_echo(void)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("ATE0V1\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}
static DR_MODEM_e M26_ATI(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("ATI\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "Q", 3);
    printf_safe("%s", reply);
    switch (ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*[^ ] %[0-9]", s);
        pModem->status.ucSignalQuality = atoi(s);
        break;
    default:
        break;
    }
    return ret;
}
//运行商
static DR_MODEM_e M26_AT_COPS(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+COPS?\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+COPS", 3);
    //printf_safe("%s", reply);
    switch (ret)
    {
    case DR_MODEM_OK:
        if (strstr(reply, "CHINA MOBILE") != NULL)
        {
            //printf_safe("%s", reply);
        }
        break;
    default:
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
static DR_MODEM_e M26_CSQ(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+CSQ\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+CSQ:", 3);
    switch (ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*[^ ] %[0-9]", s);
        pModem->status.ucSignalQuality = atoi(s);
        break;
    default:
        break;
    }
    return ret;
}

static DR_MODEM_e M26_CPIN(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+CPIN?\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+", 3);
    switch (ret)
    {
    case DR_MODEM_OK:
        if (strstr(reply, "READY") != NULL)
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

/** @brief 网络状态检测 get net reg
 *
 * @param void
 * @return uint8_t
 *
 */
static DR_MODEM_e M26_CREG(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+CREG?\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+CREG:", 3);
    switch (ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*[^,],%[0-9]", s);
        switch (atoi(s))
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
    default:
        break;
    }

    return ret;
}

/** @brief get gprs reg
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
static DR_MODEM_e M26_CGREG(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+CGREG?\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+CGREG:", 3);
    switch (ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*[^,],%[0-9]", s);
        switch (atoi(s))
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
    default:
        break;
    }

    return ret;
}

DR_MODEM_e M26_AT_CGDCONT(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+CGDCONT=%d,\"%s\",\"%s\"\r", 1, "IP", pModem->info.strAPN);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}

DR_MODEM_e M26_ATD(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("ATD*99#\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "CONNECT", 75);

    return ret;
}

/** @brief 配置前置场景 modem_set_context
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e M26_QIFGCNT(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QIFGCNT=%d\r", pModem->info.ucContext);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}
/** @brief 执行APN
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e M26_AT_QIREGAPP(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QIREGAPP\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}

DR_MODEM_e M26_QIACT(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QIACT\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 150);

    return ret;
}

DR_MODEM_e M26_QIDEACT(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
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
DR_MODEM_e M26_LOCIP(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    uint8_t *p;
    DR_MODEM_e ret;

    modem_send_at("AT+QILOCIP\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, NULL, 3);
    if (ret == DR_MODEM_OK)
    {
        sscanf(reply, "%s", pModem->status.strLocIP);
    }
    return ret;
}



DR_MODEM_e M26_TCPOPEN(DevModem_t *pModem, char *server_ip, uint16_t port)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QIOPEN=\"TCP\",\"%s\",\"%d\"\r", server_ip, port);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "CONNECT", 75);
    if (ret == DR_MODEM_OK)
    {
        if (pModem->info.ucTPMode == 0)
        {
            sscanf(reply, "%*s%s", s);
            if (strcmp(s, "OK") == 0) //"CONNECT OK"
            {
                pModem->status.eConnect = CONNECT_OK;
            }
            else if (strcmp(s, "FAIL") == 0) //"CONNECT FAIL"
            {
                pModem->status.eConnect = CONNECT_FAIL;
            }
            else if (strcmp(s, "CONNECT") == 0)  //"ALREADY CONNECT"
            {
                pModem->status.eConnect = CONNECT_OK;
            }
        }
        else
        {
            sscanf(reply, "%s", s);
            if (strcmp(s, "CONNECT") == 0) //"CONNECT OK"
            {
                pModem->status.eConnect = CONNECT_OK;
            }
            sscanf(reply, "%*s%s", s);
            if (strcmp(s, "FAIL") == 0) //"CONNECT FAIL"
            {
                pModem->status.eConnect = CONNECT_FAIL;
            }
            else if (strcmp(s, "CONNECT") == 0)  //"ALREADY CONNECT"
            {
                pModem->status.eConnect = CONNECT_OK;
            }
        }
    }
    else if (ret == DR_MODEM_ERROR)
    {
        pModem->status.eConnect = CONNECT_FAIL;
    }
    return ret;
}
DR_MODEM_e M26_QISACK(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QISACK\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QISACK:", 3);
    if (ret == DR_MODEM_OK)
    {
        sscanf(reply, "%*s%d,%d,%d", &(pModem->flag.sent), &(pModem->flag.acked), &(pModem->flag.nAcked));
    }
    return ret;
}
DR_MODEM_e M26_QISEND(DevModem_t *pModem, uint32_t len)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QISEND=%d\r", len);
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, ">", 3);

    return ret;
}
static uint32_t M26_QIRD(DevModem_t *pModem, uint8_t *pbuff, uint32_t len)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint32_t recv_len = 0;
    int i = 0;
    DR_MODEM_e ret;

    modem_send_at("AT+QIRD=%d,%d,%d,%d\r", 0, 1, 0, len);
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QIRD:", 1);
    switch (ret)
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


DR_MODEM_e M26_QICLOSE(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QICLOSE\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}
/** @brief 获取接入STATE  M26命令集 P156
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e M26_STATE(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[16 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QISTATE\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "STATE", 3);
    if (ret == DR_MODEM_OK)
    {
        sscanf(reply, "%*s%*s%*s%s", s);
        if (strcmp(s, "INITIAL") == 0)
        {
            pModem->status.statConStat = IP_INITIAL;
        }
        if (strcmp(s, "START") == 0)
        {
            pModem->status.statConStat = IP_START;
        }
        if (strcmp(s, "CONFIG") == 0)
        {
            pModem->status.statConStat = IP_CONFIG;
        }
        if (strcmp(s, "IND") == 0)
        {
            pModem->status.statConStat = IP_IND;
        }
        if (strcmp(s, "GPRSACT") == 0)
        {
            pModem->status.statConStat = IP_GPRSACT;
        }
        if (strcmp(s, "STATUS") == 0)
        {
            pModem->status.statConStat = IP_STATUS;
        }
        if (strcmp(s, "CONNECTING") == 0)
        {
            pModem->status.statConStat = TCP_CONNECTING;
        }
        if (strcmp(s, "CLOSE") == 0)
        {
            pModem->status.statConStat = IP_CLOSE;
        }
        if (strcmp(s, "OK") == 0)
        {
            pModem->status.statConStat = TCP_CONNECT_OK;
        }
        if (strcmp(s, "DEACT") == 0)
        {
            pModem->status.statConStat = PDP_DEACT;
        }
    }
    return ret;
}

DR_MODEM_e M26_QFTPUSER(DevModem_t *pModem, char *user)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QFTPUSER=\"%s\"\r", user);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}
DR_MODEM_e M26_QFTPPASS(DevModem_t *pModem, char *pass)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QFTPPASS=\"%s\"\r", pass);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}
DR_MODEM_e M26_QFTPOPEN(DevModem_t *pModem, char *server_ip, uint16_t port)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;
    uint32_t a;
    
    modem_send_at("AT+QFTPOPEN=\"%s\",%d\r", server_ip, port);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QFTPOPEN", 20);
    switch (ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*[^:]:%d", &a);
        if (a == 0)
        {
            ret = DR_MODEM_OK;
        }
        else
        {
            ret = DR_MODEM_ERROR;
        }
        break;
    default:
        break;
    }
    return ret;
}
DR_MODEM_e M26_QFTPPATH(DevModem_t *pModem, char *path)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;
    uint32_t a;
    
    modem_send_at("AT+QFTPPATH=\"/%s/\"\r", path);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QFTPPATH", 20);
    switch (ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "+QFTPPATH:%d", &a);
        if (a == 0)
        {
            ret = DR_MODEM_OK;
        }
        else
        {
            ret = DR_MODEM_ERROR;
        }
        break;
    default:
        break;
    }
    return ret;
}

DR_MODEM_e M26_QFTPGET(DevModem_t *pModem, char *fname)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QFTPGET=\"%s\"\r", fname);

    ret = modem_get_at_reply(reply, 11, "CONNECT", 20);

    return ret;
}

DR_MODEM_e M26_QFTPCLOSE(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QFTPCLOSE\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QFTPCLOSE", 20);

    return ret;
}

/** @brief 设置TCP/UDP接收显示方式
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e M26_set_RecvType(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QISHOWRA=%d\r", 0);  //在接收到的数据头位置增加数据来源的地址和端口号。具体的格式为：RECV FROM:<IP ADDRESS>:<PORT>
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    modem_send_at("AT+QISHOWPT=%d\r", 0); //在接收到的数据之前增加传输层的协议类型，TCP或者UDP。这个应用不是很多
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    modem_send_at("AT+QIHEAD=%d\r", 0);  //在接收到的数据之前增加头信息"IPD<len>:"
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    modem_send_at("AT+QINDI=%d\r", 1);
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    return ret;
}


DR_MODEM_e modem_QIMODE(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QIMODE=%d\r", pModem->info.ucTPMode);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}
DR_MODEM_e modem_QITCFG(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
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
DR_MODEM_e M26_set_Transparent(DevModem_t *pModem)
{
    DR_MODEM_e ret;
    ret = modem_QIMODE(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    if (pModem->info.ucTPMode == 1)
    {
        ret = modem_QITCFG(pModem);
        if (ret != DR_MODEM_OK)
        {
            return ret;
        }
    }

    return ret;
}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

DR_MODEM_e M26_set(void *pvModem)
{
    GPRS_set;
    return DR_MODEM_OK;
}
DR_MODEM_e M26_reset(void *pvModem)
{
    GPRS_reset;
    return DR_MODEM_OK;
}

DR_MODEM_e M26_soft_reset(void *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+CFUN=%d,%d\r", 1, 1);
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "R", 10);//Ready/RDY

    return ret;
}
/** @brief 初始化modem，置Key开启模块，检测AT返回命令
 *
 * @param void
 * @return int
 *
 */
DR_MODEM_e M26_open(void *pvModem)
{
    DevModem_t *pModem;
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;
    
    pModem = (DevModem_t*)pvModem;

    printf_modem("M26 open: \r\n");
    modem_send_at("AT\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if (ret == DR_MODEM_OK)
    {
        pModem->state = DS_MODEM_ON;
        return ret;
    }
    else
    {
        ret = M26_quit(pModem);
        if (ret == DR_MODEM_OK)
        {
            pModem->state = DS_MODEM_ON;
            return ret;
        }
    }
    M26_set(pModem); //上电启动
    printf_modem("M26 Key set!: \r\n");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "R", 10);//Ready/RDY
    switch (ret)
    {
    case DR_MODEM_OK:
        modem_send_at("AT\r");
        ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
        pModem->state = DS_MODEM_ON;
        break;
    case DR_MODEM_ERROR:
    case DR_MODEM_TIMEOUT:
        break;
    default:
        break;
    }

    return ret;
}

/**
 * @fn  DR_MODEM_e M26_init(DevModem_t *pModem)
 *
 * @brief   26 initialize
 *
 * @param [in,out]  pModem  If non-null, the modem.
 *
 * @return  A DR_MODEM_e.
 */

DR_MODEM_e M26_init(void *pvModem)
{
    DevModem_t *pModem;
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;
    
    pModem = (DevModem_t*)pvModem;

    printf_modem("modem init: \r\n");
    ret = M26_disable_echo();
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    do
    {
        ret = M26_CPIN(pModem);
        if (ret == DR_MODEM_OK)
        {
            if (pModem->status.eSimStat == CPIN_READY)
                break;
        }
        else
        {
            return ret;
        }
        modem_delayms(1000);
    } while (1);
#if 0
    do
    {
        ret = M26_CREG(pModem);
        if (ret == DR_MODEM_OK)
        {
            if (pModem->status.eNetReg == REG_LOCAl || pModem->status.eNetReg == REG_ROAMING)
                break;
        }
        else
        {
            return ret;
        }
        modem_delayms(1000);
    }
    while(1);

    do
    {
        ret = M26_CGREG(pModem);
        if (ret == DR_MODEM_OK)
        {
            if (pModem->status.eGprsReg == REG_LOCAl || pModem->status.eGprsReg == REG_ROAMING)
                break;
        }
        else
        {
            return ret;
        }
        modem_delayms(1000);
    }
    while(1);
#endif
    do
    {
        ret = M26_CSQ(pModem);
        if (ret == DR_MODEM_OK)
        {
            if (pModem->status.ucSignalQuality > 5 &&
                pModem->status.ucSignalQuality < 99)
                break;
        }
        else
        {
            return ret;
        }
        modem_delayms(1000);
    } while (1);
 
    return ret;
}

DR_MODEM_e M26_diag_PPP(void *pModem)
{
    DR_MODEM_e ret;
    ret = M26_AT_CGDCONT(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = M26_ATD(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    return ret;
}
DR_MODEM_e M26_act_PDP(void *pModem)
{
    DR_MODEM_e ret;
    ret = M26_QIFGCNT(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = M26_set_Transparent(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = M26_AT_QIREGAPP(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = M26_QIACT(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = M26_LOCIP(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
       
    return ret;
}

DR_MODEM_e M26_deact_PDP(void *pModem)
{
    DR_MODEM_e ret;
    ret = M26_QIDEACT(pModem);
    return ret;
}
DR_MODEM_e M26_open_TCP(void *pModem, char *server_ip, uint16_t port)
{
    DR_MODEM_e ret;
    ret = M26_TCPOPEN(pModem, server_ip, port);
    return ret;
}
DR_MODEM_e M26_close_TCP(void *pModem)
{
    DR_MODEM_e ret;
    ret = M26_QICLOSE(pModem);
    return ret;
}

DR_MODEM_e M26_open_FTP(void *pModem, char *server_ip, uint16_t port, char *user, char *pass)
{
    DR_MODEM_e ret;
    
    M26_QFTPUSER(pModem, user);
    M26_QFTPPASS(pModem, pass);
    ret = M26_QFTPOPEN(pModem, server_ip, port);
    if (ret != DR_MODEM_OK)
        return ret;
    
    return ret;
}

DR_MODEM_e M26_set_ftp_path(void *pModem, char *path)
{
    DR_MODEM_e ret;
    ret = M26_QFTPPATH(pModem, path);
    if (ret != DR_MODEM_OK)
        return ret;
    return ret;
}
DR_MODEM_e M26_ftp_get(void *pModem, char *fname)
{
    DR_MODEM_e ret;
    ret = M26_QFTPGET(pModem, fname);
    if (ret != DR_MODEM_OK)
        return ret;
    return ret;
}
DR_MODEM_e M26_close_FTP(void *pModem)
{
    DR_MODEM_e ret;
    ret = M26_QFTPCLOSE(pModem);
    return ret;
}
DevModem_t *M26Create(void)
{
    DevModem_t *pMod;
    pMod = (DevModem_t *)malloc(sizeof(DevModem_t));
    memset(pMod, 0, sizeof(DevModem_t));
    sprintf(pMod->info.strAPN, "CMNET");
    pMod->info.ucContext = 0;
    pMod->info.ucTPMode = 1;
    pMod->status.ucSignalQuality = 0;
    pMod->state = DS_MODEM_OFF;
    pMod->xMutex = xSemaphoreCreateMutex();
    pMod->pSendQue = QueueCreate(MAX_COMMAND_LEN);
    
    pMod->set = M26_set;
    pMod->reset = M26_reset;
    pMod->open = M26_open;
    pMod->init = M26_init;
    pMod->act_PDP = M26_act_PDP;
    pMod->deact_PDP = M26_deact_PDP;
    pMod->open_TCP = M26_open_TCP;
    pMod->close_TCP = M26_close_TCP;
    pMod->open_FTP = M26_open_FTP;
    pMod->set_ftp_path = M26_set_ftp_path;
    pMod->ftp_get = M26_ftp_get;
    pMod->close_FTP = M26_close_FTP;
    pMod->soft_reset = M26_soft_reset;
    
    return pMod;
}

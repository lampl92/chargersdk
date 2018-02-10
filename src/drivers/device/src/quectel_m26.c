#include "modem.h"

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


DR_MODEM_e M26_TCPOPEN(DevModem_t *pModem, echProtocol_t *pProto)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QIOPEN=\"TCP\",\"%s\",\"%d\"\r",
        pProto->info.strServerIP,
        pProto->info.usServerPort);

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

DR_MODEM_e M26_QFTPGET(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QFTPGET=\"%s\"\r", pechProto->info.ftp.strNewFileName);

    ret = modem_get_at_reply(reply, 11, "CONNECT", 20);

    return ret;
}
DR_MODEM_e M26_QFTPUSER(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QFTPUSER=\"%s\"\r", pechProto->info.ftp.strUser);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}
DR_MODEM_e M26_QFTPPASS(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QFTPPASS=\"%s\"\r", pechProto->info.ftp.strPassword);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}
DR_MODEM_e M26_QFTPOPEN(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;
    uint32_t a;
    
    modem_send_at("AT+QFTPOPEN=\"%s\",%d\r", pechProto->info.ftp.strServer, pechProto->info.ftp.usPort);

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
DR_MODEM_e M26_QFTPPATH(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;
    uint32_t a;
    
    modem_send_at("AT+QFTPPATH=\"/%s/\"\r", pechProto->info.ftp.strNewVersion);

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


DR_MODEM_e M26_QFTPCLOSE(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QFTPCLOSE\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QFTPCLOSE", 20);

    return ret;
}
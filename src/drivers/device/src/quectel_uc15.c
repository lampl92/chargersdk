#include "modem.h"
/** @brief 关闭GPRS连接
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e UC15_QIDEACT(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QIDEACT=%d\r", 1);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 150);

    return ret;
}
/** @brief 激活移动场景，发起GPRS连接
 *
 * @param pModem DevModem_t*
 * @return DR_MODEM_e
 *
 */
DR_MODEM_e UC15_QIACT(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QIACT=%d\r", 1);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 150);

    return ret;
}

DR_MODEM_e UC15_LOCIP(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    uint8_t *p;
    DR_MODEM_e ret;

    modem_send_at("AT+QIACT?\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QIACT:", 3);
    if (ret == DR_MODEM_OK)
    {
        sscanf(reply, "%*s %*d,%*d,%*d,%*[\"]%[^\"]", pModem->status.strLocIP);
    }
    return ret;
}
DR_MODEM_e UC15_TCPOPEN(DevModem_t *pModem, echProtocol_t *pProto)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QIOPEN=%d,%d,\"%s\",\"%s\",%d,%d,%d\r",
        pModem->info.ucContext,
        0,
        "TCP",
        pProto->info.strServerIP,
        pProto->info.usServerPort,
        0,
        2);

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
    else
    {
        pModem->status.eConnect = CONNECT_FAIL;
    }
    return ret;
}
DR_MODEM_e UC15_QICLOSE(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QICLOSE=%d\r", 0);
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 10);

    return ret;
}

DR_MODEM_e UC15_QFTPCFG(DevModem_t *pModem, char *cmd, char *usr, char *pass, uint8_t param)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;
    if (usr != NULL)
    {
        modem_send_at("AT+QFTPCFG=\"%s\",\"%s\",\"%s\"\r", cmd, usr, pass);
    }
    else
    {
        modem_send_at("AT+QFTPCFG=\"%s\",%d\r", cmd, param);
    }

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}

DR_MODEM_e UC15_QFTPOPEN(DevModem_t *pModem, char *server, uint16_t port)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;
    uint32_t a, b;

    modem_send_at("AT+QFTPOPEN=\"%s\",%d\r", server, port);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QFTPOPEN", 20);
    switch (ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*s %d,%d", &a, &b);
        if (a == 0 && b == 0)
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
DR_MODEM_e UC15_QFTPCWD(DevModem_t *pModem, char *path)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;
    uint32_t a, b;

    modem_send_at("AT+QFTPCWD=\"/%s/\"\r", path);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QFTPCWD", 10);
    switch (ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*s %d,%d", &a, &b);
        if (a == 0 && b == 0)
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
DR_MODEM_e UC15_QFTPGET(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QFTPGET=\"%s\",\"%s\"\r", pechProto->info.ftp.strNewFileName, "COM:");

    ret = modem_get_at_reply(reply, 11, "CONNECT", 20);

    return ret;
}
DR_MODEM_e UC15_QFTPCLOSE(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QFTPCLOSE\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+QFTPCLOSE", 20);

    return ret;
}
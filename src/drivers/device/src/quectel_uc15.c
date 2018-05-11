#include "modem.h"
#include "retarget.h"
#include "user_app.h"
#include <stdlib.h>
#include <string.h>

DR_MODEM_e UC15_quit(DevModem_t *pModem)
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
static DR_MODEM_e UC15_disable_echo(void)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("ATE0V1\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}


/** @brief 信号强度检测
 *
 * @param void
 * @return uint8_t 返回信号强度  0-31，值越大信号越好；99，无信号或不可检测
 *
 */
static DR_MODEM_e UC15_CSQ(DevModem_t *pModem)
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
/** @brief PIN检测
 *
 * @param void
 * @return int
 *
 */
static DR_MODEM_e UC15_CPIN(DevModem_t *pModem)
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
static DR_MODEM_e UC15_CREG(DevModem_t *pModem)
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
static DR_MODEM_e UC15_CGREG(DevModem_t *pModem)
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

DR_MODEM_e UC15_AT_CGDCONT(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+CGDCONT=%d,\"%s\",\"%s\"\r", 1, "IP", pModem->info.strAPN);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}

DR_MODEM_e UC15_ATD(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("ATD*99#\r");

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "CONNECT", 75);

    return ret;
}


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
DR_MODEM_e UC15_TCPOPEN(DevModem_t *pModem, char *server_ip, uint16_t port)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QIOPEN=%d,%d,\"%s\",\"%s\",%d,%d,%d\r",
        pModem->info.ucContext,
        0,
        "TCP",
        server_ip,
        port,
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

DR_MODEM_e UC15_QFTPOPEN(DevModem_t *pModem, char *server_ip, uint16_t port)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;
    uint32_t a, b;

    modem_send_at("AT+QFTPOPEN=\"%s\",%d\r", server_ip, port);

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
DR_MODEM_e UC15_QFTPGET(DevModem_t *pModem, char *fname)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    uint8_t  s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+QFTPGET=\"%s\",\"%s\"\r", fname, "COM:");

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


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
DR_MODEM_e UC15_set(void *pModem)
{
    GPRS_set;
    return DR_MODEM_OK;
}
DR_MODEM_e UC15_reset(void *pModem)
{
    GPRS_reset;
    return DR_MODEM_OK;
}
DR_MODEM_e UC15_soft_reset(void *pModem)
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
DR_MODEM_e UC15_open(void *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;

    printf_modem("modem open: \r\n");
    modem_send_at("AT\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
    if (ret == DR_MODEM_OK)
    {
        ((DevModem_t*)pModem)->state = DS_MODEM_ON;
        return ret;
    }
    else
    {
        ret = UC15_quit(pModem);
        if (ret == DR_MODEM_OK)
        {
            ((DevModem_t*)pModem)->state = DS_MODEM_ON;
            return ret;
        }
    }
    UC15_set(pModem); //上电启动
    printf_modem("modem Key set!: \r\n");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "R", 10);//Ready/RDY
    switch (ret)
    {
    case DR_MODEM_OK:
        modem_send_at("AT\r");
        ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);
        ((DevModem_t*)pModem)->state = DS_MODEM_ON;
        break;
    case DR_MODEM_ERROR:
    case DR_MODEM_TIMEOUT:
        break;
    default:
        break;
    }

    return ret;
}
DR_MODEM_e UC15_init(void *pvModem)
{
    DevModem_t *pModem;
    uint8_t  reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;
    int timeoutMax = 20;
    int timeout = 0;
    
    pModem = (DevModem_t*)pvModem;

    printf_modem("UC15 init: \r\n");
    ret = UC15_disable_echo();
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }

    //get info
    timeout = 0;
    do
    {
        timeout++;
        if (timeout > timeoutMax)
        {
            timeout = 0;
            return DR_MODEM_TIMEOUT;
        }
        ret = UC15_CPIN(pModem);
        if (ret != DR_MODEM_OK)
        {
            return ret;
        }
        modem_delayms(1000);
    } while (pModem->status.eSimStat != CPIN_READY);
#if 1
    timeout = 0;
    do
    {
        timeout++;
        if (timeout > timeoutMax)
        {
            timeout = 0;
            return DR_MODEM_TIMEOUT;
        }
        ret = UC15_CREG(pModem);
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
    } while (1);

    timeout = 0;
    do
    {
        timeout++;
        if (timeout > timeoutMax)
        {
            timeout = 0;
            return DR_MODEM_TIMEOUT;
        }
        ret = UC15_CGREG(pModem);
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
    } while (1);
#endif
    timeout = 0;
    do
    {
        timeout++;
        if (timeout > timeoutMax)
        {
            timeout = 0;
            return DR_MODEM_TIMEOUT;
        }
        ret = UC15_CSQ(pModem);
        if (ret != DR_MODEM_OK)
        {
            return ret;
        }
        modem_delayms(1000);
    } while (pModem->status.ucSignalQuality < 5 ||
            pModem->status.ucSignalQuality >= 99);
   
    return ret;
}


DR_MODEM_e UC15_diag_PPP(void *pModem)
{
    DR_MODEM_e ret;
    ret = UC15_AT_CGDCONT(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = UC15_ATD(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    return ret;
}


DR_MODEM_e UC15_act_PDP(void *pModem)
{
    DR_MODEM_e ret;
   
    ret = UC15_QIACT(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = UC15_LOCIP(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    
    return ret;
}

DR_MODEM_e UC15_deact_PDP(void *pModem)
{
    DR_MODEM_e ret;
    ret = UC15_QIDEACT(pModem);
    return ret;
}

DR_MODEM_e UC15_open_TCP(void *pModem, char *server_ip, uint16_t port)
{
    DR_MODEM_e ret;
    ret = UC15_TCPOPEN(pModem, server_ip, port);
    return ret;
}

DR_MODEM_e UC15_close_TCP(void *pModem)
{
    DR_MODEM_e ret;
    ret = UC15_QICLOSE(pModem);
    return ret;
}

DR_MODEM_e UC15_open_FTP(void *pModem, char *server_ip, uint16_t port, char *user, char *pass)
{
    DR_MODEM_e ret;
    
    UC15_QFTPCFG(pModem, "contextid", NULL, NULL, 1);
    UC15_QFTPCFG(pModem, "account", user, pass, 0);
    UC15_QFTPCFG(pModem, "filetype", NULL, NULL, 1);
    UC15_QFTPCFG(pModem, "transmode", NULL, NULL, 1);
    UC15_QFTPCFG(pModem, "rsptimeout", NULL, NULL, 90);
    ret = UC15_QFTPOPEN(pModem, server_ip, port);
    if (ret != DR_MODEM_OK)
        return ret;

    return ret;
}
DR_MODEM_e UC15_set_ftp_path(void *pModem, char *path)
{
    DR_MODEM_e ret;
    ret = UC15_QFTPCWD(pModem, path);
    if (ret != DR_MODEM_OK)
        return ret;
    return ret;
}
DR_MODEM_e UC15_ftp_get(void *pModem, char *fname)
{
    DR_MODEM_e ret;
    ret = UC15_QFTPGET(pModem, fname);
    if (ret != DR_MODEM_OK)
        return ret;
    return ret;
}
DR_MODEM_e UC15_close_FTP(void *pModem)
{
    DR_MODEM_e ret;
    ret = UC15_QFTPCLOSE(pModem);
    return ret;
}
DevModem_t *UC15Create(void)
{
    DevModem_t *pMod;
    pMod = (DevModem_t *)malloc(sizeof(DevModem_t));
    memset(pMod, 0, sizeof(DevModem_t));
    sprintf(pModem->info.strAPN, "3GNET");
    pMod->info.ucContext = 1;
    pMod->info.ucTPMode = 1;
    pMod->status.ucSignalQuality = 0;
    pMod->state = DS_MODEM_OFF;
    pMod->xMutex = xSemaphoreCreateMutex();
    pMod->pSendQue = QueueCreate(MAX_COMMAND_LEN);
    
    pMod->open = UC15_open;
    pMod->set = UC15_set;
    pMod->reset = UC15_reset;
    pMod->init = UC15_init;
    pMod->diag_PPP = UC15_diag_PPP;
    pMod->act_PDP = UC15_act_PDP;
    pMod->deact_PDP = UC15_deact_PDP;
    pMod->open_TCP = UC15_open_TCP;
    pMod->close_TCP = UC15_close_TCP;
    pMod->open_FTP = UC15_open_FTP;
    pMod->set_ftp_path = UC15_set_ftp_path;
    pMod->ftp_get = UC15_ftp_get;
    pMod->close_FTP = UC15_close_FTP;
    pMod->soft_reset = UC15_soft_reset;
    
    return pMod;
}

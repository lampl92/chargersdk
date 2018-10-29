#include "modem.h"
#include "retarget.h"
#include "user_app.h"
#include "bsp_uart.h"
#include <stdlib.h>
#include <string.h>
#include "ppp/ppp.h"

DR_MODEM_e Quectel_quit(void *pvModem)
{
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
    char s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("+++ATH\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "\r\n", 3);

    return ret;
}

/** @brief 关闭回显
 *
 * @param void
 * @return int
 *
 */
static DR_MODEM_e Quectel_disable_echo(void)
{
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("ATE0V1\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}
static DR_MODEM_e Quectel_ATI(DevModem_t *pModem)
{
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
    char s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("ATI\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, NULL, 3);
    printf_safe("%s", reply);
    switch (ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%s%s", pModem->info.strManufacturer, pModem->info.strDeviceModule);
        break;
    default:
        break;
    }
    return ret;
}
//ICCID
static DR_MODEM_e Quectel_AT_QCCID(DevModem_t *pModem)
{
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
    char s[8 + 1] = { 0 };
    DR_MODEM_e ret;
    char *p;

    modem_send_at("AT+QCCID\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "89", 3);
    printf_safe("%s", reply);
    switch (ret)
    {
    case DR_MODEM_OK:
        p  = strstr(reply, "89");
        sscanf(p, "%20s", pModem->info.strICCID);
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
static DR_MODEM_e Quectel_CSQ(DevModem_t *pModem)
{
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
    char s[8 + 1] = { 0 };
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

static DR_MODEM_e Quectel_CPIN(DevModem_t *pModem)
{
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
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
static DR_MODEM_e Quectel_CREG(DevModem_t *pModem)
{
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
    char s[8 + 1] = { 0 };
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
static DR_MODEM_e Quectel_CGREG(DevModem_t *pModem)
{
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
    char s[8 + 1] = { 0 };
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

DR_MODEM_e Quectel_AT_CGDCONT(DevModem_t *pModem)
{
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
    char s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+CGDCONT=%d,\"%s\",\"%s\"\r", 1, "IP", pModem->info.strAPN);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}

DR_MODEM_e Quectel_ATD(DevModem_t *pModem)
{
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
    char s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("ATD%s\r", pModem->info.strATD);

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "CONNECT", 75);

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

DR_MODEM_e Quectel_keyon(void *pvModem)
{
    printf_modem("Quectel Key on!: \r\n");
    GPRS_set;
    modem_delayms(1100); // >1s
    GPRS_reset;
    return DR_MODEM_OK;
}
DR_MODEM_e Quectel_keyoff(void *pvModem)
{
    printf_modem("Quectel Key off!: \r\n");
    GPRS_set;
    modem_delayms(800); //0.7s < keyoff < 1s
    GPRS_reset;
    modem_delayms(8000); // 2s to 12s
    return DR_MODEM_OK;
}

DR_MODEM_e Quectel_soft_reset(void *pModem)
{
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
    char s[8 + 1] = { 0 };
    DR_MODEM_e ret;

    modem_send_at("AT+CFUN=%d,%d\r", 1, 1);
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "R", 10); //Ready/RDY

    return ret;
}
/** @brief 初始化modem，置Key开启模块，检测AT返回命令
 *
 * @param void
 * @return int
 *
 */
DR_MODEM_e Quectel_open(void *pvModem)
{
    DevModem_t *pModem;
    char reply[MAX_COMMAND_LEN + 1] = { 0 };
    DR_MODEM_e ret;

    int timeout = 0;
    int timeoutMax = 10;

    GPRS_reset;

    pModem = (DevModem_t*)pvModem;

    printf_modem("Quectel open: \r\n");
    modem_send_at("AT\r");
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 1);
    if (ret == DR_MODEM_OK)
    {
        return ret;
    }
    else
    {
        ret = Quectel_quit(pModem);
        if (ret == DR_MODEM_OK)
        {
            return ret;
        }
    }
    Quectel_keyon(pModem);  //上电启动
    timeout = 0;
    while (ret != DR_MODEM_OK)
    {
        timeout++;
        if (timeout > timeoutMax)
        {
            ret = DR_MODEM_TIMEOUT;
            return ret;
        }
        modem_send_at("AT\r");
        ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 1);
    }

    return ret;
}

/**
 * @fn  DR_MODEM_e Quectel_init(DevModem_t *pModem)
 *
 * @brief   26 initialize
 *
 * @param [in,out]  pModem  If non-null, the modem.
 *
 * @return  A DR_MODEM_e.
 */

DR_MODEM_e Quectel_init(void *pvModem)
{
    DevModem_t *pModem;
    DR_MODEM_e ret;
    int timeoutMax = 20;
    int timeout = 0;

    pModem = (DevModem_t*)pvModem;
    GPRS_reset;
    printf_modem("Quectel init: \r\n");
    ret = Quectel_disable_echo();
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    timeout = 0;
    Quectel_ATI(pModem);

    do
    {
        timeout++;
        if (timeout > timeoutMax)
        {
            timeout = 0;
            return DR_MODEM_TIMEOUT;
        }
        ret = Quectel_AT_QCCID(pModem);
        if (ret == DR_MODEM_OK)
        {
            break;
        }
        modem_delayms(1000);
    } while (1);

    //中国移动
    //898600MFSSYYGXXXXXXP
    //89： 国际编号
    //86： 国家编号，86：中国
    //00： 运营商编号，00：中国移动
    //中国联通
    //898601YY8SSXXXXXXXXP
    //89： 国际编号
    //86： 国家编号，86：中国
    //01： 运营商编号，01：中国联通
    //中国电信
    //898603YYXMHHHXXXXXXP
    //89：国际编号
    //86：国家编号，86：中国
    //03：运营商编号，03：中国电信
    switch(pModem->info.strICCID[5])
    {
    case '0'://中国移动
        printf_modem("中国移动\n");
        break;
    case '1'://中国联通
        printf_modem("中国联通\n");
        break;
    case '3'://中国电信
        printf_modem("中国电信\n");
        break;
    default:
        break;
    }
    if(strstr(pModem->info.strDeviceModule, "M26") != NULL)
    {
        switch (pModem->info.strICCID[5])
        {
        case '0'://中国移动
        case '1'://中国联通
            break;
        case '3'://中国电信
            while(1)
            {
#if EVSE_USING_GUI
                LCD_ShowString(400, 460, 380, 20, 16, "DianXin Card Invalid");
#endif
                printf_modem("DianXin Card Invalid\n");
                vTaskDelay(1000);
            }
            break;
        default:
            while (1)
            {
#if EVSE_USING_GUI
                LCD_ShowString(400, 460, 380, 20, 16, "Card Invalid");
#endif
                printf_modem("Card Invalid\n");
                vTaskDelay(1000);
            }
            break;
        }
    }
    else if(strstr(pModem->info.strDeviceModule, "UC15") != NULL)
    {
        pModem->info.pppAuthProto = PPP_AUTH_PROTOCOL_PAP;
        switch (pModem->info.strICCID[5])
        {
        case '0'://中国移动
            break;
        case '1'://中国联通
            sprintf(pModem->info.strAPN, "UNINET");
            break;
        case '3'://中国电信
            while (1)
            {
#if EVSE_USING_GUI
                LCD_ShowString(400, 460, 380, 20, 16, "DianXin Card Invalid");
#endif
                printf_modem("DianXin Card Invalid\n");
                vTaskDelay(1000);
            }
            break;
        default:
            while (1)
            {
#if EVSE_USING_GUI
                LCD_ShowString(400, 460, 380, 20, 16, "Card Invalid");
#endif
                printf_modem("Card Invalid\n");
                vTaskDelay(1000);
            }
            break;
        }
    }
    else if(strstr(pModem->info.strDeviceModule, "EC20") != NULL)
    {
        switch (pModem->info.strICCID[5])
        {
        case '0': //中国移动
        case '1': //中国联通
            sprintf(pModem->info.pppAuthUser, "test");
            sprintf(pModem->info.pppAuthPass, "test");
            break;
        case '3': //中国电信
            memset(pModem->info.strAPN, 0, sizeof(pModem->info.strAPN));
            sprintf(pModem->info.strATD, "#777");
            sprintf(pModem->info.pppAuthUser, "ctnet@mycdma.cn");
            sprintf(pModem->info.pppAuthPass, "vnet.mobi");
            break;
        default:
            while (1)
            {
#if EVSE_USING_GUI
                LCD_ShowString(400, 460, 380, 20, 16, "Card Invalid");
#endif
                printf_modem("Card Invalid\n");
                vTaskDelay(1000);
            }
            break;
        }
    }

    timeout = 0;
    do
    {
        timeout++;
        if (timeout > timeoutMax)
        {
            timeout = 0;
            return DR_MODEM_TIMEOUT;
        }
        ret = Quectel_CPIN(pModem);
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
    timeout = 0;
    do
    {
        timeout++;
        if (timeout > timeoutMax)
        {
            timeout = 0;
            return DR_MODEM_TIMEOUT;
        }
        ret = Quectel_CREG(pModem);
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
        ret = Quectel_CGREG(pModem);
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
        ret = Quectel_CSQ(pModem);
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

DR_MODEM_e Quectel_diag_PPP(void *pModem)
{
    DR_MODEM_e ret;
    ret = Quectel_AT_CGDCONT(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = Quectel_ATD(pModem);
    if (ret != DR_MODEM_OK)
    {
        return ret;
    }
    return ret;
}

void QuectelGetDefaultSettings(DevModem_t *pModem)
{
    sprintf(pModem->info.strAPN, "CMNET");
    sprintf(pModem->info.strATD, "*99#");
    pModem->info.pppACCM = 0x00000000;
    pModem->info.pppAuthProto = PPP_AUTH_PROTOCOL_PAP | PPP_AUTH_PROTOCOL_CHAP_MD5;
    pModem->state = DS_MODEM_OFF;
}
DevModem_t *QuectelCreate(void)
{
    DevModem_t *pMod;
    pMod = (DevModem_t *)malloc(sizeof(DevModem_t));
    memset(pMod, 0, sizeof(DevModem_t));
    QuectelGetDefaultSettings(pMod);
    pMod->uart_handle = uart_open(MODEM_UARTx, MODEM_UART_BAND, MODEM_UART_DATA, MODEM_UART_PARI, MODEM_UART_STOP);
    if (pMod->uart_handle < 0)
    {
        printf_safe("Quectel %s 初始化失败，code = %d！\n", MODEM_UARTx, pMod->uart_handle);
    }

    pMod->keyon = Quectel_keyon;
    pMod->keyoff = Quectel_keyoff;
    pMod->open = Quectel_open;
    pMod->init = Quectel_init;
    pMod->diag_PPP = Quectel_diag_PPP;
    pMod->soft_reset = Quectel_soft_reset;

    return pMod;
}

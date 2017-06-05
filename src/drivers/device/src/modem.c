#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "modem.h"
#include "bsp_uart.h"
#include "user_app.h"

#define MODEMDEBUG(msg) do{printf_safe(msg)}while(0);

static int get_line(uint8_t *line, int len);

static uint8_t UART_getc(void)
{
    uint8_t ch = 0;
    uart_read(UART_PORT_GPRS, &ch, 1, 0);
    return ch;
}

static void UART_puts(uint8_t *cmd)
{
    uart_write(UART_PORT_GPRS, cmd, strlen(cmd));
}

/** @brief
 *
 * @param line uint8_t*
 * @param len uint32_t
 * @return uint32_t
 *
 */
static  uint32_t get_line(uint8_t *line, uint32_t len)
{
    uint8_t  c;
    uint32_t   cnt  = 0;
    int   i;

    for (i = 0; i < len; i++)
    {
        c  = UART_getc();
        if ( c == '\0' )
        {
            break;
        }

        if ( c == '\r' )
        {
            c  = UART_getc();
            {
                break;
            }
        }

        line[cnt++]  = c;
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

    UART_puts(cmd);

    cmd[strlen(cmd) - 1]  = '\0';
    MODEMDEBUG(("send at: [%s].\r\n", cmd));

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
    int    time;
    int    n;
    int    sockfd;
    int    stat;
    DR_MODEM_e    ret;


    time  = 0;
    while (1)
    {
        n  = get_line(reply, len);
        if ( n > 0 )
        {
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

//            p  = strstr(reply, "MIPSTAT:");
//            if ( p )
//            {
//                if ( (sscanf(reply, "+MIPSTAT: %d,%d", &sockfd, &stat) == 2)
//                        && (sockfd == 1) && (stat == 1) )
//                {
//                    ret  = -2;
//                    break;
//                }
//            }

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

    MODEMDEBUG(("wait at return: [%s].\r\n\r\n", reply));
    return ret;
}


/** @brief 初始化modem，置Key开启模块，检测AT返回命令
 *
 * @param void
 * @return int
 *
 */
static DR_MODEM_e modem_open(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    DR_MODEM_e ret;

    MODEMDEBUG(("modem open: \r\n"));
    GPRS_set; //上电启动
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "RDY", 5);
    switch(ret)
    {
    case DR_MODEM_OK:
        modem_send_at(("AT\r"));
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

    MODEMDEBUG(("modem disable echo: \r\n"));
    modem_send_at(("ATE0V1\r"));
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "OK", 3);

    return ret;
}


/** @brief PIN检测
 *
 * @param void
 * @return int
 *
 */
static DR_MODEM_e modem_get_simcard_stat(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    DR_MODEM_e ret;

    MODEMDEBUG(("modem get simcard stat: \r\n"));
    modem_send_at(("AT+CPIN?\r"));
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+CPIN:", 3);
    switch(ret)
    {
    case DR_MODEM_OK:
        if(strstr(reply, "READY") != NULL)
        {
            pModem->status.ucSimStat = DR_MODEM_CPIN_READY;
            ret = DR_MODEM_CPIN_READY;
        }
        else
        {
            pModem->status.ucSimStat = DR_MODEM_CPIN_OTHER;
            ret = DR_MODEM_CPIN_OTHER;
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
 * @return uint8_t 返回信号强度  0-99 值越小信号越好
 *
 */
static DR_MODEM_e modem_get_signal_quality(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    uint8_t  s[8 + 1]  = {0};
    DR_MODEM_e ret;


    MODEMDEBUG(("modem get aerial signal: \r\n"));
    modem_send_at(("AT+CSQ\r"));
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

    MODEMDEBUG(("modem get network stat: \r\n"));
    modem_send_at(("AT+CREG?\r"));
    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+CREG:", 3);
    switch(ret)
    {
    case DR_MODEM_OK:
        sscanf(reply, "%*[^,],%[0-9]", s);
        pModem->status.ucNetReg = atoi(s);
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

    MODEMDEBUG(("modem get gprs stat: \r\n"));
    modem_send_at(("AT+CGREG?\r"));

    ret = modem_get_at_reply(reply, sizeof(reply) - 1, "+CGREG:", 3);
    switch(ret)
    {
        case DR_MODEM_OK:
            sscanf(reply, "%*[^,],%[0-9]", s);
            pModem->status.ucGprsReg = atoi(s);
            break;
    }

    return ret;
}

void modem_get_info(DevModem_t *pModem)
{
    MODEMDEBUG(("modem get informations: \r\n"));

    modem_get_simcard_stat(pModem);
    modem_get_signal_quality(pModem);
    modem_get_net_reg(pModem);
    modem_get_gprs_reg(pModem);
}


DR_MODEM_e modem_init(DevModem_t *pModem)
{
    uint8_t  reply[MAX_COMMAND_LEN + 1]  = {0};
    DR_MODEM_e ret;

    MODEMDEBUG(("modem init: \r\n"));
    ret = modem_open(pModem);
    if(ret != DR_MODEM_OK)
    {
        return ret;
    }
    ret = modem_disable_echo();

    return ret;
}

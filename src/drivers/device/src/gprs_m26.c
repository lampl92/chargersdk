/**
* @file gprs_m26.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-27
*/
#include "bsp.h"
#include "gprs_m26.h"

#define GPRS_IO     PBout(3)

dev_gprs_t dev_gprs;

void gprs_delayms(uint32_t ms)
{
    vTaskDelay(ms);
}
/** @brief 不可重入,线程不安全,注意使用. 将IO进行保护后则线程安全
 *
 * @param
 * @param
 * @return
 *
 */

uint32_t gprs_ioctl(uint8_t ioctl)
{
    uint32_t res = 0;
    uint32_t fn_res = 0;
    uint8_t try_cont = 0;
    switch(ioctl)
    {
    case DA_GPRS_ON:
        GPRS_IO = 1;
        gprs_delayms(2000);
        GPRS_IO = 0;
        do
        {
            try_cont++;
            if(try_cont > 10)
            {
                try_cont = 0;
                fn_res = 0;
                dev_gprs.state = DS_GPRS_ERR;
                break;
            }
            res = recvStrCmp(pGprsRecvQue, "RDY", 0);
            if(res == 0)
            {
                gprs_delayms(1000);
            }
        }
        while(res == 0);
        if(try_cont != 0)
        {
            fn_res = 1;
            gprs_delayms(5000);
            dev_gprs.state = DS_GPRS_ON;
        }
        break;
    case DA_GPRS_OFF:
        GPRS_IO = 1;
        gprs_delayms(1000);
        GPRS_IO = 0;
        do
        {
            try_cont++;
            if(try_cont > 10)
            {
                try_cont = 0;
                fn_res = 0;
                dev_gprs.state = DS_GPRS_ERR;
                break;
            }
            res = recvStrCmp(pGprsRecvQue, "NORMAL POWER DOWN", 0);
            if(res == 0)
            {
                gprs_delayms(1000);
            }
        }
        while(res == 0);
        if(try_cont != 0)
        {
            fn_res = 1;
            gprs_delayms(15000);
            dev_gprs.state = DS_GPRS_OFF;
        }
        break;
    case DA_GPRS_RESET:
        do
        {
            GPRS_IO = 1;
            try_cont++;
            if(try_cont > 10)
            {
                try_cont = 0;
                fn_res = 0;
                dev_gprs.state = DS_GPRS_ERR;
                break;
            }
            res = recvStrCmp(pGprsRecvQue, "RDY", 0);
            if(res == 0)
            {
                gprs_delayms(3000);
            }
        }
        while(res == 0);
        if(try_cont != 0)
        {
            GPRS_IO = 0;
            fn_res = 1;
            gprs_delayms(5000);
            dev_gprs.state = DS_GPRS_ON;
        }
        break;
    }
    return fn_res;
}

uint32_t gprs_init(void)
{
    uint8_t state;
    uint32_t res;

    GPRS_IO = 0;
    pGprsRecvQue->Flush(pGprsRecvQue);
    gprs_ioctl(DA_GPRS_RESET);
    if(dev_gprs.state == DS_GPRS_ON)
    {
        dev_gprs.pollstate = DS_GPRS_POLL_AT;
    }
    else
    {
        ThrowErrorCode(defDevID_GPRS, ERR_GPRS_FAULT, ERR_LEVEL_WARNING, "初始化失败");
    }
}
uint32_t gprs_read_AT(void);
uint32_t gprs_send_AT(void)
{
    pGprsRecvQue->Flush(pGprsRecvQue);
    HAL_UART_Transmit(&GPRS_UARTx_Handler, "AT\r\n", strlen("AT\r\n"), 0xFFFF);
    gprs_delayms(100);

    return gprs_read_AT();
}

uint32_t gprs_read_AT(void)
{
    uint8_t buff[20];
    uint8_t *p = NULL;
    uint32_t rl;//read len;
    uint8_t res;

    res = readRecvQueEx(pGprsRecvQue, buff, 0, &rl);
    p = strstr(buff, "OK");
    if(p != NULL)
    {
        return DR_AT_OK;
    }
    else
    {
        return DR_AT_ERR;
    }
}

uint32_t gprs_read_ATE0V1(void);
uint32_t gprs_send_ATE0V1(void)
{
    pGprsRecvQue->Flush(pGprsRecvQue);
    HAL_UART_Transmit(&GPRS_UARTx_Handler, "ATE0V1\r\n", strlen("ATE0V1\r\n"), 0xFFFF);
    gprs_delayms(100);
    return gprs_read_ATE0V1();
}

uint32_t gprs_read_ATE0V1(void)
{
    uint8_t *p;
    uint8_t buff[20];
    uint32_t rl;//read len;
    uint8_t res;

    res = readRecvQueEx(pGprsRecvQue, buff, 0, &rl);
    p = strstr(buff, "OK");
    if(p != NULL)
    {
        return DR_AT_OK;
    }
    else
    {
        return DR_AT_ERR;
    }
}
uint32_t gprs_read_CPIN(void);
uint32_t gprs_send_CPIN(void)
{
    pGprsRecvQue->Flush(pGprsRecvQue);
    HAL_UART_Transmit(&GPRS_UARTx_Handler, "AT+CPIN?\r\n", strlen("AT+CPIN?\r\n"), 0xFFFF);
    gprs_delayms(100);
    return gprs_read_CPIN();
}

uint32_t gprs_read_CPIN(void)
{
    uint8_t *p;
    uint8_t buff[20];
    uint32_t rl;//read len;
    uint8_t res;

    res = readRecvQueEx(pGprsRecvQue, buff, 0, &rl);
    p = strstr(buff, "+CPIN: READY");
    if(p != NULL)
    {
        return DR_AT_OK;
    }
    else
    {
        return DR_AT_ERR;
    }
}
uint32_t gprs_read_CREG(void);
uint32_t gprs_send_CREG(void)
{
    pGprsRecvQue->Flush(pGprsRecvQue);
    HAL_UART_Transmit(&GPRS_UARTx_Handler, "AT+CREG?\r\n", strlen("AT+CREG?\r\n"), 0xFFFF);
    gprs_delayms(100);
    return gprs_read_CREG();
}

uint32_t gprs_read_CREG(void)
{
    uint8_t *p;
    uint8_t buff[20];
    uint32_t rl;//read len;
    uint8_t res;

    res = readRecvQueEx(pGprsRecvQue, buff, 0, &rl);
    p = strstr(buff, "+CREG: 0,1");
    if(p != NULL)
    {
        return DR_AT_OK;
    }
    else
    {
        p = strstr(buff, "+CREG");
        if(p != NULL)
        {
            gprs_delayms(1000);
        }
        return DR_AT_ERR;
    }
}
uint32_t gprs_read_CGREG(void);
uint32_t gprs_send_CGREG(void)
{
    pGprsRecvQue->Flush(pGprsRecvQue);
    HAL_UART_Transmit(&GPRS_UARTx_Handler, "AT+CGREG?\r\n", strlen("AT+CGREG?\r\n"), 0xFFFF);
    gprs_delayms(100);
    return gprs_read_CGREG();
}

uint32_t gprs_read_CGREG(void)
{
    uint8_t *p;
    uint8_t buff[20];
    uint32_t rl;//read len;
    uint8_t res;

    res = readRecvQueEx(pGprsRecvQue, buff, 0, &rl);
    p = strstr(buff, "+CGREG: 0,1");
    if(p != NULL)
    {
        return DR_AT_OK;
    }
    else
    {
        p = strstr(buff, "+CGREG:");
        if(p != NULL)
        {
            gprs_delayms(1000);
        }
        return DR_AT_ERR;
    }
}
uint32_t gprs_read_APN(void);
uint32_t gprs_send_APN(void)
{
    pGprsRecvQue->Flush(pGprsRecvQue);
    HAL_UART_Transmit(&GPRS_UARTx_Handler, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", strlen("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n"), 0xFFFF);
    gprs_delayms(100);
    return gprs_read_APN();
}

uint32_t gprs_read_APN(void)
{
    uint8_t *p;
    uint8_t buff[20];
    uint32_t rl;//read len;
    uint8_t res;

    res = readRecvQueEx(pGprsRecvQue, buff, 0, &rl);
    p = strstr(buff, "OK");
    if(p != NULL)
    {
        return DR_AT_OK;
    }
    else
    {
        return DR_AT_ERR;
    }
}
uint32_t gprs_read_ATD(void);
uint32_t gprs_send_ATD(void)
{
    pGprsRecvQue->Flush(pGprsRecvQue);
    HAL_UART_Transmit(&GPRS_UARTx_Handler, "ATD*99#\r\n", strlen("ATD*99#\r\n"), 0xFFFF);
    gprs_delayms(100);
    return gprs_read_ATD();

}
uint32_t gprs_read_ATD(void)
{
    uint8_t *p;
    uint8_t buff[20];
    uint32_t rl;//read len;
    uint8_t res;

    res = readRecvQueEx(pGprsRecvQue, buff, 0, &rl);
    p = strstr(buff, "CONNECT");
    if(p != NULL)
    {
        return DR_AT_OK;
    }
    else
    {
        return DR_AT_ERR;
    }
}

uint32_t gprs_ppp_poll(void)
{
    uint32_t res_at;
    uint32_t try_cont = 0;
    while(1)
    {
        switch(dev_gprs.pollstate)
        {
        case DS_GPRS_POLL_AT:
            try_cont++;
            res_at = gprs_send_AT();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
                    try_cont = 0;
                    dev_gprs.pollstate = DS_GPRS_POLL_ERR;
                }
                else
                {
                    dev_gprs.pollstate = DS_GPRS_POLL_AT;
                }
            }
            else
            {
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_ATE0V1;
            }
            break;
        case DS_GPRS_POLL_ATE0V1:
            try_cont++;
            res_at = gprs_send_ATE0V1();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
                    try_cont = 0;
                    dev_gprs.pollstate = DS_GPRS_POLL_ERR;
                    break;
                }
                else
                {
                    dev_gprs.pollstate = DS_GPRS_POLL_ATE0V1;
                }
            }
            else
            {
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_CPIN;
            }
            break;
        case DS_GPRS_POLL_CPIN:
            try_cont++;
            res_at = gprs_send_CPIN();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
                    try_cont = 0;
                    dev_gprs.pollstate = DS_GPRS_POLL_ERR;
                }
                else
                {
                    dev_gprs.pollstate = DS_GPRS_POLL_CPIN;
                }
            }
            else
            {
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_CREG;
            }
            break;
        case DS_GPRS_POLL_CREG:
            try_cont++;
            res_at = gprs_send_CREG();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {

                    dev_gprs.pollstate = DS_GPRS_POLL_ERR;
                }
                else
                {
                    dev_gprs.pollstate = DS_GPRS_POLL_CREG;
                }
            }
            else
            {
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_CGREG;
            }
            break;

        case DS_GPRS_POLL_CGREG:
            try_cont++;
            res_at = gprs_send_CGREG();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
                    try_cont = 0;
                    dev_gprs.pollstate = DS_GPRS_POLL_ERR;
                }
                else
                {
                    dev_gprs.pollstate = DS_GPRS_POLL_CGREG;
                }
            }
            else
            {
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_APN;
            }
            break;
        case DS_GPRS_POLL_APN:
            try_cont++;
            res_at = gprs_send_APN();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
                    try_cont = 0;
                    dev_gprs.pollstate = DS_GPRS_POLL_ERR;
                }
                else
                {
                    dev_gprs.pollstate = DS_GPRS_POLL_APN;
                }
            }
            else
            {
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_ATD;
            }
            break;
        case DS_GPRS_POLL_ATD:
            try_cont++;
            res_at = gprs_send_ATD();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
                    try_cont = 0;
                    dev_gprs.pollstate = DS_GPRS_POLL_ERR;
                }
                else
                {
                    dev_gprs.pollstate = DS_GPRS_POLL_ATD;
                }
            }
            else
            {
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_PPP;
            }
            break;
        case DS_GPRS_POLL_PPP:
            xEventGroupSetBits(xHandleEventlwIP, defEventBitDailCONNECT);
            dev_gprs.pollstate = DS_GPRS_POLL_PPPDego;
            break;
        case DS_GPRS_POLL_PPPDego:
                break;
        case DS_GPRS_POLL_ERR:
            break;
        }
        vTaskDelay(100);
    }
}
#if self_test_gprs
uint8_t test_state = DA_GPRS_RESET;
uint8_t test_gprs()
{
    uint32_t fn_res;
    pGprsRecvQue->Flush(pGprsRecvQue);
    switch(test_state)
    {
    case DA_GPRS_ON:
        printf_safe("test on START:     ");
        fn_res = gprs_ioctl(DA_GPRS_ON);
        if(fn_res == 1)
        {
            printf_safe("OK\n");
            test_state = DA_GPRS_RESET;
        }
        else
        {
            printf_safe("ERR\n");
        }
        break;
    case DA_GPRS_RESET:
        printf_safe("test reset START:  ");
        fn_res = gprs_ioctl(DA_GPRS_RESET);
        if(fn_res == 1)
        {
            printf_safe("OK\n");
            test_state = DA_GPRS_OFF;
        }
        else
        {
            printf_safe("ERR\n");
        }
        break;
    case DA_GPRS_OFF:
        printf_safe("test off START:    ");
        fn_res = gprs_ioctl(DA_GPRS_OFF);
        if(fn_res == 1)
        {
            printf_safe("OK\n");
            test_state = DA_GPRS_ON;
        }
        else
        {
            printf_safe("ERR\n");
        }
        break;
    default:
        break;
    }
}
#endif

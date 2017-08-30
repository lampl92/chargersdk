/**
* @file gprs_m26.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-27
*/
#include "bsp.h"
#include "gprs_m26.h"

#define DEBUG_DEV_GPRS 1

#ifdef EVSE_DEBUG

#undef   GPRS_set
#undef   GPRS_reset
#define GPRS_set        PBout(3) = 1
#define GPRS_reset      PBout(3) = 0

#endif
dev_gprs_t dev_gprs;

uint32_t gprs_send_PlusPlusPlus(void);
uint32_t gprs_send_ATH(void);

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
        GPRS_set;
        gprs_delayms(2000);
        GPRS_reset;
        do
        {
#if DEBUG_DEV_GPRS
            printf_safe("GPRS 开机中 第 %d 次尝试（10次）\n", try_cont);
#endif
            try_cont++;
            if(try_cont > 10)
            {
                try_cont = 0;
                fn_res = 0;
                dev_gprs.state = DS_GPRS_ERR;
#if DEBUG_DEV_GPRS
                printf_safe("GPRS 开机失败\n");
#endif
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
#if DEBUG_DEV_GPRS
            printf_safe("GPRS 开机\n");
#endif
            dev_gprs.state = DS_GPRS_ON;
        }
        break;
    case DA_GPRS_OFF:
        GPRS_set;
        gprs_delayms(1000);
        GPRS_reset;
        do
        {
#if DEBUG_DEV_GPRS
            printf_safe("GPRS 关机中 第 %d 次尝试（10次）\n", try_cont);
#endif
            try_cont++;
            if(try_cont > 10)
            {
                try_cont = 0;
                fn_res = 0;
                dev_gprs.state = DS_GPRS_ERR;
#if DEBUG_DEV_GPRS
                printf_safe("GPRS 关机失败\n");
#endif
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
#if DEBUG_DEV_GPRS
            printf_safe("GPRS 关机\n");
#endif
        }
        break;
    case DA_GPRS_RESET:
//        GPRS_reset;
//        gprs_delayms(2000);
        do
        {
#if DEBUG_DEV_GPRS
            printf_safe("GPRS 重启中 第 %d 次尝试（10次）\n", try_cont);
#endif
            GPRS_set;
            try_cont++;
            if(try_cont > 10)
            {
                GPRS_reset;
                try_cont = 0;
                fn_res = 0;
                dev_gprs.state = DS_GPRS_ERR;
#if DEBUG_DEV_GPRS
                printf_safe("GPRS 重启失败\n");
#endif
                break;
            }
            res = recvStrCmp(pGprsRecvQue, "RDY", 0);
            if(res == 0)
            {
                gprs_delayms(3000);
                //bsp_DelayMS(3000);
            }
        }
        while(res == 0);
        if(try_cont != 0)
        {
            GPRS_reset;
            fn_res = 1;
            gprs_delayms(5000);
#if DEBUG_DEV_GPRS
            printf_safe("GPRS 重启成功\n");
#endif
            dev_gprs.state = DS_GPRS_ON;
        }
        break;
    }
    return fn_res;
}
uint32_t gprs_send_AT(void);
/** @brief
 *
 * @param void
 * @return uint32_t 0 失败，1 成功
 *
 */
uint32_t gprs_init(void)
{
    uint8_t state;
    uint32_t res;
    uint32_t res_at;

    res = 0;
#if 1
    GPRS_reset;
    pGprsRecvQue->Flush(pGprsRecvQue);
    gprs_send_PlusPlusPlus();
    gprs_send_ATH();
    res_at = gprs_send_AT();
    if(res_at != DR_AT_OK)
    {
        gprs_ioctl(DA_GPRS_RESET);
    }
    else
    {
        dev_gprs.state = DS_GPRS_ON;
    }

    if(dev_gprs.state == DS_GPRS_ON)
    {
        res = 1;
        dev_gprs.pollstate = DS_GPRS_POLL_AT;
    }
    else
    {
        res = 0;
        ThrowErrorCode(defDevID_GPRS, ERR_GPRS_FAULT, ERR_LEVEL_WARNING, "初始化失败");
    }
    #endif
    return res;
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
        if(buff[0] == 0x30 && buff[1] == 0x0d)
        {
            return DR_AT_OK;
        }
        else
        {
            return DR_AT_ERR;
        }
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

//    res = readRecvQueEx(pGprsRecvQue, buff, 0, &rl);
////    p = strstr(buff, "+CPIN: READY");
//    p = strstr(buff, "READY");
    res = recvStrCmp(pGprsRecvQue, "READY", 0);
    if(res == 1)//if(p != NULL)
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
//    p = strstr(buff, "+CREG: 0,5");
    p = strstr(buff, "+CREG: 0");

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
//    p = strstr(buff, "+CGREG: 0,5");
    p = strstr(buff, "+CGREG: 0");
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
uint32_t gprs_send_PlusPlusPlus(void)
{
    pGprsRecvQue->Flush(pGprsRecvQue);
    HAL_UART_Transmit(&GPRS_UARTx_Handler, "+++", strlen("+++"), 0xFFFF);
    gprs_delayms(1000);
    return gprs_read_AT();
}
uint32_t gprs_send_ATH(void)
{
    pGprsRecvQue->Flush(pGprsRecvQue);
    HAL_UART_Transmit(&GPRS_UARTx_Handler, "ATH\r\n", strlen("ATH\r\n"), 0xFFFF);
    gprs_delayms(5000);
    return gprs_read_AT();
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
uint32_t gprs_send_RESET(void)
{
    pGprsRecvQue->Flush(pGprsRecvQue);
    HAL_UART_Transmit(&GPRS_UARTx_Handler, "AT+CFUN=1,1\r\n", strlen("AT+CFUN=1,1\r\n"), 0xFFFF);
    gprs_delayms(100);
}
uint32_t gprs_ppp_poll(void)
{
    uint32_t res_at;
    uint32_t try_cont = 0;
    EventBits_t uxBitLwip;
    while(1)
    {
        switch(dev_gprs.pollstate)
        {
        case DS_GPRS_POLL_AT:
            try_cont++;
#if DEBUG_DEV_GPRS
            printf_safe("GPRS Send CMD：AT 第 %d 次（5次）\n", try_cont);
#endif
            res_at = gprs_send_AT();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
#if DEBUG_DEV_GPRS
                    printf_safe("GPRS Send CMD：AT 失败\n");
#endif
                    try_cont = 0;
                    dev_gprs.pollstate = DS_GPRS_POLL_ERR;
                }
                else
                {
                    dev_gprs.pollstate = DS_GPRS_POLL_PlusPlusPlus;
                }
            }
            else
            {
#if DEBUG_DEV_GPRS
                printf_safe("GPRS recv CMD：AT OK\n");
#endif
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_ATE0V1;
            }
            break;
        case DS_GPRS_POLL_PlusPlusPlus:
            res_at = gprs_send_PlusPlusPlus();
            if(res_at == DR_AT_OK)
            {
                dev_gprs.pollstate = DS_GPRS_POLL_AT;
            }
            else
            {
                dev_gprs.pollstate = DS_GPRS_POLL_ERR;
            }
            break;
        case DS_GPRS_POLL_ATE0V1:
            try_cont++;
#if DEBUG_DEV_GPRS
            printf_safe("GPRS Send CMD：ATE0V1 第 %d 次（5次）\n", try_cont);
#endif
            res_at = gprs_send_ATE0V1();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
                    try_cont = 0;
#if DEBUG_DEV_GPRS
                    printf_safe("GPRS Send CMD：ATE0V1 失败\n");
#endif
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
#if DEBUG_DEV_GPRS
                printf_safe("GPRS recv CMD：ATE0V1 OK\n");
#endif
                dev_gprs.pollstate = DS_GPRS_POLL_CPIN;
            }
            break;
        case DS_GPRS_POLL_CPIN:
            try_cont++;
#if DEBUG_DEV_GPRS
            printf_safe("GPRS Send CMD：AT+CPIN? 第 %d 次（5次）\n", try_cont);
#endif
            res_at = gprs_send_CPIN();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
                    try_cont = 0;
#if DEBUG_DEV_GPRS
                    printf_safe("GPRS Send CMD：AT+CPIN? 失败\n");
#endif
                    dev_gprs.pollstate = DS_GPRS_POLL_ERR;
                }
                else
                {
                    dev_gprs.pollstate = DS_GPRS_POLL_CPIN;
                }
            }
            else
            {
#if DEBUG_DEV_GPRS
                printf_safe("GPRS recv CMD：AT+CPIN? OK\n");
#endif
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_CREG;
            }
            break;
        case DS_GPRS_POLL_CREG:
            try_cont++;
#if DEBUG_DEV_GPRS
            printf_safe("GPRS Send CMD：AT+CREG? 第 %d 次（5次）\n", try_cont);
#endif
            res_at = gprs_send_CREG();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
                    try_cont = 0;
#if DEBUG_DEV_GPRS
                    printf_safe("GPRS Send CMD：AT+CREG? 失败\n");
#endif
                    dev_gprs.pollstate = DS_GPRS_POLL_ERR;
                }
                else
                {
                    dev_gprs.pollstate = DS_GPRS_POLL_CREG;
                }
            }
            else
            {
#if DEBUG_DEV_GPRS
                printf_safe("GPRS recv CMD：AT+CREG? OK\n");
#endif
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_CGREG;
            }
            break;

        case DS_GPRS_POLL_CGREG:
            try_cont++;
#if DEBUG_DEV_GPRS
            printf_safe("GPRS Send CMD：AT+CGREG? 第 %d 次（5次）\n", try_cont);
#endif
            res_at = gprs_send_CGREG();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
#if DEBUG_DEV_GPRS
                    printf_safe("GPRS Send CMD：AT+CGREG? 失败\n");
#endif
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
#if DEBUG_DEV_GPRS
                printf_safe("GPRS recv CMD：AT+CGREG? OK\n");
#endif
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_APN;
            }
            break;
        case DS_GPRS_POLL_APN:
            try_cont++;
#if DEBUG_DEV_GPRS
            printf_safe("GPRS Send CMD：APN 第 %d 次（5次）\n", try_cont);
#endif
            res_at = gprs_send_APN();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
#if DEBUG_DEV_GPRS
                    printf_safe("GPRS Send CMD：APN 失败\n");
#endif
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
#if DEBUG_DEV_GPRS
                printf_safe("GPRS recv CMD：APN OK\n");
#endif
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_ATD;
            }
            break;
        case DS_GPRS_POLL_ATD:
            try_cont++;
#if DEBUG_DEV_GPRS
            printf_safe("GPRS Send CMD：ATD 第 %d 次（5次）\n", try_cont);
#endif
            res_at = gprs_send_ATD();
            if(res_at != DR_AT_OK)
            {
                if(try_cont > 5)
                {
#if DEBUG_DEV_GPRS
                    printf_safe("GPRS Send CMD：ATD 失败\n");
#endif
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
#if DEBUG_DEV_GPRS
                printf_safe("GPRS recv CMD：ATD OK\n");
#endif
                try_cont = 0;
                dev_gprs.pollstate = DS_GPRS_POLL_PPP;
            }
            break;
        case DS_GPRS_POLL_PPP:
            xEventGroupSetBits(xHandleEventTCP, defEventBitDailCONNECT);
            dev_gprs.pollstate = DS_GPRS_POLL_PPPDego;
            break;
        case DS_GPRS_POLL_PPPDego:
//            if(recvStrCmp(pGprsRecvQue, "NO CARRIER", 0) == 1)
//            {
//
//            }
            uxBitLwip = xEventGroupWaitBits(xHandleEventTCP, defEventBitReDail,
                                            pdTRUE, pdTRUE, portMAX_DELAY);
            if((uxBitLwip & defEventBitReDail) == defEventBitReDail)
            {
                dev_gprs.pollstate = DS_GPRS_POLL_ERR;
            }
            break;
        case DS_GPRS_POLL_ERR:
            gprs_send_RESET();
            vTaskDelay(10000);
            dev_gprs.pollstate = DS_GPRS_POLL_AT;
            break;
        }
        vTaskDelay(1000);
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

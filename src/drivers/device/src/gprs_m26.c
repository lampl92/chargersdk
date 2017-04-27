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

uint8_t gprs_ioctl(uint8_t ioctl)
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
            res = recvStrCmp(pGprsRecvQue, "Call Ready", 0);
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
            res = recvStrCmp(pGprsRecvQue, "Call Ready", 0);
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

uint8_t gprs_init(void)
{
    uint8_t state;
    uint32_t res;

    GPRS_IO = 0;
    pGprsRecvQue->Flush(pGprsRecvQue);
    gprs_ioctl(DA_GPRS_RESET);
    if(dev_gprs.state == DS_GPRS_ERR)
    {
        ThrowErrorCode(defDevID_GPRS, ERR_GPRS_FAULT, ERR_LEVEL_WARNING, "初始化失败");
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

/**
* @file taskppp.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-02
*/

#include "netif/ppp/ppp.h"
#include "lwip_init.h"
#include "bsp.h"
#include "gprs_m26.h"


void input_over_serial(int ppp)
{
    uint8_t data[1500];
    uint32_t len;
//    int i;
    if(readRecvQueProto(pGprsRecvQue, data, 0x7e, 0x7e, &len) == 1)
    {
        if(len != 0)
        {
            pppos_input(ppp, data, len);
        }
//        printf_safe("mcu input: ");
//        for(i = 0; i < len; i++)
//        {
//            printf_safe("%02X ", data[i]);
//        }
//        printf_safe("\n");
//        printf_safe("len = %d\n", len);
    }
}

void vTaskPPP(void *pvParameters)
{
    int ppp;
    ppp = lwip_init_task();
    while(1)
    {
//        if(dev_gprs.pollstate == DS_GPRS_POLL_PPPDego)
//        {
//            input_over_serial(ppp);
//        }

        vTaskDelay(1000);
    }
}

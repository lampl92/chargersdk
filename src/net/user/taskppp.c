/**
* @file taskppp.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-02
*/

#include "netif/ppp/pppos.h"
#include "lwip_init.h"
#include "bsp.h"

void input_over_serial(ppp_pcb *pcb)
{
    uint8_t data[512];
    uint32_t len;
    int i;
    if(readRecvQueEx(pGprsRecvQue, data, 0, &len) == 1)
    {
        pppos_input_tcpip(pcb, data, len);
        printf_safe("mcu input: ");
        for(i = 0; i < len; i++)
        {
            printf_safe("%X ", data[i]);
        }
        printf_safe("\n");
        printf_safe("len = %d\n", len);
    }
}

void vTaskPPP(void *pvParameters)
{
    ppp_pcb *ppp;
    ppp = lwip_init_task();
    while(1)
    {
        input_over_serial(ppp);
        vTaskDelay(100);
    }
}

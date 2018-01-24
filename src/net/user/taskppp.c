/**
* @file taskppp.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-02
*/
#include "bsp.h"
#include "gprs_m26.h"
#include "FreeRTOS.h"
#include "event_groups.h"

#if 0
EventBits_t uxBitLwip;
//void input_over_serial(int ppp)
//{
//    uint8_t data[1500];
//    uint32_t len;
////    int i;
//    if(readRecvQueProto(pGprsRecvQue, data, 0x7e, 0x7e, &len) == 1)
//    {
//        if(len != 0)
//        {
//            pppos_input(ppp, data, len);
//        }
////        printf_safe("mcu input: ");
////        for(i = 0; i < len; i++)
////        {
////            printf_safe("%02X ", data[i]);
////        }
////        printf_safe("\n");
////        printf_safe("len = %d\n", len);
//    }
//}

void vTaskPPP(void *pvParameters)
{
    int ppp;
    ppp = lwip_init_task();
    while(1)
    {
        uxBitLwip = xEventGroupWaitBits(xHandleEventTCP,
                                        defEventBitDailCONNECT,
                                        pdTRUE, pdTRUE, portMAX_DELAY);
        if((uxBitLwip & defEventBitDailCONNECT) == defEventBitDailCONNECT)
        {
            ppp = pppOverSerialOpen(0, ppp_on_status, &ppp);
        }
        vTaskDelay(1000);
    }
}
#endif
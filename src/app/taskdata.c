/**
* @file taskdata.c
* @brief �����ļ���д����������
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "includes.h"
#include "taskcreate.h"
#include "taskdata.h"
#include "interface.h"

//#define DEBUG_NO_TASKDATA

void vTaskEVSEData(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBitsTimer;
    EventBits_t uxBitsData;
    EventBits_t uxBitsCharge;

    ulTotalCON = pListCON->Total;
    uxBitsTimer = 0;
    uxBitsData = 0;
    uxBitsCharge = 0;
    THROW_ERROR(defDevID_File, CreateOrderFile(), ERR_LEVEL_WARNING);//����order.txt
    while(1)
    {
#ifndef DEBUG_NO_TASKDATA
        /* �������� */
        //1. �ȴ�ˢ������¼�
        uxBitsData = xEventGroupWaitBits(xHandleEventData,
                                         defEventBitOrderTmp,   //RFID�з������¼�
                                         pdTRUE, pdFALSE, 0);
        if((uxBitsData & defEventBitOrderTmp) == defEventBitOrderTmp)
        {
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            pCON->order.state = STATE_ORDER_TMP;
        }
        //2. �ȴ�StartCharge�¼�
        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                               defEventBitCONStartOK,
                                               pdFALSE, pdFALSE, 0);
            if((uxBitsCharge & defEventBitCONStartOK) == defEventBitCONStartOK)
            {
                pCON->order.state = STATE_ORDER_MAKE;
            }
        }

        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            switch(pCON->order.state)
            {
            case STATE_ORDER_IDLE:
                break;
            case STATE_ORDER_TMP:
                makeOrder(pCON);
                xEventGroupSetBits(xHandleEventData, defEventBitOrderUpdateOK);
                break;
            case STATE_ORDER_MAKE:
                makeOrder(pCON);
                xEventGroupSetBits(xHandleEventData, defEventBitOrderMakeOK);
                break;
            case STATE_ORDER_UPDATE:
            case STATE_ORDER_FINISH:
                break;
            }
        }


        /* ��ȡ�ļ����� */
        uxBitsTimer = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                          defEventBitTimerCBDataRefresh,
                                          pdTRUE, pdFALSE, 0);
        if((uxBitsTimer & defEventBitTimerCBDataRefresh) == defEventBitTimerCBDataRefresh)
        {
            THROW_ERROR(defDevID_File, pEVSE->info.GetSN(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetID(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetType(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetTotalCON(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetLngLat(pEVSE), ERR_LEVEL_WARNING);
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                THROW_ERROR(defDevID_File, pCON->info.GetSocketType(pCON), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pCON->info.GetVolatageUpperLimits(pCON), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pCON->info.GetVolatageLowerLimits(pCON), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pCON->info.GetRatedCurrent(pCON), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pCON->info.GetRatedPower(pCON), ERR_LEVEL_WARNING);
            }
        }
        /* end of ��ȡ�ļ����� */

//        uxBits = xEventGroupWaitBits(xHandleEventData, defEventBitAddOrder, pdTRUE, pdFALSE, 0);
//        if((uxBits & defEventBitAddOrder) == defEventBitAddOrder)
//        {
//            DataAddOrder();
//            xEventGroupSetBits(xHandleEventData, defEventBitAddOrderOK);
//        }

#if DEBUG_DATA

#endif
#endif //DEBUG_NO_TASKDATA
        vTaskDelay(100);
    }
}

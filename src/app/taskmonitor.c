/**
* @file taskmonitor.c
* @brief �������
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskmonitor.h"
#include "interface.h"

static ErrorLevel_t HandleTemp(ChargePoint_t *pPoint, double temp, double lower, double upper)
{
    ErrorLevel_t errlevel;
    if(temp >= lower ||  temp <= upper )    //-40~105
    {
        if(temp >= upper - 10)      //95~105
        {
            errlevel = ERR_LEVEL_WARNING;
        }
        else                        //-40~95
        {
            errlevel = ERR_LEVEL_OK;
        }
    }
    else if(temp > upper)                   //>105
    {
        errlevel = ERR_LEVEL_CRITICAL;
    }
    else if(temp < lower)                   //<-40
    {
        //...
    }
    return errlevel;
}

void vTaskEVSEMonitor(void *pvParameters)
{
    uint32_t ulTotalPoint = pListChargePoint->Total;
    ChargePoint_t *pPoint[ulTotalPoint];
    int i;
    uint8_t ucCurrentId;
    EventBits_t uxBits;
    ErrorLevel_t errlevel;
    for(i = 0; i < ulTotalPoint; i++)
    {
        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray[i]);
    }
    ucCurrentId = 0;
    uxBits = 0;
    while(1)
    {
        /** ��ȡEVSE��ChargePoint״̬ */

        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBTemp, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBTemp) == defEventBitTimerCBTemp)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                THROW_ERROR(i, pPoint[i]->status.GetACLTemp(pPoint[i]), ERR_LEVEL_WARNING);
                THROW_ERROR(i, pPoint[i]->status.GetACNTemp(pPoint[i]), ERR_LEVEL_WARNING);
                if(pPoint[i]->info.ucConnectorType == defConnectorTypeB)
                {
                    THROW_ERROR(i, pPoint[i]->status.GetBTypeConnectorTemp1(pPoint[i]), ERR_LEVEL_WARNING);
                    THROW_ERROR(i, pPoint[i]->status.GetBTypeConnectorTemp2(pPoint[i]), ERR_LEVEL_WARNING);
                }
            }
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBLockState, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBLockState) == defEventBitTimerCBLockState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                if(pPoint[i]->info.ucConnectorType == defConnectorTypeB)
                {
                    THROW_ERROR(i, pPoint[i]->status.GetBTypeConnectorLock(pPoint[i]), ERR_LEVEL_WARNING);
                }
            }
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBPlugState, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBPlugState) == defEventBitTimerCBPlugState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                THROW_ERROR(i, pPoint[i]->status.GetPlugState(pPoint[i]), ERR_LEVEL_CRITICAL);//��GetPlugState�л�ȡ��CC��CP״̬
            }
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBChargingData, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBChargingData) == defEventBitTimerCBChargingData)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                THROW_ERROR(i, pPoint[i]->status.GetChargingVoltage(pPoint[i]), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pPoint[i]->status.GetChargingCurrent(pPoint[i]), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pPoint[i]->status.GetChargingFrequence(pPoint[i]), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pPoint[i]->status.GetRelayState(pPoint[i]), ERR_LEVEL_CRITICAL);
            }
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBEVSEState, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBEVSEState) == defEventBitTimerCBEVSEState)
        {
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetScramState(pEVSE), ERR_LEVEL_CRITICAL);
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetPEState(pEVSE), ERR_LEVEL_CRITICAL);
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetKnockState(pEVSE), ERR_LEVEL_TIPS);
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetArresterState(pEVSE), ERR_LEVEL_TIPS);
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetPowerOffState(pEVSE), ERR_LEVEL_TIPS);
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBRFID, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBRFID) == defEventBitTimerCBRFID)
        {
            THROW_ERROR(defDevID_RFID, pRFIDDev->status.GetUID(pRFIDDev), ERR_LEVEL_CRITICAL);
        }

        /** end of ��ȡEVSE��ChargePoint״̬ */

        /** �жϸ�״̬ */
        if((uxBits & defEventBitTimerCBTemp) == defEventBitTimerCBTemp)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                /** ACLTemp */
                errlevel = HandleTemp(pPoint[i]->status.dACLTemp,
                                      pPoint[i]->info.dACTempLowerLimits,
                                      pPoint[i]->info.dACTempUpperLimits);
                switch(errlevel)
                {
                case ERR_LEVEL_OK:
                    xEventGroupClearBits(pPoint[i]->status.xHandleEventException, defEventBitExceptionTempW);
                    xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdACTempOK);
                    break;
                case ERR_LEVEL_WARNING:
                    xEventGroupSetBits(pPoint[i]->status.xHandleEventException, defEventBitExceptionTempW);
                    xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdACTempOK);
                    break;
                case ERR_LEVEL_CRITICAL:
                    //����ģ����Ƴ��׮ͣ�����Ͽ�AC���������תS1���أ�CP�źű��ָߵ�ƽ���
                    xEventGroupClearBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdACTempOK);
                    ThrowErrorCode(i, ERR_POINT_ACLTEMP_DECT_FAULT, ERR_LEVEL_CRITICAL);
                }
                /** end of ACLTemp */

                /** ACNTemp */
                errlevel = HandleTemp(pPoint[i]->status.dACNTemp,
                                      pPoint[i]->info.dACTempLowerLimits,
                                      pPoint[i]->info.dACTempUpperLimits);
                switch(errlevel)
                {
                case ERR_LEVEL_OK:
                    xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdACTempOK);
                    break;
                case ERR_LEVEL_WARNING:
                    xEventGroupSetBits(pPoint[i]->status.xHandleEventException, defEventBitExceptionTempW);
                    xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdACTempOK);
                    break;
                case ERR_LEVEL_CRITICAL:
                    //����ģ����Ƴ��׮ͣ�����Ͽ�AC���������תS1���أ�CP�źű��ָߵ�ƽ���
                    xEventGroupClearBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdACTempOK);
                    ThrowErrorCode(i, ERR_POINT_ACNTEMP_DECT_FAULT, ERR_LEVEL_CRITICAL);
                }
                /** end of ACNTemp */

                if(pPoint[i]->info.ucConnectorType == defConnectorTypeB)
                {
                    /** ConnectorTemp1 */
                    errlevel = HandleTemp(pPoint[i]->status.dBTypeConnectorTemp1,
                                          pPoint[i]->info.dConnectorTempLowerLimits,
                                          pPoint[i]->info.dConnectorTempUpperLimits);
                    switch(errlevel)
                    {
                    case ERR_LEVEL_OK:
                        xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdConnTempOK);
                        break;
                    case ERR_LEVEL_WARNING:

                        xEventGroupSetBits(pPoint[i]->status.xHandleEventException, defEventBitExceptionTempW);
                        xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdConnTempOK);
                        break;
                    case ERR_LEVEL_CRITICAL:
                        //����ģ����Ƴ��׮ͣ�����Ͽ�AC���������תS1���أ�CP�źű��ָߵ�ƽ���
                        xEventGroupClearBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdConnTempOK);
                        ThrowErrorCode(i, ERR_POINT_BTEMP1_DECT_FAULT, ERR_LEVEL_CRITICAL);
                    }
                    /** end of ConnectorTemp1 */

                    /** ConnectorTemp2 */
                    errlevel = HandleTemp(pPoint[i]->status.dBTypeConnectorTemp2,
                                          pPoint[i]->info.dConnectorTempLowerLimits,
                                          pPoint[i]->info.dConnectorTempUpperLimits);
                    switch(errlevel)
                    {
                    case ERR_LEVEL_OK:
                        xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdConnTempOK);
                        break;
                    case ERR_LEVEL_WARNING:

                        xEventGroupSetBits(pPoint[i]->status.xHandleEventException, defEventBitExceptionTempW);
                        xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdConnTempOK);
                        break;
                    case ERR_LEVEL_CRITICAL:
                        //����ģ����Ƴ��׮ͣ�����Ͽ�AC���������תS1���أ�CP�źű��ָߵ�ƽ���
                        xEventGroupClearBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdConnTempOK);
                        ThrowErrorCode(i, ERR_POINT_BTEMP2_DECT_FAULT, ERR_LEVEL_CRITICAL);
                    }
                    /** end of ConnectorTemp2 */
                }
            }
        }/** end of defEventBitTimerCBTemp */
        if((uxBits & defEventBitTimerCBLockState) == defEventBitTimerCBLockState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                if(pPoint[i]->info.ucConnectorType == defConnectorTypeB)
                {
                    if(pPoint[i]->status.xBTypeConnectorLockState == LOCK)
                    {
                        xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdLocked);
                    }
                    else
                    {
                        xEventGroupClearBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdLocked);
                    }
                }
            }
        }/** end of defEventBitTimerCBLockState */

        if((uxBits & defEventBitTimerCBPlugState) == defEventBitTimerCBPlugState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                if(pPoint[i]->status.xPlugState == PLUG)
                {
                    xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdPlugOK);
                }
                else if(pPoint[i]->status.xPlugState == UNPLUG)
                {
                    xEventGroupClearBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdPlugOK);
                }

                /** @note (rgw#1#):  CC��CP״̬�Ѿ���GetPlugState�л�ȡ����TaskCharge���ж�*/
            }
        }
        if((uxBits & defEventBitTimerCBChargingData) == defEventBitTimerCBChargingData)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                if()
//                THROW_ERROR(pPoint[i]->status.GetChargingVoltage(pPoint[i]), ERR_LEVEL_CRITICAL);
//                THROW_ERROR(pPoint[i]->status.GetChargingCurrent(pPoint[i]), ERR_LEVEL_CRITICAL);
//                THROW_ERROR(pPoint[i]->status.GetChargingFrequence(pPoint[i]), ERR_LEVEL_CRITICAL);
            }
        }
        if((uxBits & defEventBitTimerCBEVSEState) == defEventBitTimerCBEVSEState)
        {
//            THROW_ERROR(pEVSE->status.GetScramState(pEVSE), ERR_LEVEL_CRITICAL);
//            THROW_ERROR(pEVSE->status.GetPEState(pEVSE), ERR_LEVEL_CRITICAL);
//            THROW_ERROR(pEVSE->status.GetKnockState(pEVSE), ERR_LEVEL_TIPS);
//            THROW_ERROR(pEVSE->status.GetArresterState(pEVSE), ERR_LEVEL_TIPS);
//            THROW_ERROR(pEVSE->status.GetPowerOffState(pEVSE), ERR_LEVEL_TIPS);
        }
        /** end of �ж�״̬ */


#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSEMonitor);
#endif
        vTaskDelay(50);
    }
}

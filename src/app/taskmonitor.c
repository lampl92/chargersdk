/**
* @file taskmonitor.c
* @brief 监控任务
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskmonitor.h"
#include "interface.h"
typedef enum
{
    VOLT_OK,
    VOLT_LOWER,
    VOLT_UPPER
} HandleVolt_t;
extern void vVoltTimerCB(TimerHandle_t xTimer);
static ErrorLevel_t HandleTemp(double temp, double lower, double upper)
{
    ErrorLevel_t templevel;
    if(temp >= lower &&  temp <= upper )    //-40~105
    {
        if(temp >= upper - 10)      //95~105
        {
            templevel = ERR_LEVEL_WARNING;
        }
        else                        //-40~95
        {
            templevel = ERR_LEVEL_OK;
        }
    }
    else if(temp > upper)                   //>105
    {
        templevel = ERR_LEVEL_CRITICAL;
    }
    else if(temp < lower)                   //<-40
    {
        //...
    }
    return templevel;
}

static HandleVolt_t HandleVolt(double volt, double lower, double upper)
{
    HandleVolt_t voltstat;
    if(volt >= lower && volt <= upper)
    {
        voltstat = VOLT_OK;
    }
    else if(volt > upper)
    {
        voltstat = VOLT_UPPER;
    }
    else if(volt < lower)
    {
        voltstat = VOLT_LOWER;
    }
    return voltstat;
}
void vTaskEVSEMonitor(void *pvParameters)
{
    ChargePoint_t *pPoint = NULL;
    uint32_t ulTotalPoint;
    int i;
    EventBits_t uxBits;
    ErrorLevel_t templevel;
    HandleVolt_t voltstat;
    uint8_t strTimerName[30];

    ulTotalPoint = pListChargePoint->Total;
    uxBits = 0;
    while(1)
    {

        /* 获取EVSE和ChargePoint状态 */

        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBTemp, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBTemp) == defEventBitTimerCBTemp)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                THROW_ERROR(i, pPoint->status.GetACLTemp(pPoint), ERR_LEVEL_WARNING);
                THROW_ERROR(i, pPoint->status.GetACNTemp(pPoint), ERR_LEVEL_WARNING);
                if(pPoint->info.ucConnectorType == defConnectorTypeB)
                {
                    THROW_ERROR(i, pPoint->status.GetBTypeConnectorTemp1(pPoint), ERR_LEVEL_WARNING);
                    THROW_ERROR(i, pPoint->status.GetBTypeConnectorTemp2(pPoint), ERR_LEVEL_WARNING);
                }
            }
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBLockState, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBLockState) == defEventBitTimerCBLockState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                if(pPoint->info.ucConnectorType == defConnectorTypeB)
                {
                    THROW_ERROR(i, pPoint->status.GetBTypeConnectorLock(pPoint), ERR_LEVEL_WARNING);
                }
            }
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBPlugState, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBPlugState) == defEventBitTimerCBPlugState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                THROW_ERROR(i, pPoint->status.GetPlugState(pPoint), ERR_LEVEL_CRITICAL);//在GetPlugState中获取了CC与CP状态
            }
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBChargingData, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBChargingData) == defEventBitTimerCBChargingData)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                THROW_ERROR(i, pPoint->status.GetChargingVoltage(pPoint), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pPoint->status.GetChargingCurrent(pPoint), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pPoint->status.GetChargingFrequence(pPoint), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pPoint->status.GetRelayState(pPoint), ERR_LEVEL_CRITICAL);
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

        /* end of 获取EVSE和ChargePoint状态 */

        /* 判断各状态 */
        if((uxBits & defEventBitTimerCBTemp) == defEventBitTimerCBTemp)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                /* ACLTemp */
                templevel = HandleTemp(pPoint->status.dACLTemp,
                                       pPoint->info.dACTempLowerLimits,
                                       pPoint->info.dACTempUpperLimits);
                switch(templevel)
                {
                case ERR_LEVEL_OK:
                    xEventGroupClearBits(pPoint->status.xHandleEventException, defEventBitExceptionTempW);
                    xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointACTempOK);
                    break;
                case ERR_LEVEL_WARNING:
                    xEventGroupSetBits(pPoint->status.xHandleEventException, defEventBitExceptionTempW);
                    xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointACTempOK);
                    break;
                case ERR_LEVEL_CRITICAL:
                    //控制模块控制充电桩停机，断开AC输出，并跳转S1开关，CP信号保持高电平输出
                    xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointACTempOK);
                    ThrowErrorCode(i, ERR_POINT_ACLTEMP_DECT_FAULT, ERR_LEVEL_CRITICAL);
                    break;
                default:
                    break;
                }
                /* end of ACLTemp */

                /* ACNTemp */
                templevel = HandleTemp(pPoint->status.dACNTemp,
                                       pPoint->info.dACTempLowerLimits,
                                       pPoint->info.dACTempUpperLimits);
                switch(templevel)
                {
                case ERR_LEVEL_OK:
                    xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointACTempOK);
                    break;
                case ERR_LEVEL_WARNING:
                    xEventGroupSetBits(pPoint->status.xHandleEventException, defEventBitExceptionTempW);
                    xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointACTempOK);
                    break;
                case ERR_LEVEL_CRITICAL:
                    //控制模块控制充电桩停机，断开AC输出，并跳转S1开关，CP信号保持高电平输出
                    xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointACTempOK);
                    ThrowErrorCode(i, ERR_POINT_ACNTEMP_DECT_FAULT, ERR_LEVEL_CRITICAL);
                    break;
                default:
                    break;
                }
                /* end of ACNTemp */

                if(pPoint->info.ucConnectorType == defConnectorTypeB)
                {
                    /* ConnectorTemp1 */
                    templevel = HandleTemp(pPoint->status.dBTypeConnectorTemp1,
                                           pPoint->info.dConnectorTempLowerLimits,
                                           pPoint->info.dConnectorTempUpperLimits);
                    switch(templevel)
                    {
                    case ERR_LEVEL_OK:
                        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointConnTempOK);
                        break;
                    case ERR_LEVEL_WARNING:
                        xEventGroupSetBits(pPoint->status.xHandleEventException, defEventBitExceptionTempW);
                        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointConnTempOK);
                        break;
                    case ERR_LEVEL_CRITICAL:
                        //控制模块控制充电桩停机，断开AC输出，并跳转S1开关，CP信号保持高电平输出
                        xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointConnTempOK);
                        ThrowErrorCode(i, ERR_POINT_BTEMP1_DECT_FAULT, ERR_LEVEL_CRITICAL);
                        break;
                    default:
                        break;
                    }
                    /* end of ConnectorTemp1 */

                    /* ConnectorTemp2 */
                    templevel = HandleTemp(pPoint->status.dBTypeConnectorTemp2,
                                           pPoint->info.dConnectorTempLowerLimits,
                                           pPoint->info.dConnectorTempUpperLimits);
                    switch(templevel)
                    {
                    case ERR_LEVEL_OK:
                        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointConnTempOK);
                        break;
                    case ERR_LEVEL_WARNING:
                        xEventGroupSetBits(pPoint->status.xHandleEventException, defEventBitExceptionTempW);
                        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointConnTempOK);
                        break;
                    case ERR_LEVEL_CRITICAL:
                        //控制模块控制充电桩停机，断开AC输出，并跳转S1开关，CP信号保持高电平输出
                        xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointConnTempOK);
                        ThrowErrorCode(i, ERR_POINT_BTEMP2_DECT_FAULT, ERR_LEVEL_CRITICAL);
                        break;
                    default:
                        break;
                    }
                    /* end of ConnectorTemp2 */
                }
            }
        }/* end of defEventBitTimerCBTemp */
        if((uxBits & defEventBitTimerCBLockState) == defEventBitTimerCBLockState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                if(pPoint->info.ucConnectorType == defConnectorTypeB)
                {
                    if(pPoint->status.xBTypeConnectorLockState == LOCK)
                    {
                        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointLocked);
                    }
                    else
                    {
                        xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointLocked);
                    }
                }
            }
        }/* end of defEventBitTimerCBLockState */

        if((uxBits & defEventBitTimerCBPlugState) == defEventBitTimerCBPlugState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                if(pPoint->status.xPlugState == PLUG)
                {
                    xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointPlugOK);
                }
                else if(pPoint->status.xPlugState == UNPLUG)
                {
                    xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointPlugOK);
                }

                /** @note (rgw#1#):  CC和CP状态已经在GetPlugState中获取，在TaskCharge中判断*/
            }
        }/* end of defEventBitTimerCBPlugState */
        if((uxBits & defEventBitTimerCBChargingData) == defEventBitTimerCBChargingData)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                /* 电压判断 */
                voltstat = HandleVolt(pPoint->status.dChargingVoltage,
                                      pPoint->info.dVolatageLowerLimits,
                                      pPoint->info.dVolatageUpperLimits);

                switch(pPoint->status.xVoltStat)
                {
                case STATE_VOLT_OK:
                    switch(voltstat)
                    {
                    case VOLT_OK:
                        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointVoltOK);
                        break;
                    case VOLT_LOWER:
                        xsprintf(strTimerName, "TimerPoint%d_VoltLow_Dummy", i);
                        pPoint->status.xHandleTimerVolt = xTimerCreate(strTimerName, 3000, pdFALSE, (void *)i, vVoltTimerCB);
                        xTimerStart(pPoint->status.xHandleTimerVolt, 0);
                        pPoint->status.xVoltStat = STATE_VOLT_LOWER_Dummy;
                        break;
                    case VOLT_UPPER:
                        xsprintf(strTimerName, "TimerPoint%d_VoltUp_Dummy", i);
                        pPoint->status.xHandleTimerVolt = xTimerCreate(strTimerName, 3000, pdFALSE, (void *)i, vVoltTimerCB);
                        xTimerStart(pPoint->status.xHandleTimerVolt, 0);
                        pPoint->status.xVoltStat = STATE_VOLT_UPPER_Dummy;
                        break;
                    default:
                        break;
                    }
                case STATE_VOLT_LOWER_Dummy:
                case STATE_VOLT_UPPER_Dummy:
                    uxBits = xEventGroupWaitBits(pPoint->status.xHandleEventException,
                                                 defEventBitExceptionVoltTimer,
                                                 pdTRUE, pdFALSE, 0);
                    if((uxBits & defEventBitExceptionVoltTimer) == defEventBitExceptionVoltTimer)
                    {
                        xTimerDelete(pPoint->status.xHandleTimerVolt, 0);
                        xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointVoltOK);
                        if(pPoint->state == POINT_CHARGING)
                        {
                            THROW_ERROR(i, pPoint->status.SetRelay(pPoint, SWITCH_OFF), ERR_LEVEL_CRITICAL);
                        }
                        /********************************/
                        /** @todo (rgw#1#): 向系统告警 */
                        /********************************/
                        if(pPoint->status.xVoltStat == STATE_VOLT_LOWER_Dummy)
                        {
                            pPoint->status.xVoltStat = STATE_VOLT_LOWER;
                        }
                        if(pPoint->status.xVoltStat == STATE_VOLT_UPPER_Dummy)
                        {
                            pPoint->status.xVoltStat = STATE_VOLT_UPPER;
                        }
                    }
                    else
                    {
                        switch(voltstat)
                        {
                        case VOLT_OK:
                            xTimerStop(pPoint->status.xHandleTimerVolt, 0);
                            xTimerDelete(pPoint->status.xHandleTimerVolt, 0);
                            pPoint->status.xVoltStat = STATE_VOLT_OK;
                            break;
                        case VOLT_LOWER:
                            if(pPoint->status.xVoltStat == STATE_VOLT_UPPER_Dummy)
                            {
                                xTimerReset(pPoint->status.xHandleTimerVolt, 0);
                                pPoint->status.xVoltStat = STATE_VOLT_LOWER_Dummy;
                            }
                            break;
                        case VOLT_UPPER:
                            if(pPoint->status.xVoltStat == STATE_VOLT_LOWER_Dummy)
                            {
                                xTimerReset(pPoint->status.xHandleTimerVolt, 0);
                                pPoint->status.xVoltStat = STATE_VOLT_UPPER_Dummy;
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    break;
                case STATE_VOLT_LOWER:
                case STATE_VOLT_UPPER:
                    voltstat = HandleVolt(pPoint->status.dChargingVoltage,
                                          pPoint->info.dVolatageLowerLimits + 10,
                                          pPoint->info.dVolatageUpperLimits - 10);
                    switch(voltstat)
                    {
                    case VOLT_OK://200~240
                        xsprintf(strTimerName, "TimerPoint%d_VoltOK_Dummy", i);
                        pPoint->status.xHandleTimerVolt = xTimerCreate(strTimerName, 5000, pdFALSE, (void *)i, vVoltTimerCB);
                        xTimerStart(pPoint->status.xHandleTimerVolt, 0);
                        pPoint->status.xVoltStat = STATE_VOLT_OK_Dummy;
                        break;
                    case VOLT_LOWER:
                    case VOLT_UPPER:
                        break;
                    default:
                        break;
                    }
                    break;
                case STATE_VOLT_OK_Dummy:
                    uxBits = xEventGroupWaitBits(pPoint->status.xHandleEventException,
                                                 defEventBitExceptionVoltTimer,
                                                 pdTRUE, pdFALSE, 0);
                    if((uxBits & defEventBitExceptionVoltTimer) == defEventBitExceptionVoltTimer)
                    {
                        xTimerDelete(pPoint->status.xHandleTimerVolt, 0);
                        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointVoltOK);
                        if(pPoint->state == POINT_CHARGING)
                        {
                            THROW_ERROR(i, pPoint->status.SetRelay(pPoint, SWITCH_ON), ERR_LEVEL_CRITICAL);
                        }
                        /********************************/
                        /** @todo (rgw#1#): 系统恢复   */
                        /********************************/
                        pPoint->status.xVoltStat = STATE_VOLT_OK;
                    }
                    else
                    {
                        voltstat = HandleVolt(pPoint->status.dChargingVoltage,
                                              pPoint->info.dVolatageLowerLimits + 10,
                                              pPoint->info.dVolatageUpperLimits - 10);
                        switch(voltstat)
                        {
                        case VOLT_OK://200~240
                            break;
                        case VOLT_LOWER:
                            xTimerStop(pPoint->status.xHandleTimerVolt, 0);
                            xTimerDelete(pPoint->status.xHandleTimerVolt, 0);
                            pPoint->status.xVoltStat = STATE_VOLT_LOWER;
                            break;
                        case VOLT_UPPER:
                            xTimerStop(pPoint->status.xHandleTimerVolt, 0);
                            xTimerDelete(pPoint->status.xHandleTimerVolt, 0);
                            pPoint->status.xVoltStat = STATE_VOLT_UPPER;
                            break;
                        default:
                            break;
                        }
                    }
                }/* end of switch(pPoint->status.xVoltStat) */
                /* end of 电压判断 */

                /* 电流判断 */

                /** @todo (rgw#1#): 电流应该如何判断？ */

                /* end of 电流判断 */

                /* 频率判断 */

                /** @todo (rgw#1#): 频率需要判断？ */

                /* end of 频率判断 */

            } /* end of for(i = 0; i < ulTotalPoint; i++) */
        }/* end of defEventBitTimerCBChargingData */
        if((uxBits & defEventBitTimerCBEVSEState) == defEventBitTimerCBEVSEState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                if(pEVSE->status.ulScramState == 0)
                {
                    xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitEVSEScramOK);
                }
                else
                {
                    xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitEVSEScramOK);
                }
                if(pEVSE->status.ulPEState == 0)
                {
                    xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitEVSEPEOK);
                }
                else
                {
                    xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitEVSEPEOK);
                }
                if(pEVSE->status.ulKnockState == 0)
                {
                    xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitEVSEKnockOK);
                }
                else
                {
                    xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitEVSEKnockOK);
                }
                if(pEVSE->status.ulArresterState == 0)
                {
                    xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitEVSEArresterOK);
                }
                else
                {
                    xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitEVSEArresterOK);
                }
                if(pEVSE->status.ulPowerOffState == 0)
                {
                    xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitEVSEPowerOffOK);
                }
                else
                {
                    xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitEVSEPowerOffOK);
                }
            }
        }/* end of defEventBitTimerCBEVSEState */
        /* end of 判断状态 */
#if DEBUG_MONITOR
        xprintf("%s\n", TASKNAME_EVSEMonitor);
#endif
        vTaskDelay(10);//要比timer中的检测周期快
    }/* end of while(1)*/
}

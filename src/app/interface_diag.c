/**
* @file interface_diag.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-09
*/
#include "interface.h"
#include "evse_config.h"
#include "evse_globals.h"

typedef enum
{
    VOLT_OK,
    VOLT_LOWER,
    VOLT_UPPER
} HandleVolt_t;
typedef enum
{
    CURR_OK,
    CURR_UPPER,
} HandleCurr_t;

/** @brief 比较温度
 *
 * @param temp double   获取到的温度
 * @param lower double  温度下限
 * @param upper double  温度上限
 * @return ErrorLevel_t
 *
 */
static ErrorLevel_t HandleTemp(double temp, double lower, double upper)
{
    ErrorLevel_t templevel;
    if(temp >= lower &&  temp <= upper )    //-40~105
    {
        if(temp >= upper - defMonitorTempPeriod)      //95~105
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

/** @brief 比较电压
 *
 * @param volt double
 * @param lower double
 * @param upper double
 * @return HandleVolt_t
 *
 */
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
/** @brief 比较电流
 *
 * @param curr double       获取到的电流
 * @param ratecurr double   额定电流
 * @return HandleCurr_t
 *
 */
static HandleCurr_t HandleCurr(double curr, double ratecurr)
{
    HandleCurr_t currstat;
    if(curr <= ratecurr + defMonitorCurrPeriod)
    {
        currstat = CURR_OK;
    }
    else
    {
        currstat = CURR_UPPER;
    }
    return currstat;
}

/** @brief 电压判断 北汽需求 P10 f)过压保护 g)欠压保护
 *
 * @param pPoint ChargePoint_t*
 * @return void
 *
 */
void DiagVoltageError(ChargePoint_t *pPoint)
{
    HandleVolt_t voltstat;
    EventBits_t uxBitsException;
    uint8_t strTimerName[50];
    int i;

    uxBitsException = 0;
    memset(strTimerName, 0, 50);
    i = pPoint->info.ucChargePointID;

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
            pPoint->status.xHandleTimerVolt = xTimerCreate(strTimerName,
                                              defDiagVoltDummyCyc,
                                              pdFALSE,
                                              (void *)i,
                                              vVoltTimerCB);
            xTimerStart(pPoint->status.xHandleTimerVolt, 0);
            pPoint->status.xVoltStat = STATE_VOLT_LOWER_Dummy;
            break;
        case VOLT_UPPER:
            xsprintf(strTimerName, "TimerPoint%d_VoltUp_Dummy", i);
            pPoint->status.xHandleTimerVolt = xTimerCreate(strTimerName,
                                              defDiagVoltDummyCyc,
                                              pdFALSE,
                                              (void *)i,
                                              vVoltTimerCB);
            xTimerStart(pPoint->status.xHandleTimerVolt, 0);
            pPoint->status.xVoltStat = STATE_VOLT_UPPER_Dummy;
            break;
        default:
            break;
        }
        break;
    case STATE_VOLT_LOWER_Dummy:
    case STATE_VOLT_UPPER_Dummy:
        uxBitsException = xEventGroupWaitBits(pPoint->status.xHandleEventException,
                                              defEventBitExceptionVoltTimer,
                                              pdTRUE, pdFALSE, 0);
        if((uxBitsException & defEventBitExceptionVoltTimer) == defEventBitExceptionVoltTimer)
        {
            xTimerDelete(pPoint->status.xHandleTimerVolt, 0);
            xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointVoltOK);
            if(pPoint->state == STATE_POINT_CHARGING)
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
                              pPoint->info.dVolatageLowerLimits + defMonitorVoltPeriod,
                              pPoint->info.dVolatageUpperLimits - defMonitorVoltPeriod);
        switch(voltstat)
        {
        case VOLT_OK://200~240
            xsprintf(strTimerName, "TimerPoint%d_VoltOK_Dummy", i);
            pPoint->status.xHandleTimerVolt = xTimerCreate(strTimerName,
                                              defDiagVoltRecoverCyc,
                                              pdFALSE,
                                              (void *)i,
                                              vVoltTimerCB);
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
        uxBitsException = xEventGroupWaitBits(pPoint->status.xHandleEventException,
                                              defEventBitExceptionVoltTimer,
                                              pdTRUE, pdFALSE, 0);
        if((uxBitsException & defEventBitExceptionVoltTimer) == defEventBitExceptionVoltTimer)
        {
            xTimerDelete(pPoint->status.xHandleTimerVolt, 0);
            xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointVoltOK);
            if(pPoint->state == STATE_POINT_CHARGING)
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
                                  pPoint->info.dVolatageLowerLimits + defMonitorVoltPeriod,
                                  pPoint->info.dVolatageUpperLimits - defMonitorVoltPeriod);
            switch(voltstat)
            {
            case VOLT_OK://200~240
                break;
            case VOLT_LOWER:
                xTimerDelete(pPoint->status.xHandleTimerVolt, 0);
                pPoint->status.xVoltStat = STATE_VOLT_LOWER;
                break;
            case VOLT_UPPER:
                xTimerDelete(pPoint->status.xHandleTimerVolt, 0);
                pPoint->status.xVoltStat = STATE_VOLT_UPPER;
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }/* end of switch(pPoint->status.xVoltStat) */
}
/** @brief 电流判断 GB/T 18487.1-2015 A3.10.7 P24 在此基础上增加一次降功率修正操作
 *
 * @param pPoint ChargePoint_t*
 * @return void
 *
 */
void DiagCurrentError(ChargePoint_t *pPoint)
{
    HandleCurr_t currstat;
    EventBits_t uxBitsCharge;
    EventBits_t uxBitsException;
    uint8_t strTimerName[50];
    int i;

    uxBitsCharge = 0;
    uxBitsException = 0;
    currstat = CURR_OK;
    memset(strTimerName, 0, 50);
    i = pPoint->info.ucChargePointID;

    if(pPoint->state == STATE_POINT_CHARGING)
    {
        currstat = HandleCurr(pPoint->status.dChargingCurrent,
                              pPoint->info.dRatedCurrent);
        switch(pPoint->status.xCurrStat)
        {
        case STATE_CURR_INIT:
            xsprintf(strTimerName, "TimerPoint%d_CurrInit", i);
            pPoint->status.xHandleTimerCurr = xTimerCreate(strTimerName,
                                              defDiagCurrInitCyc,
                                              pdFALSE,
                                              (void *)i,
                                              vCurrTimerCB);
            xTimerStart(pPoint->status.xHandleTimerCurr, 0);
            pPoint->status.xCurrStat = STATE_CURR_DELAY;
            break;
        case STATE_CURR_DELAY:
            uxBitsException = xEventGroupWaitBits(pPoint->status.xHandleEventException,
                                                  defEventBitExceptionCurrTimer,
                                                  pdTRUE, pdFALSE, 0);
            if((uxBitsException & defEventBitExceptionCurrTimer) == defEventBitExceptionCurrTimer)
            {
                xTimerDelete(pPoint->status.xHandleTimerCurr, 0);
                pPoint->status.xCurrStat = STATE_CURR_OK;
            }
            break;
        case STATE_CURR_OK:
            switch(currstat)
            {
            case CURR_OK:
                xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointCurrOK);
                break;
            case CURR_UPPER:
                xsprintf(strTimerName, "TimerPoint%d_CurrUp_Dummy", i);
                pPoint->status.xHandleTimerCurr = xTimerCreate(strTimerName,
                                                  defDiagCurrDummyCyc,
                                                  pdFALSE,
                                                  (void *)i,
                                                  vCurrTimerCB);
                xTimerStart(pPoint->status.xHandleTimerCurr, 0);
                pPoint->status.xCurrStat = STATE_CURR_UPPER_Dummy;
                break;
            default:
                break;
            }
            break;
        case STATE_CURR_UPPER_Dummy:
            uxBitsException = xEventGroupWaitBits(pPoint->status.xHandleEventException,
                                                  defEventBitExceptionCurrTimer,
                                                  pdTRUE, pdFALSE, 0);
            if((uxBitsException & defEventBitExceptionCurrTimer) == defEventBitExceptionCurrTimer)
            {
                xTimerDelete(pPoint->status.xHandleTimerCurr, 0);
                THROW_ERROR(i, pPoint->status.SetLoadPercent(pPoint, 80), ERR_LEVEL_WARNING);
                xsprintf(strTimerName, "TimerPoint%d_CurrUp_Fix", i);
                pPoint->status.xHandleTimerCurr = xTimerCreate(strTimerName,
                                                  defDiagCurrDummyCyc,
                                                  pdFALSE,
                                                  (void *)i,
                                                  vCurrTimerCB);
                xTimerStart(pPoint->status.xHandleTimerCurr, 0);
                pPoint->status.xCurrStat = STATE_CURR_UPPER_Fix;
            }
            else
            {
                switch(currstat)
                {
                case CURR_OK:
                    xTimerDelete(pPoint->status.xHandleTimerCurr, 0);
                    pPoint->status.xCurrStat = STATE_CURR_OK;
                    break;
                case CURR_UPPER:
                    break;
                default:
                    break;
                }
            }
            break;
        case STATE_CURR_UPPER_Fix:
            uxBitsException = xEventGroupWaitBits(pPoint->status.xHandleEventException,
                                                  defEventBitExceptionCurrTimer,
                                                  pdTRUE, pdFALSE, 0);
            if((uxBitsException & defEventBitExceptionCurrTimer) == defEventBitExceptionCurrTimer)
            {
                xTimerDelete(pPoint->status.xHandleTimerCurr, 0);
                /** @todo (rgw#1#): 电流错误，HMI提示拔枪重新操作 */
                xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointCurrOK);
                pPoint->status.xCurrStat = STATE_CURR_ERROR;
            }
            else
            {
                switch(currstat)
                {
                case CURR_OK:
                    xTimerDelete(pPoint->status.xHandleTimerCurr, 0);
                    pPoint->status.xCurrStat = STATE_CURR_OK;
                    break;
                case CURR_UPPER:
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }/*end of switch(xCurrStat)*/
    }/*end of (state == STATE_POINT_CHARGING)*/
    else //进入到STATE_CURR_ERROR前，point的状态应该已经变成非CHARGING状态。
    {
        switch(pPoint->status.xCurrStat)
        {
        case STATE_CURR_ERROR:
            uxBitsCharge = xEventGroupGetBits(pPoint->status.xHandleEventCharge);
            if((uxBitsCharge & defEventBitPointPlugOK) != defEventBitPointPlugOK)
            {
                pPoint->status.xCurrStat = STATE_CURR_INIT;
            }
            break;
        default:
            break;
        }
    }
}
/** @brief B型插座温度与进线温度检测
 *
 * @param pPoint ChargePoint_t*
 * @return void
 *
 */
void DiagTempError(ChargePoint_t *pPoint)
{
    ErrorLevel_t templevel;
    int i;
    i = pPoint->info.ucChargePointID;
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
    else if(pPoint->info.ucConnectorType == defConnectorTypeC) //C型连接没有温度检测点，直接置位OK
    {
        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointConnTempOK);
    }
}
/** @brief 频率判断
 *
 * @param pPoint ChargePoint_t*
 * @return void
 *
 */
void DiagFreqError(ChargePoint_t *pPoint)
{

    if(pPoint->status.dChargingFrequence >= defMonitorFreqLower - defMonitorFreqPeriod &&
            pPoint->status.dChargingFrequence <= defMonitorFreqUpper + defMonitorFreqPeriod )
    {
        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointFreqOK);
    }
    else
    {
        xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointFreqOK);
    }
}
void DiagPlugError(ChargePoint_t *pPoint)
{
    if(pPoint->status.xPlugState == PLUG)
    {
        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointPlugOK);
    }
    else if(pPoint->status.xPlugState == UNPLUG)
    {
        xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointPlugOK);
    }
    if(pPoint->status.xCPState == CP_6V_PWM || pPoint->status.xCPState == CP_6V)
    {
        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointS2Closed);
        xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointS2Opened);
    }
    else
    {
        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointS2Opened);
        xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointS2Closed);
    }
    /** @note (rgw#1#):  CC和CP状态已经在GetPlugState中获取，在TaskCharge中判断*/
}
void DiagLockError(ChargePoint_t *pPoint)
{
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
    else if(pPoint->info.ucConnectorType == defConnectorTypeC) //C型连接没有锁，直接置位OK
    {
        xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointLocked);
    }
}
/** @brief 主要进行启动条件判断
 *
 * @param pPoint ChargePoint_t*
 * @return void
 *
 */
void DiagEVSEError(ChargePoint_t *pPoint)
{
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

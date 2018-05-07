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
typedef enum
{
    FREQ_OK,
    FREQ_LOWER,
    FREQ_UPPER
} HandleFreq_t;
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
    //if(curr <= ratecurr + defMonitorCurrPeriod)
    if(curr <= ratecurr * defMonitorCurrPeriod)
    {
        currstat = CURR_OK;
    }
    else
    {
        currstat = CURR_UPPER;
    }
    return currstat;
}

static HandleFreq_t HandleFreq(double freq, double lower, double upper)
{
    HandleFreq_t freqstat;
    if (freq >= lower && freq <= upper)
    {
        freqstat = FREQ_OK;
    }
    else if (freq > upper)
    {
        freqstat = FREQ_UPPER;
    }
    else if (freq < lower)
    {
        freqstat = FREQ_LOWER;
    }
    return freqstat;
}
/** @brief 电压判断 北汽需求 P10 f)过压保护 g)欠压保护
 *
 * @param pCON CON_t*
 * @return void
 *
 */
void DiagVoltageError(CON_t *pCON)
{
    HandleVolt_t voltstat;
    EventBits_t uxBitsException;
//    uint8_t strTimerName[50];
    int id;

    uxBitsException = 0;
//    memset(strTimerName, 0, 50);
    id = pCON->info.ucCONID;

    voltstat = HandleVolt(pCON->status.dChargingVoltage,
                          pCON->info.dVolatageLowerLimits,
                          pCON->info.dVolatageUpperLimits);

    switch(pCON->status.xVoltStat)
    {
    case STATE_VOLT_OK:
        pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_VoltUp;
        pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_VoltLow;
        switch(voltstat)
        {
        case VOLT_OK:
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONVoltOK);
            break;
        case VOLT_LOWER:
            pCON->status.xHandleTimerVolt = xTimerCreate("TimerCON_VoltLow_Dummy",
                                              defDiagVoltDummyCyc,
                                              pdFALSE,
                                              (void *)id,
                                              vVoltTimerCB);
            xTimerStart(pCON->status.xHandleTimerVolt, 100);
            pCON->status.xVoltStat = STATE_VOLT_LOWER_Dummy;
            break;
        case VOLT_UPPER:
            pCON->status.xHandleTimerVolt = xTimerCreate("TimerCON_VoltUp_Dummy",
                                              defDiagVoltDummyCyc,
                                              pdFALSE,
                                              (void *)id,
                                              vVoltTimerCB);
            xTimerStart(pCON->status.xHandleTimerVolt, 100);
            pCON->status.xVoltStat = STATE_VOLT_UPPER_Dummy;
            break;
        default:
            break;
        }
        break;
    case STATE_VOLT_LOWER_Dummy:
    case STATE_VOLT_UPPER_Dummy:
        uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                              defEventBitExceptionVoltTimer,
                                              pdTRUE, pdFALSE, 0);
        if((uxBitsException & defEventBitExceptionVoltTimer) == defEventBitExceptionVoltTimer)
        {
            xTimerDelete(pCON->status.xHandleTimerVolt, 100);
            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONVoltOK);
            if(pCON->state == STATE_CON_CHARGING)
            {
                THROW_ERROR(id, pCON->status.SetRelay(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL, "DiagVolt");
            }
            /********************************/
            /** @todo (rgw#1#): 向系统告警 */
            /********************************/
            if(pCON->status.xVoltStat == STATE_VOLT_LOWER_Dummy)
            {
                pCON->status.xVoltStat = STATE_VOLT_LOWER;
            }
            if(pCON->status.xVoltStat == STATE_VOLT_UPPER_Dummy)
            {
                pCON->status.xVoltStat = STATE_VOLT_UPPER;
            }
        }
        else
        {
            switch(voltstat)
            {
            case VOLT_OK:
                xTimerDelete(pCON->status.xHandleTimerVolt, 100);
                pCON->status.xVoltStat = STATE_VOLT_OK;
                break;
            case VOLT_LOWER:
                if(pCON->status.xVoltStat == STATE_VOLT_UPPER_Dummy)
                {
                    xTimerReset(pCON->status.xHandleTimerVolt, 0);
                    pCON->status.xVoltStat = STATE_VOLT_LOWER_Dummy;
                }
                break;
            case VOLT_UPPER:
                if(pCON->status.xVoltStat == STATE_VOLT_LOWER_Dummy)
                {
                    xTimerReset(pCON->status.xHandleTimerVolt, 0);
                    pCON->status.xVoltStat = STATE_VOLT_UPPER_Dummy;
                }
                break;
            default:
                break;
            }
        }
        break;
    case STATE_VOLT_LOWER:
    case STATE_VOLT_UPPER:
        voltstat = HandleVolt(pCON->status.dChargingVoltage,
                              pCON->info.dVolatageLowerLimits + defMonitorVoltPeriod,
                              pCON->info.dVolatageUpperLimits - defMonitorVoltPeriod);
        switch(voltstat)
        {
        case VOLT_OK://200~240
            pCON->status.xHandleTimerVolt = xTimerCreate("TimerCON_VoltOK_Dummy",
                                              defDiagVoltRecoverCyc,
                                              pdFALSE,
                                              (void *)id,
                                              vVoltTimerCB);
            xTimerStart(pCON->status.xHandleTimerVolt, 100);
            pCON->status.xVoltStat = STATE_VOLT_OK_Dummy;
            break;
        case VOLT_LOWER:
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_VoltUp;
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_AC_A_VoltLow;
            break;
        case VOLT_UPPER:
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_AC_A_VoltUp;
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_VoltLow;
            break;
        default:
            break;
        }
        break;
    case STATE_VOLT_OK_Dummy:
        uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                              defEventBitExceptionVoltTimer,
                                              pdTRUE, pdFALSE, 0);
        if((uxBitsException & defEventBitExceptionVoltTimer) == defEventBitExceptionVoltTimer)
        {
            xTimerDelete(pCON->status.xHandleTimerVolt, 100);
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONVoltOK);
            if(pCON->state == STATE_CON_CHARGING)
            {
                THROW_ERROR(id, pCON->status.SetRelay(pCON, SWITCH_ON), ERR_LEVEL_CRITICAL, "DiagVolt");
            }
            /********************************/
            /** @todo (rgw#1#): 系统恢复   */
            /********************************/
            pCON->status.xVoltStat = STATE_VOLT_OK;
        }
        else
        {
            voltstat = HandleVolt(pCON->status.dChargingVoltage,
                                  pCON->info.dVolatageLowerLimits + defMonitorVoltPeriod,
                                  pCON->info.dVolatageUpperLimits - defMonitorVoltPeriod);
            switch(voltstat)
            {
            case VOLT_OK://200~240
                break;
            case VOLT_LOWER:
                xTimerDelete(pCON->status.xHandleTimerVolt, 100);
                pCON->status.xVoltStat = STATE_VOLT_LOWER;
                break;
            case VOLT_UPPER:
                xTimerDelete(pCON->status.xHandleTimerVolt, 100);
                pCON->status.xVoltStat = STATE_VOLT_UPPER;
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }/* end of switch(pCON->status.xVoltStat) */
}
/** @brief 电流判断 GB/T 18487.1-2015 A3.10.7 (P24) 在此基础上增加一次降功率修正操作
 *
 * @param pCON CON_t*
 * @return void
 *
 */
void DiagCurrentError(CON_t *pCON)
{
    HandleCurr_t currstat;
    EventBits_t uxBitsCharge;
    EventBits_t uxBitsException;
//    uint8_t strTimerName[50];
    int id;

    uxBitsCharge = 0;
    uxBitsException = 0;
    currstat = CURR_OK;
//    memset(strTimerName, 0, 50);
    id = pCON->info.ucCONID;

    {
        currstat = HandleCurr(pCON->status.dChargingCurrent,
                              pCON->info.dRatedCurrent);
        switch(pCON->status.xCurrStat)
        {
        case STATE_CURR_INIT:
            pCON->status.xHandleTimerCurr = xTimerCreate("TimerCON_CurrInit",
                                              defDiagCurrInitCyc,
                                              pdFALSE,
                                              (void *)id,
                                              vCurrTimerCB);
            xTimerStart(pCON->status.xHandleTimerCurr, 100);
            pCON->status.xCurrStat = STATE_CURR_DELAY;
            break;
        case STATE_CURR_DELAY:
            uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                                  defEventBitExceptionCurrTimer,
                                                  pdTRUE, pdTRUE, 0);
            if((uxBitsException & defEventBitExceptionCurrTimer) == defEventBitExceptionCurrTimer)
            {
                xTimerDelete(pCON->status.xHandleTimerCurr, 100);
                pCON->status.xCurrStat = STATE_CURR_OK;
            }
            break;
        case STATE_CURR_OK:
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_CurrUp_War;
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_CurrUp_Cri;
            switch(currstat)
            {
            case CURR_OK:
                xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONCurrOK);
                break;
            case CURR_UPPER:
                pCON->status.SetCPSwitch(pCON, SWITCH_ON);//过流可能是由于修改的额定电流，先按照修改的额定电流设定一次CP
                pCON->status.xHandleTimerCurr = xTimerCreate("TimerCON_CurrUp_Dummy",
                                                  defDiagCurrDummyCyc,
                                                  pdFALSE,
                                                  (void *)id,
                                                  vCurrTimerCB);
                xTimerStart(pCON->status.xHandleTimerCurr, 100);
                pCON->status.xCurrStat = STATE_CURR_UPPER_Dummy;
                break;
            default:
                break;
            }
            break;
        case STATE_CURR_UPPER_Dummy:
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_AC_A_CurrUp_War;
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_CurrUp_Cri;
     
            uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                                  defEventBitExceptionCurrTimer,
                                                  pdTRUE, pdTRUE, 0);
            if((uxBitsException & defEventBitExceptionCurrTimer) == defEventBitExceptionCurrTimer)
            {
                xTimerDelete(pCON->status.xHandleTimerCurr, 100);
                THROW_ERROR(id, pCON->status.SetLoadPercent(pCON, 50), ERR_LEVEL_WARNING, "DiagCurr SetLoad");
                pCON->status.xHandleTimerCurr = xTimerCreate("TimerCON_CurrUp_Fix",
                                                  defDiagCurrDummyCyc,
                                                  pdFALSE,
                                                  (void *)id,
                                                  vCurrTimerCB);
                xTimerStart(pCON->status.xHandleTimerCurr, 100);
                pCON->status.xCurrStat = STATE_CURR_UPPER_Fix;
                break;
            }
            else
            {
                switch(currstat)
                {
                case CURR_OK:
                    xTimerDelete(pCON->status.xHandleTimerCurr, 100);
                    pCON->status.xCurrStat = STATE_CURR_OK;
                    break;
                case CURR_UPPER:
                    break;
                default:
                    break;
                }
            }
            break;
        case STATE_CURR_UPPER_Fix:
            uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                                  defEventBitExceptionCurrTimer,
                                                  pdTRUE, pdTRUE, 0);
            if((uxBitsException & defEventBitExceptionCurrTimer) == defEventBitExceptionCurrTimer)
            {
                xTimerDelete(pCON->status.xHandleTimerCurr, 100);
                /** @todo (rgw#1#): 电流错误，HMI提示拔枪重新操作 */
                xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONCurrOK);
                pCON->status.xCurrStat = STATE_CURR_ERROR;
                break;
            }
            else
            {
                switch(currstat)
                {
                case CURR_OK:
                    xTimerDelete(pCON->status.xHandleTimerCurr, 100);
                    pCON->status.xCurrStat = STATE_CURR_OK;
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
    }

    {//进入到STATE_CURR_ERROR前，connector的状态应该已经变成非CHARGING状态。
        switch(pCON->status.xCurrStat)
        {
        case STATE_CURR_ERROR:
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_CurrUp_War;
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_AC_A_CurrUp_Cri;
            uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
            if((uxBitsCharge & defEventBitCONPlugOK) != defEventBitCONPlugOK)
            {
                if (currstat != CURR_UPPER)
                    pCON->status.xCurrStat = STATE_CURR_INIT;
            }
            break;
        default:
            break;
        }
    }
}
/** @brief B型插座温度与进线温度检测
 *
 * @param pCON CON_t*
 * @return void
 *
 */
void DiagEVSETempError(EVSE_t *pEVSE)
{
    CON_t *pCON;
    int i;
    ErrorLevel_t templevel;
    ErrorLevel_t templevel_EVSE_A;
    ErrorLevel_t templevel_EVSE_B;
    ErrorLevel_t templevel_EVSE_C;
    ErrorLevel_t templevel_EVSE_N;

    {
        templevel_EVSE_A = HandleTemp(pEVSE->status.dAC_A_Temp_IN,
            pEVSE->info.dACTempLowerLimits,
            pEVSE->info.dACTempUpperLimits);
        switch (templevel_EVSE_A)
        {
        case ERR_LEVEL_OK:
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_A_Temp_War;
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_A_Temp_Cri;
            break;
        case ERR_LEVEL_WARNING:
            pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_AC_A_Temp_War;
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_A_Temp_Cri;
            break;
        case ERR_LEVEL_CRITICAL:
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_A_Temp_War;
            pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_AC_A_Temp_Cri;
            break;
        default:
            break;
        }
    }
    {
        templevel_EVSE_B = HandleTemp(pEVSE->status.dAC_B_Temp_IN,
            pEVSE->info.dACTempLowerLimits,
            pEVSE->info.dACTempUpperLimits);
        switch (templevel_EVSE_B)
        {
        case ERR_LEVEL_OK:
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_B_Temp_War;
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_B_Temp_Cri;
            break;
        case ERR_LEVEL_WARNING:
            pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_AC_B_Temp_War;
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_B_Temp_Cri;
            break;
        case ERR_LEVEL_CRITICAL:
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_B_Temp_War;
            pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_AC_B_Temp_Cri;
            break;
        default:
            break;
        }
    }
    {
        templevel_EVSE_C = HandleTemp(pEVSE->status.dAC_C_Temp_IN,
            pEVSE->info.dACTempLowerLimits,
            pEVSE->info.dACTempUpperLimits);
        switch (templevel_EVSE_C)
        {
        case ERR_LEVEL_OK:
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_C_Temp_War;
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_C_Temp_Cri;
            break;
        case ERR_LEVEL_WARNING:
            pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_AC_C_Temp_War;
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_C_Temp_Cri;
            break;
        case ERR_LEVEL_CRITICAL:
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_C_Temp_War;
            pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_AC_C_Temp_Cri;
            break;
        default:
            break;
        }
    }
    {
        templevel_EVSE_N = HandleTemp(pEVSE->status.dAC_N_Temp_IN,
            pEVSE->info.dACTempLowerLimits,
            pEVSE->info.dACTempUpperLimits);
        switch (templevel_EVSE_N)
        {
        case ERR_LEVEL_OK:
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_N_Temp_War;
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_N_Temp_Cri;
            break;
        case ERR_LEVEL_WARNING:
            pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_AC_N_Temp_War;
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_N_Temp_Cri;
            break;
        case ERR_LEVEL_CRITICAL:
            pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_AC_N_Temp_War;
            pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_AC_N_Temp_Cri;
            break;
        default:
            break;
        }
    }
    for(i = 0; i < pEVSE->info.ucTotalCON; i++)
    {  
        pCON = CONGetHandle(i);
        if ((pEVSE->status.ulSignalAlarm & defSignalGroupEVSE_Alarm_Temp_Cri) != 0)
        {   //控制模块控制充电桩停机，断开AC输出，并跳转S1开关，CP信号保持高电平输出
            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitEVSETempOK);
        }
        else if ((pEVSE->status.ulSignalAlarm & defSignalGroupEVSE_Alarm_Temp_War) != 0)
        {
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitEVSETempOK);
        }
        else
        {
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitEVSETempOK);
        }
    }
}
/** @brief B型插座温度与进线温度检测
 *
 * @param pCON CON_t*
 * @return void
 *
 */
void DiagCONTempError(CON_t *pCON)
{
    ErrorLevel_t templevel;
    ErrorLevel_t templevel_CON_A;
    ErrorLevel_t templevel_CON_B;
    ErrorLevel_t templevel_CON_C;
    ErrorLevel_t templevel_CON_N;

    {
        templevel_CON_A = HandleTemp(pCON->status.dACLTemp,
            pCON->info.dACTempLowerLimits,
            pCON->info.dACTempUpperLimits);
        switch (templevel_CON_A)
        {
        case ERR_LEVEL_OK:
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_Temp_War;
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_Temp_Cri;
            break;
        case ERR_LEVEL_WARNING:
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_AC_A_Temp_War;
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_Temp_Cri;
            break;
        case ERR_LEVEL_CRITICAL:
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_A_Temp_War;
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_AC_A_Temp_Cri;
            break;
        default:
            break;
        }
    }
//    templevel_CON_B = HandleTemp(pCON->status.dACLTemp,
//                           pCON->info.dACTempLowerLimits,
//                           pCON->info.dACTempUpperLimits);
//    templevel_CON_C = HandleTemp(pCON->status.dACLTemp,
//                           pCON->info.dACTempLowerLimits,
//                           pCON->info.dACTempUpperLimits);
    {
        templevel_CON_N = HandleTemp(pCON->status.dACNTemp,
            pCON->info.dACTempLowerLimits,
            pCON->info.dACTempUpperLimits);
        switch (templevel_CON_N)
        {
        case ERR_LEVEL_OK:
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_N_Temp_War;
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_N_Temp_Cri;
            break;
        case ERR_LEVEL_WARNING:
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_AC_N_Temp_War;
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_N_Temp_Cri;
            break;
        case ERR_LEVEL_CRITICAL:
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_N_Temp_War;
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_AC_N_Temp_Cri;
            break;
        default:
            break;
        }
    }

    if(pCON->info.ucSocketType == defSocketTypeB)
    {
        /* SocketTemp1 */
        templevel = HandleTemp(pCON->status.dBTypeSocketTemp1,
                               pCON->info.dSocketTempLowerLimits,
                               pCON->info.dSocketTempUpperLimits);
        switch(templevel)
        {
        case ERR_LEVEL_OK:
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp1_War;
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp1_Cri;
            break;
        case ERR_LEVEL_WARNING:
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_SocketTemp1_War;
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp1_Cri;
            break;
        case ERR_LEVEL_CRITICAL:
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp1_War;
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_SocketTemp1_Cri;
            break;
        default:
            break;
        }
        /* end of SocketTemp1 */

        /* SocketTemp2 */
        templevel = HandleTemp(pCON->status.dBTypeSocketTemp2,
                               pCON->info.dSocketTempLowerLimits,
                               pCON->info.dSocketTempUpperLimits);
        switch(templevel)
        {
        case ERR_LEVEL_OK:
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp2_War;
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp2_Cri;
            break;
        case ERR_LEVEL_WARNING:
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_SocketTemp2_War;
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp2_Cri;
            break;
        case ERR_LEVEL_CRITICAL:
            pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp2_War;
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_SocketTemp2_Cri;
            break;
        default:
            break;
        }
        /* end of SocketTemp2 */
    }
    else if(pCON->info.ucSocketType == defSocketTypeC) //C型连接没有温度检测点，直接置位OK
    {
        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONSocketTempOK);
        pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp1_War;
        pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp1_Cri;
        pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp2_War;
        pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_SocketTemp2_Cri;
    }
    
    {  
        if ((pCON->status.ulSignalAlarm & defSignalGroupCON_Alarm_Temp_Cri) != 0)
        {   //控制模块控制充电桩停机，断开AC输出，并跳转S1开关，CP信号保持高电平输出
            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONACTempOK);
        }
        else if ((pCON->status.ulSignalAlarm & defSignalGroupCON_Alarm_Temp_War) != 0)
        {
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONACTempOK);
        }
        else
        {
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONACTempOK);
        }
    }
}
/** @brief 频率判断
 *
 * @param pCON CON_t*
 * @return void
 *
 */
void DiagFreqError(CON_t *pCON)
{
    HandleFreq_t freqstat;
    EventBits_t uxBitsException;
    int id;
    
    uxBitsException = 0;
    id = pCON->info.ucCONID;

    freqstat = HandleFreq(pCON->status.dChargingFrequence,
        defMonitorFreqLower,
        defMonitorFreqUpper);

    switch (pCON->status.xFreqStat)
    {
    case STATE_FREQ_OK:
        pCON->status.ulSignalAlarm &= ~defSignalCON_Alarm_AC_Freq_Cri;
        switch (freqstat)
        {
        case FREQ_OK:
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONFreqOK);
            break;
        case FREQ_LOWER:
            pCON->status.xHandleTimerFreq = xTimerCreate("TimerCON_FreqLow_Dummy",
                defDiagFreqDummyCyc,
                pdFALSE,
                (void *)id,
                vFreqTimerCB);
            xTimerStart(pCON->status.xHandleTimerFreq, 100);
            pCON->status.xFreqStat = STATE_FREQ_LOWER_Dummy;
            break;
        case FREQ_UPPER:
            pCON->status.xHandleTimerFreq = xTimerCreate("TimerCON_FreqUp_Dummy",
                defDiagFreqDummyCyc,
                pdFALSE,
                (void *)id,
                vFreqTimerCB);
            xTimerStart(pCON->status.xHandleTimerFreq, 100);
            pCON->status.xFreqStat = STATE_FREQ_UPPER_Dummy;
            break;
        default:
            break;
        }
        break;
    case STATE_FREQ_LOWER_Dummy:
    case STATE_FREQ_UPPER_Dummy:
        uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
            defEventBitExceptionFreqTimer,
            pdTRUE,
            pdFALSE,
            0);
        if ((uxBitsException & defEventBitExceptionFreqTimer) == defEventBitExceptionFreqTimer)
        {
            xTimerDelete(pCON->status.xHandleTimerFreq, 100);
            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONFreqOK);
            if (pCON->status.xFreqStat == STATE_FREQ_LOWER_Dummy)
            {
                pCON->status.xFreqStat = STATE_FREQ_LOWER;
            }
            if (pCON->status.xFreqStat == STATE_FREQ_UPPER_Dummy)
            {
                pCON->status.xFreqStat = STATE_FREQ_UPPER;
            }
        }
        else
        {
            switch (freqstat)
            {
            case FREQ_OK:
                xTimerDelete(pCON->status.xHandleTimerFreq, 100);
                pCON->status.xFreqStat = STATE_FREQ_OK;
                break;
            case FREQ_LOWER:
                if (pCON->status.xFreqStat == STATE_FREQ_UPPER_Dummy)
                {
                    xTimerReset(pCON->status.xHandleTimerFreq, 0);
                    pCON->status.xFreqStat = STATE_FREQ_LOWER_Dummy;
                }
                break;
            case FREQ_UPPER:
                if (pCON->status.xFreqStat == STATE_FREQ_LOWER_Dummy)
                {
                    xTimerReset(pCON->status.xHandleTimerFreq, 0);
                    pCON->status.xFreqStat = STATE_FREQ_UPPER_Dummy;
                }
                break;
            default:
                break;
            }
        }
        break;
    case STATE_FREQ_LOWER:
    case STATE_FREQ_UPPER:
        freqstat = HandleFreq(pCON->status.dChargingFrequence,
            defMonitorFreqLower + defMonitorFreqPeriod,
            defMonitorFreqUpper - defMonitorFreqPeriod);
        switch (freqstat)
        {
        case FREQ_OK://49.5~50.5
            pCON->status.xHandleTimerFreq = xTimerCreate("TimerCON_FreqOK_Dummy",
                defDiagFreqDummyCyc,
                pdFALSE,
                (void *)id,
                vFreqTimerCB);
            xTimerStart(pCON->status.xHandleTimerFreq, 100);
            pCON->status.xFreqStat = STATE_FREQ_OK_Dummy;
            break;
        case FREQ_LOWER:
        case FREQ_UPPER:
            pCON->status.ulSignalAlarm |= defSignalCON_Alarm_AC_Freq_Cri;
            break;
        default:
            break;
        }
        break;
    case STATE_FREQ_OK_Dummy:
        uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
            defEventBitExceptionFreqTimer,
            pdTRUE,
            pdFALSE,
            0);
        if ((uxBitsException & defEventBitExceptionFreqTimer) == defEventBitExceptionFreqTimer)
        {
            xTimerDelete(pCON->status.xHandleTimerFreq, 100);
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONFreqOK);
            pCON->status.xFreqStat = STATE_FREQ_OK;
        }
        else
        {
            freqstat = HandleFreq(pCON->status.dChargingFrequence,
                defMonitorFreqLower + defMonitorFreqPeriod,
                defMonitorFreqUpper - defMonitorFreqPeriod);
            switch (freqstat)
            {
            case FREQ_OK://49.5~50.5
                break;
            case FREQ_LOWER:
                xTimerDelete(pCON->status.xHandleTimerFreq, 100);
                pCON->status.xFreqStat = STATE_FREQ_LOWER;
                break;
            case FREQ_UPPER:
                xTimerDelete(pCON->status.xHandleTimerFreq, 100);
                pCON->status.xFreqStat = STATE_FREQ_UPPER;
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }/* end of switch(pCON->status.xFreqStat) */
}
void DiagPlugError(CON_t *pCON)
{
    if(pCON->status.xPlugState == PLUG)
    {
        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONPlugOK);
        pCON->status.ulSignalState |= defSignalCON_State_Plug;
    }
    else if(pCON->status.xPlugState == UNPLUG)
    {
        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONPlugOK);
        pCON->status.ulSignalState &= ~defSignalCON_State_Plug;
    }
	
    if(pCON->status.xCPState == CP_6V_PWM || pCON->status.xCPState == CP_6V)
    {
        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONS2Closed);
        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONS2Opened);
        pCON->status.ulSignalState |= defSignalCON_State_S2;
    }
    else
    {
        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONS2Opened);
        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONS2Closed);
        pCON->status.ulSignalState &= ~defSignalCON_State_S2;
    }
    /** (rgw#1#):  CC和CP状态已经在GetPlugState中获取，在TaskCharge中判断*/
}
void DiagLockError(CON_t *pCON)
{
    if(pCON->info.ucSocketType == defSocketTypeB)
    {
        if(pCON->status.xBTypeSocketLockState == LOCK)
        {
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONLocked);
            pCON->status.ulSignalState |= defSignalCON_State_SocketLock;
        }
        else
        {
            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONLocked);
            pCON->status.ulSignalState &= ~defSignalCON_State_SocketLock;
        }
    }
    else if(pCON->info.ucSocketType == defSocketTypeC) //C型连接没有锁，直接置位OK
    {
        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONLocked);
        pCON->status.ulSignalState |= defSignalCON_State_SocketLock;
    }
}
/** @brief 主要进行启动条件判断
 *
 * @param pCON CON_t*
 * @return void
 *
 */
void DiagEVSEError(CON_t *pCON)
{
    if(pEVSE->status.ulScramState == 0)
    {
        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitEVSEScramOK);
        pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_Scram;
    }
    else
    {
        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitEVSEScramOK);
        pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_Scram;
    }
    if(pEVSE->status.ulPEState == 0)
    {
        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitEVSEPEOK);
        pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_PE;
    }
    else
    {
        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitEVSEPEOK);
        pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_PE;
    }
    if(pEVSE->status.ulKnockState == 0)
    {
        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitEVSEKnockOK);
        pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_Knock;
    }
    else
    {
        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitEVSEKnockOK);
        pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_Knock;
    }
    if(pEVSE->status.ulArresterState == 0)
    {
        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitEVSEArresterOK);
        pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_Arrester;
    }
    else
    {
        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitEVSEArresterOK);
        pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_Arrester;
    }
    if(pEVSE->status.ulPowerOffState == 0)
    {
        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitEVSEPowerOffOK);
        pEVSE->status.ulSignalAlarm &= ~defSignalEVSE_Alarm_PowerOff;
    }
    else
    {
        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitEVSEPowerOffOK);
        pEVSE->status.ulSignalAlarm |= defSignalEVSE_Alarm_PowerOff;
    }
}

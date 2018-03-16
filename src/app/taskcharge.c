/**
* @file taskcharge.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskcharge.h"
#include "interface.h"

//#define DEBUG_NO_TASKCHARGE

static void SetCONSignalWorkState(CON_t *pCON, uint32_t signal)
{
    switch (signal)
    {
        case defSignalCON_State_Standby:
            pCON->status.ulSignalState |= defSignalCON_State_Standby;
            pCON->status.ulSignalState &= ~defSignalCON_State_Working;
            pCON->status.ulSignalState &= ~defSignalCON_State_Stopping;
            pCON->status.ulSignalState &= ~defSignalCON_State_Fault;
            break;
        case defSignalCON_State_Working:
            pCON->status.ulSignalState &= ~defSignalCON_State_Standby;
            pCON->status.ulSignalState |= defSignalCON_State_Working;
            pCON->status.ulSignalState &= ~defSignalCON_State_Stopping;
            pCON->status.ulSignalState &= ~defSignalCON_State_Fault;
            break;
        case defSignalCON_State_Stopping:
            pCON->status.ulSignalState &= ~defSignalCON_State_Standby;
            pCON->status.ulSignalState &= ~defSignalCON_State_Working;
            pCON->status.ulSignalState |= defSignalCON_State_Stopping;
            pCON->status.ulSignalState &= ~defSignalCON_State_Fault;
            break;
        case defSignalCON_State_Fault:
            pCON->status.ulSignalState &= ~defSignalCON_State_Standby;
            pCON->status.ulSignalState &= ~defSignalCON_State_Working;
            pCON->status.ulSignalState &= ~defSignalCON_State_Stopping;
            pCON->status.ulSignalState |= defSignalCON_State_Fault;
            break;
    }

}

int manual_charge(void *pvCON, int onoff)
{
    CON_t *pCON;
    EventBits_t uxBit;
    int timecont = 0;
    
    pCON = (CON_t *)pvCON;
    
    if (onoff > 0)
    {
        if ((pCON->status.ulSignalState & defSignalCON_State_Standby) == defSignalCON_State_Standby)
        {
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
            uxBit = xEventGroupWaitBits(pCON->status.xHandleEventCharge, defEventBitCONStartOK, pdFALSE, pdTRUE, 10000);
            if ((uxBit & defEventBitCONStartOK) == defEventBitCONStartOK)
            {
                return 1;
            }
        }
        else
        {
            return 0;
        }
    }
    else if (onoff == 0)
    {
        if ((pCON->status.ulSignalState & defSignalCON_State_Working) == defSignalCON_State_Working)
        {
            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
            timecont = 0;
            while (pCON->state == STATE_CON_CHARGING)
            {
                timecont++;
                if (timecont > 100)
                {
                    return 0;
                }
                vTaskDelay(100);
            }
            return 1;
        }
        else
        {
            return 0;
        }
    }

    return 0;
}

void vTaskEVSECharge(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBitsCharge;
    EventBits_t uxBitsException;
    uint8_t stop_try;
    uint8_t unlock_try;
    uint8_t dev_err;
    ErrorCode_t errcode;

    ulTotalCON = pListCON->Total;
    uxBitsCharge = 0;
    uxBitsException = 0;
    stop_try = 0;
    unlock_try = 0;
    dev_err = 0;
    errcode = ERR_NO;

    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        THROW_ERROR(i, errcode = pCON->status.GetRelayState(pCON), ERR_LEVEL_CRITICAL, "Charge init");
        if(pCON->status.ucRelayLState == SWITCH_ON &&
                pCON->status.ucRelayNState == SWITCH_ON)
        {
            THROW_ERROR(i, pCON->status.SetRelay(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL, "Charge init");
        }
        if(pCON->info.ucSocketType == defSocketTypeB)
        {
            THROW_ERROR(i, pCON->status.GetBTypeSocketLock(pCON), ERR_LEVEL_CRITICAL, "Charge init");
            if(pCON->status.xBTypeSocketLockState == LOCK &&
                    pCON->status.xBTypeSocketLockState == LOCK)
            {
                THROW_ERROR(i, pCON->status.SetBTypeSocketLock(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL, "Charge init");
            }
        }
    }

    while(1)
    {
#ifndef DEBUG_NO_TASKCHARGE
        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            switch(pCON->state)
            {
            case STATE_CON_IDLE://状态1
                SetCONSignalWorkState(pCON, defSignalCON_State_Standby);
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                   defEventBitCONPlugOK,
                                                   pdFALSE, pdFALSE, 0);
                if((uxBitsCharge & defEventBitCONPlugOK) == defEventBitCONPlugOK)
                {
                    pCON->state = STATE_CON_PLUGED;
                }
                break;
            case STATE_CON_PLUGED://状态2
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                   defEventBitCPSwitchCondition,
                                                   pdFALSE, pdTRUE, 0);
                if((uxBitsCharge & defEventBitCPSwitchCondition) == defEventBitCPSwitchCondition)
                {
                    pCON->status.SetLoadPercent(pCON, pCON->status.ucLoadPercent);
                    THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_ON), ERR_LEVEL_CRITICAL, "STATE_CON_PLUGED");
                    vTaskDelay(defRelayDelay);
	                if ((pCON->status.xCPState == CP_9V_PWM || pCON->status.xCPState == CP_6V_PWM)//后一种情况适用于无S2车辆, 即S1闭合后直接进入6V_PWM状态。
                       &&(pCON->status.xCPState != CP_12V_PWM)) 
                    {
                        pCON->state = STATE_CON_PRECONTRACT;
                    }
                }
                if((uxBitsCharge & defEventBitCONPlugOK) != defEventBitCONPlugOK)
                {
                    pCON->state = STATE_CON_RETURN;
                }

//                if((uxBitsCharge & defEventBitCONVoltOK) != defEventBitCONVoltOK)
//                {
//                    printf_safe("插枪状态下CPSwitch失败：电压不对：%.2lf\n", pCON->status.dChargingVoltage);
//                }
//                if((uxBitsCharge & defEventBitCONSocketTempOK) != defEventBitCONSocketTempOK)
//                {
//                    printf_safe("插枪状态下CPSwitch失败：插座温度不对：1:%.2lf 2:%.2lf\n", pCON->status.dBTypeSocketTemp1, pCON->status.dBTypeSocketTemp2);
//                }
//                if((uxBitsCharge & defEventBitCONACTempOK) != defEventBitCONACTempOK)
//                {
//                    printf_safe("插枪状态下CPSwitch失败：AC温度不对：L:%.2lf N:%.2lf\n", pCON->status.dACLTemp, pCON->status.dACNTemp);
//                }
//                if((uxBitsCharge & defEventBitEVSEScramOK) != defEventBitEVSEScramOK)
//                {
//                    printf_safe("插枪状态下CPSwitch失败：急停告警\n");
//                }
//                if((uxBitsCharge & defEventBitEVSEPEOK) != defEventBitEVSEPEOK)
//                {
//                    printf_safe("插枪状态下CPSwitch失败：PE告警\n");
//                }
//                if((uxBitsCharge & defEventBitEVSEKnockOK) != defEventBitEVSEKnockOK)
//                {
//                    printf_safe("插枪状态下CPSwitch失败：撞击告警\n");
//                }
//                if((uxBitsCharge & defEventBitEVSEArresterOK) != defEventBitEVSEArresterOK)
//                {
//                     printf_safe("插枪状态下CPSwitch失败：防雷告警\n");
//                }
//                if((uxBitsCharge & defEventBitEVSEPowerOffOK) != defEventBitEVSEPowerOffOK)
//                {
//                     printf_safe("插枪状态下CPSwitch失败：停电告警\n");
//                }
                break;
            case STATE_CON_PRECONTRACT://状态2' 充电设备准备就绪，等待车的S2，由车辆决定，可用于预约充电等
		        //---预备充电过程中对异常进行检测
				uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
					defEventBitCPSwitchCondition,
					pdFALSE, pdTRUE, 0);
	            if ((uxBitsCharge & defEventBitCPSwitchCondition) == defEventBitCPSwitchCondition)
	            {
	            }
	            else
	            {
		            pCON->state = STATE_CON_RETURN;
		            break;
	            }
	            //////
                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if((uxBitsCharge & defEventBitCONS2Closed) == defEventBitCONS2Closed)
                {
                    pCON->state = STATE_CON_STARTCHARGE;
                }
                if((uxBitsCharge & defEventBitCONPlugOK) != defEventBitCONPlugOK) //状态1'触发条件
                {
                    pCON->status.xHandleTimerCharge = xTimerCreate("TimerCON_Charge_AntiShake",
                                                      defChargeAntiShakeCyc,
                                                      pdFALSE,
                                                      (void *)i,
                                                      vChargeStateTimerCB);
                    xTimerStart(pCON->status.xHandleTimerCharge, 0);
                    pCON->state = STATE_CON_PRECONTRACT_LOSEPLUG;
                }
                break;
            case STATE_CON_PRECONTRACT_LOSEPLUG://状态1' 未连接PWM，充电设备准备好后失去连接
                uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                                      defEventBitExceptionChargeTimer,
                                                      pdTRUE, pdFALSE, 0);
                if((uxBitsException & defEventBitExceptionChargeTimer) == defEventBitExceptionChargeTimer)
                {
                    xTimerDelete(pCON->status.xHandleTimerCharge, 0);
                    if(pCON->status.xCPState == CP_12V)
                    {
                        pCON->state = STATE_CON_RETURN;
                    }
                }
                else
                {
                    uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                    if((uxBitsCharge & defEventBitCONPlugOK) == defEventBitCONPlugOK)
                    {
                        pCON->state = STATE_CON_PRECONTRACT;
                    }
                }
                break;
            case STATE_CON_STARTCHARGE:
	            //---准备充电再次进行检测异常
	            uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
													(defEventBitCPSwitchCondition | defEventBitCONS2Closed),
													pdFALSE, pdTRUE, 0);
	            //fix:增加defEventBitCONS2Closed是因为未刷卡情况下，S2闭合时会直接跳到STATE_CON_STARTCHARGE状态，再打开S2不会退出STARTCHARGE状态。
	            //    因此增加在STATE_CON_STARTCHARGE状态中检测S2状态，闭合后退出状态。
	            if ((uxBitsCharge & (defEventBitCPSwitchCondition | defEventBitCONS2Closed)) == (defEventBitCPSwitchCondition | defEventBitCONS2Closed))
	            {
	            }
	            else
	            {
		            pCON->state = STATE_CON_PRECONTRACT;
		            break;
	            }
	            //end fix
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                   defEventBitCONPlugOK,
                                                   pdFALSE, pdFALSE, 0);
                if((uxBitsCharge & defEventBitCONPlugOK) != defEventBitCONPlugOK)
                {
                    pCON->state = STATE_CON_RETURN;
                    break;
                }
                
                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if((uxBitsCharge & defEventBitCONAuthed) == defEventBitCONAuthed)
                {
                    //锁枪
                    if(pCON->info.ucSocketType == defSocketTypeB)
                    {
                        uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                           defEventBitCONLocked,
                                                           pdFALSE, pdFALSE, 0);
                        if((uxBitsCharge & defEventBitCONLocked) == defEventBitCONLocked)
                        {
                            /** @todo (rgw#1#): HMI */
                        }
                        else
                        {
                            THROW_ERROR(i, pCON->status.SetBTypeSocketLock(pCON, SWITCH_ON), ERR_LEVEL_CRITICAL, "STATE_CON_STARTCHARGE");
                            vTaskDelay(defRelayDelay);
                        }
                    }

                    uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                       defEventBitCONLocked,
                                                       pdFALSE, pdFALSE, 0);
                    if((uxBitsCharge & defEventBitCONLocked) == defEventBitCONLocked)
                    {
                        THROW_ERROR(i, errcode = pCON->status.StartCharge(pCON), ERR_LEVEL_CRITICAL, "STATE_CON_STARTCHARGE");
                        if(errcode == ERR_NO)
                        {
                            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONStartOK);//rfid任务在等待
                            pCON->state = STATE_CON_CHARGING;
                            printf_safe("\e[44;37mStart Charge!\e[0m\n");
                        }
                        /** @todo (rgw#1#): 如果继电器操作失败，转换到ERR状态 */
                    }
                }
                break;
            case STATE_CON_CHARGING:
                SetCONSignalWorkState(pCON, defSignalCON_State_Working);
                uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                                      defEventBitExceptionDevFault,
                                                      pdFALSE, pdFALSE, 0);
                if((uxBitsException & defEventBitExceptionDevFault) != 0)
                {
                    printf_safe("Dev Fault Stop Error!\n");
                    pCON->state = STATE_CON_STOPCHARGE;
                    break;
                }
                /*** 判断用户相关停止条件  ***/
                uxBitsException = xEventGroupGetBits(pCON->status.xHandleEventException);
                if ((uxBitsException & defEventBitExceptionLimitPower) == defEventBitExceptionLimitPower)    //达到充电电量限制
                {
                    printf_safe("LimitPower Stop Charge!\n");
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeLimitPower);
                    xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionLimitPower);
                    xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                }
                if((uxBitsException & defEventBitExceptionLimitFee) == defEventBitExceptionLimitFee)    //达到充电金额限制
                {
                    printf_safe("LimitFee Stop Charge!\n");
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeLimitFee);
                    xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionLimitFee);
                    pCON->state = STATE_CON_STOPCHARGE;
                }
                if ((uxBitsException & defEventBitExceptionLimitTime) == defEventBitExceptionLimitTime)    //达到充电时间限制
                {
                    printf_safe("LimitTime Stop Charge!\n");
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeLimitTime);
                    xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionLimitTime);
                    pCON->state = STATE_CON_STOPCHARGE;
                }
                if((uxBitsException & defEventBitExceptionRemoteStop) == defEventBitExceptionRemoteStop)    //远程停止
                {
                    printf_safe("Remote Stop Charge!\n");
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeRemoteStop);
                    xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionRemoteStop);
                    pCON->state = STATE_CON_STOPCHARGE;
                }
                if((uxBitsException & defEventBitExceptionRFIDStop) == defEventBitExceptionRFIDStop)    //刷卡停止
                {
                    printf_safe("RFID Stop Charge!\n");
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeRFIDStop);
                    xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionRFIDStop);
                    pCON->state = STATE_CON_STOPCHARGE;
                }
                if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_CurrUp_Cri) == defSignalCON_Alarm_AC_A_CurrUp_Cri)
                {
                    printf_safe("Curr Stop Charge!\n");
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeCurr);
                    pCON->state = STATE_CON_STOPCHARGE;
                }
                if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_Scram) == defSignalEVSE_Alarm_Scram)
                {
                    printf_safe("Scram Stop Charge!\n");
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeScram);
                    pCON->state = STATE_CON_STOPCHARGE;
                }
                /******************************/

                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if (((uxBitsCharge & defEventBitCONS2Opened) == defEventBitCONS2Opened) && 
                    ((uxBitsCharge & defEventBitCONPlugOK) == defEventBitCONPlugOK)) //6vpwm->9vpwm S2主动断开
                {
                    printf_safe("\e[44;37mS2 Opened, Full!\e[0m\n");
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeFull);
                    pCON->state = STATE_CON_STOPCHARGE;
                }
                else if(((uxBitsCharge & (defEventBitChargeCondition)) | defEventBitCONVoltOK) != (defEventBitChargeCondition))//除去S2主动断开情况，如果被监测的点有False, 电压异常由diag处理
                {
                    if((uxBitsCharge & defEventBitCONPlugOK) != defEventBitCONPlugOK)
                    {
                        printf_safe("\e[44;37mFource Unplug!\e[0m\n");
                        xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeUnPlug);
                    }
                    else if((uxBitsCharge & defEventBitCONAuthed) != defEventBitCONAuthed)
                    {
                        //用户原因停止
                        printf_safe("\e[44;37mAuth Clear!\e[0m\n");
                    }
                    pCON->state = STATE_CON_STOPCHARGE;
                    
                    /** @todo (rgw#1#): 后续会增加判断失效点，并对失效点进行提示。或者在这里不进行提示，而在发现失效时进行提示 */
                }
                break;
            case STATE_CON_STOPCHARGE:
                SetCONSignalWorkState(pCON, defSignalCON_State_Stopping);
                xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONStartOK);
#ifdef DEBUG_DIAG_DUMMY
                pCON->state = STATE_CON_RETURN;
                break;
#endif
                errcode = pCON->status.StopCharge(pCON);
                if (errcode == ERR_NO)
                {
                    printf_safe("\e[44;37mStop Charge!\e[0m\n");
                    stop_try = 0;
                    pCON->state = STATE_CON_UNLOCK;
                }
                else
                {
                    stop_try++;
                    printf_safe("paste!! try %d\n", stop_try);
                    if (stop_try >= 30)
                    {
                        pCON->state = STATE_CON_UNLOCK;//即便继电器失败, 也要解锁枪锁
                        break;
                    }
                    //vTaskDelay(1000);
                }
                break;
            case STATE_CON_UNLOCK:
                if (pCON->info.ucSocketType == defSocketTypeB)
                {
                    uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                    if ((uxBitsCharge & defEventBitCONLocked) == defEventBitCONLocked)
                    {
                        THROW_ERROR(i, pCON->status.SetBTypeSocketLock(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL, "STATE_CON_STOPCHARGE");
                        vTaskDelay(defRelayDelay);
                        pCON->status.GetBTypeSocketLock(pCON);
                        if (pCON->status.xBTypeSocketLockState == UNLOCK)
                        {
                            unlock_try = 0;
                            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONLocked);
                            if (stop_try != 0)//承接上面继电器失败
                            {
                                pCON->state = STATE_CON_ERROR;
                                break;
                            }
                            pCON->state = STATE_CON_RETURN;
                        }
                        else
                        {
                            unlock_try++;
                            if (unlock_try >= 5)
                            {
                                pCON->state = STATE_CON_ERROR;
                                break;
                            }
                            vTaskDelay(1000);
                        }
                    }
                    else
                    {
                        //开锁状态
                    }
                }
                else if (pCON->info.ucSocketType == defSocketTypeC)
                {
                    if (stop_try != 0)//承接上面继电器失败
                    {
                        pCON->state = STATE_CON_ERROR;
                        break;
                    }
                    pCON->state = STATE_CON_RETURN;
                }
                break;
            case STATE_CON_ERROR:
                SetCONSignalWorkState(pCON, defSignalCON_State_Fault);
                if (dev_err == 1)
                {
                    pCON->state = STATE_CON_DEV_ERROR;
                    break;
                }
                else if (stop_try != 0)
                {
                    //printf_safe("继电器故障! 断电修复后才能继续充电!!!\n");   
                    pCON->status.ulSignalFault |= defSignalCON_Fault_RelayPaste;
                }
                else if (unlock_try != 0)
                {
                    //printf_safe("枪锁故障! 断电修复后才能继续充电!!!\n");   
                    pCON->status.ulSignalFault |= defSignalCON_Fault_SocketLock;
                }
                else
                {
                    pCON->state = STATE_CON_RETURN;
                }
                ////
                THROW_ERROR(i, errcode = pCON->status.GetRelayState(pCON), ERR_LEVEL_CRITICAL, "Charge init");
                if (pCON->status.ucRelayLState == SWITCH_OFF &&
                        pCON->status.ucRelayNState == SWITCH_OFF)
                {
                    pCON->status.ulSignalFault &= ~defSignalCON_Fault_RelayPaste;
                    pCON->state = STATE_CON_RETURN;
                }
                if (pCON->info.ucSocketType == defSocketTypeB)
                {
                    THROW_ERROR(i, pCON->status.GetBTypeSocketLock(pCON), ERR_LEVEL_CRITICAL, "Charge init");
                    if (pCON->status.xBTypeSocketLockState == UNLOCK &&
                            pCON->status.xBTypeSocketLockState == UNLOCK)
                    {
                        pCON->status.ulSignalFault &= ~defSignalCON_Fault_SocketLock;
                        pCON->state = STATE_CON_RETURN;
                    }
                }
                break;
            case STATE_CON_DEV_ERROR:
                THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL, "Charging return");
                uxBitsException = xEventGroupGetBits(pCON->status.xHandleEventException);
                if ((uxBitsException & defEventBitExceptionDevFault) == 0)
                {
                    printf_safe("Error recovery!\n");
                    dev_err = 0;
                    pCON->state = STATE_CON_ERROR;
                } 
                break;
            case STATE_CON_RETURN:
                stop_try = 0;
                unlock_try = 0;
                dev_err = 0;
                xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed); //清除认证标志
                THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL, "Charging return");
                pCON->status.ucLoadPercent = 100;
                pCON->state = STATE_CON_IDLE;
                break;
            }
        }
#endif
        vTaskDelay(10);
    }
}

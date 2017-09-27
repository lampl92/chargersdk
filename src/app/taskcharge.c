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

//#define RFID_ProtoOK 
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

void vTaskEVSECharge(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBitsCharge;
    EventBits_t uxBitsException;
    uint8_t strTimerName[50];
    ErrorCode_t errcode;

    ulTotalCON = pListCON->Total;
    uxBitsCharge = 0;
    uxBitsException = 0;
    memset(strTimerName, 0, 50);
    errcode = ERR_NO;

    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        THROW_ERROR(i, errcode = pCON->status.GetRelayState(pCON), ERR_LEVEL_CRITICAL, "Charge init");
        if(errcode == ERR_NO)
        {
            xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionRelayPaste);
        }
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
                    /** @todo (rgw#1#): HMI */

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
                    xsprintf(strTimerName, "TimerCON%d_Charge_AntiShake", i);
                    pCON->status.xHandleTimerCharge = xTimerCreate(strTimerName,
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
                    THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL, "STATE_CON_PRECONTRACT_LOSEPLUG");
                    vTaskDelay(defRelayDelay);
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
		            pCON->state = STATE_CON_RETURN;
		            break;
	            }
	            
	            /////
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
                        THROW_ERROR(i, pCON->status.StartCharge(pCON), ERR_LEVEL_CRITICAL, "STATE_CON_STARTCHARGE");
                        vTaskDelay(defRelayDelay);
                        THROW_ERROR(i, errcode = pCON->status.GetRelayState(pCON), ERR_LEVEL_CRITICAL, "STATE_CON_STARTCHARGE");
                        if(errcode == ERR_NO)
                        {
                            xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionRelayPaste);
                        }
                        if(pCON->status.ucRelayLState == SWITCH_ON &&
                                pCON->status.ucRelayNState == SWITCH_ON)
                        {
                            //vTaskDelay(5000);//在这5s之间，防止RFID勿刷，并等待电流稳定。
                            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONStartOK);//rfid任务在等待
                            pCON->state = STATE_CON_CHARGING;
                            printf_safe("Start Charge!\n");
                        }
                        /** @todo (rgw#1#): 如果继电器操作失败，转换到ERR状态 */
                    }
                }
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                   defEventBitCONPlugOK,
                                                   pdFALSE, pdFALSE, 0);
                if((uxBitsCharge & defEventBitCONPlugOK) != defEventBitCONPlugOK)
                {
                    THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL, "拔枪关pwm");
                    pCON->state = STATE_CON_RETURN;
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
                    pCON->state = STATE_CON_ERROR;
                    break;
                }
                /*** 判断用户相关停止条件  ***/
                uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                                      defEventBitExceptionStopType,
                                                      pdTRUE, pdFALSE, 0);
                if((uxBitsException & defEventBitExceptionLimitFee) == defEventBitExceptionLimitFee)    //达到充电金额限制
                {
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeLimitFee);
                    xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                }
                if((uxBitsException & defEventBitExceptionRemoteStop) == defEventBitExceptionRemoteStop)    //远程停止
                {
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeRemoteStop);
                    xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                }
                if((uxBitsException & defEventBitExceptionRFIDStop) == defEventBitExceptionRFIDStop)    //刷卡停止
                {
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeRFIDStop);
                    xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                }
                /******************************/

                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
//                printf_safe("uxBitsCharge = %X\n", uxBitsCharge);
//                printf_safe("CPCondition = %X\n", defEventBitChargeCondition);
                if((uxBitsCharge & defEventBitCONS2Opened) == defEventBitCONS2Opened) //6vpwm->9vpwm S2主动断开
                {
                    THROW_ERROR(i, pCON->status.StopCharge(pCON), ERR_LEVEL_CRITICAL, "STATE_CON_CHARGING S2 Open");
                    vTaskDelay(defRelayDelay);
                    THROW_ERROR(i, errcode = pCON->status.GetRelayState(pCON), ERR_LEVEL_CRITICAL, "STATE_CON_CHARGING");
                    if(errcode == ERR_NO)
                    {
                        xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionRelayPaste);
                    }
                    if(pCON->status.ucRelayLState == SWITCH_OFF &&
                            pCON->status.ucRelayNState == SWITCH_OFF)
                    {
                        xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeFull);
                        printf_safe("S2 Stop Charge!\n");
                        pCON->state = STATE_CON_STOPCHARGE;
                    }
                }
                else if(((uxBitsCharge & (defEventBitChargeCondition)) | defEventBitCONVoltOK) != (defEventBitChargeCondition))//除去S2主动断开情况，如果被监测的点有False, 电压异常由diag处理
                {
                    if((uxBitsCharge & defEventBitCONAuthed) != defEventBitCONAuthed)
                    {
                        //用户原因停止
                    }
                    THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL, "STATE_CON_CHARGING Without \"S2 open\"");
                    vTaskDelay(defRelayDelay);
#ifdef DEBUG_DIAG_DUMMY
                    pCON->status.xCPState = CP_12V;
#endif

                    if(pCON->status.xCPState == CP_6V ||
                       pCON->status.xCPState == CP_9V ||
                       pCON->status.xCPState == CP_12V)
                    {
                        uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                           defEventBitCONS2Opened,
                                                           pdFALSE, pdTRUE, 100);//S1转换到12V后S2应在100ms内断开，否则强制带载断电。
                        //此处应该判断uxbits，但在这里无意义，因为无论如何100ms内或者100ms外都要断电。
                        THROW_ERROR(i, pCON->status.StopCharge(pCON), ERR_LEVEL_CRITICAL, "other stop charge");
                        vTaskDelay(defRelayDelay);
                        THROW_ERROR(i, errcode = pCON->status.GetRelayState(pCON), ERR_LEVEL_CRITICAL, "other stop charge");
                        if(errcode == ERR_NO)
                        {
                            xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionRelayPaste);
                        }
#ifdef DEBUG_DIAG_DUMMY
                        pCON->status.ucRelayLState = SWITCH_OFF;
                        pCON->status.ucRelayNState = SWITCH_OFF;
#endif
                        if(pCON->status.ucRelayLState == SWITCH_OFF &&
                           pCON->status.ucRelayNState == SWITCH_OFF)
                        {
                            printf_safe("Other Stop Charge!\n");
                            pCON->state = STATE_CON_STOPCHARGE;
                        }
                    }
                    /** @todo (rgw#1#): 后续会增加判断失效点，并对失效点进行提示。或者在这里不进行提示，而在发现失效时进行提示 */
                }
                break;
            case STATE_CON_STOPCHARGE:
                SetCONSignalWorkState(pCON, defSignalCON_State_Stopping);

                /** @todo (rgw#1#): 等待结费
                                    结费成功后通知HMI显示结费完成,进入idle */
#ifdef DEBUG_DIAG_DUMMY
                xEventGroupSetBits(xHandleEventHMI, defeventBitHMI_ChargeReqDispDoneOK);
#endif
//                xEventGroupSync(xHandleEventHMI,
//                                defEventBitHMI_ChargeReqDispDone,
//                                defeventBitHMI_ChargeReqDispDoneOK,
//                                portMAX_DELAY );
                xEventGroupSetBits(xHandleEventHMI, defEventBitHMI_ChargeReqDispDone);//通知HMI显示结束订单

                xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONStartOK);
#ifdef RFID_ProtoOK// 刷卡协议完成后添加
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                                   defEventBitOrderFinishToChargetask,
                                                   pdTRUE, pdTRUE, 10000);
	            if ((uxBitsCharge & defEventBitOrderFinishToChargetask) == defEventBitOrderFinishToChargetask)
                {
#endif
#ifdef DEBUG_DIAG_DUMMY
                            pCON->state = STATE_CON_IDLE;
#endif
                    //解锁
                    if(pCON->info.ucSocketType == defSocketTypeB)
                    {
                        uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                        if((uxBitsCharge & defEventBitCONLocked) == defEventBitCONLocked)
                        {
                            THROW_ERROR(i, pCON->status.SetBTypeSocketLock(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL, "STATE_CON_STOPCHARGE");
                            vTaskDelay(defRelayDelay);
                            pCON->status.GetBTypeSocketLock(pCON);
                            if(pCON->status.xBTypeSocketLockState == UNLOCK)
                            {
                                xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONLocked);
                                pCON->state = STATE_CON_RETURN;
                            }
                        }
                        else
                        {

                        }
                    }
                    else if(pCON->info.ucSocketType == defSocketTypeC)
                    {
                        pCON->state = STATE_CON_RETURN;
                    }
#ifdef RFID_ProtoOK// 刷卡协议完成后添加
                }
#endif
                break;
            case STATE_CON_ERROR:
                SetCONSignalWorkState(pCON, defSignalCON_State_Fault);
                THROW_ERROR(i, pCON->status.StopCharge(pCON), ERR_LEVEL_CRITICAL, "STATE_CON_ERROR");
                vTaskDelay(defRelayDelay);
                /** @todo (rgw#1#): 等待diag处理完成 */

                pCON->state = STATE_CON_RETURN;
                break;
            case STATE_CON_RETURN:
                xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed); //清除认证标志
                pCON->status.ucLoadPercent = 100;
                pCON->state = STATE_CON_IDLE;
                break;
            }
        }
#endif
        vTaskDelay(10);
    }
}

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

void vTaskEVSECharge(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBitsCharge;
    EventBits_t uxBitsException;
    uint8_t strTimerName[50];

    ulTotalCON = pListCON->Total;
    uxBitsCharge = 0;
    uxBitsException = 0;
    memset(strTimerName, 0, 50);

    while(1)
    {
#ifndef DEBUG_NO_TASKCHARGE
        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            switch(pCON->state)
            {
            case STATE_CON_IDLE://状态1
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
                        THROW_ERROR(i, pCON->status.SetBTypeSocketLock(pCON, SWITCH_ON), ERR_LEVEL_CRITICAL, "STATE_CON_PLUGED");
                        vTaskDelay(defRelayDelay);
                    }
                }
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                   defEventBitCPSwitchCondition,
                                                   pdFALSE, pdTRUE, 0);
                if((uxBitsCharge & defEventBitCPSwitchCondition) == defEventBitCPSwitchCondition)
                {
                    pCON->status.SetLoadPercent(pCON, 100);/** @fixme (rgw#1#): 设置PWM脉宽，100%负载启动 */
                    THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_ON), ERR_LEVEL_CRITICAL, "STATE_CON_PLUGED");
                    vTaskDelay(defRelayDelay);
                    if(pCON->status.xCPState == CP_9V_PWM || pCON->status.xCPState == CP_6V_PWM) //后一种情况适用于无S2车辆, 即S1闭合后直接进入6V_PWM状态。
                    {
                        pCON->state = STATE_CON_PRECONTRACT;
                    }
                }
                if((uxBitsCharge & defEventBitCONPlugOK) != defEventBitCONPlugOK)
                {
                    pCON->state = STATE_CON_IDLE;
                }
                break;
            case STATE_CON_PRECONTRACT://状态2' 充电设备准备就绪，等待车的S2，由车辆决定，可用于预约充电等
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
                        pCON->state = STATE_CON_IDLE;
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
                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if((uxBitsCharge & defEventBitCONAuthed) == defEventBitCONAuthed)
                {
                    THROW_ERROR(i, pCON->status.StartCharge(pCON), ERR_LEVEL_CRITICAL, "STATE_CON_STARTCHARGE");
                    vTaskDelay(defRelayDelay);
                    if(pCON->status.ucRelayLState == SWITCH_ON &&
                            pCON->status.ucRelayNState == SWITCH_ON)
                    {
                        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONStartOK);//rfid任务在等待
                        pCON->state = STATE_CON_CHARGING;
                        printf_safe("Start Charge!\n");
                        vTaskDelay(5000);//在这5s之间，防止RFID勿刷，并等待电流稳定。
                    }
                    /** @todo (rgw#1#): 如果继电器操作失败，转换到ERR状态 */
                }

                break;
            case STATE_CON_CHARGING:
                uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                                      defEventBitExceptionCritical,
                                                      pdFALSE, pdFALSE, 0);
                if((uxBitsException & defEventBitExceptionCritical) != 0)
                {
                    printf_safe("Stop Error!\n");
                    pCON->state = STATE_CON_ERROR;
                    break;
                }
                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if((uxBitsCharge & defEventBitCONS2Opened) == defEventBitCONS2Opened) //6vpwm->9vpwm S2主动断开
                {
                    THROW_ERROR(i, pCON->status.StopCharge(pCON), ERR_LEVEL_CRITICAL, "STATE_CON_CHARGING S2 Open");
                    vTaskDelay(defRelayDelay);
                    if(pCON->status.ucRelayLState == SWITCH_OFF &&
                            pCON->status.ucRelayNState == SWITCH_OFF)
                    {
                        printf_safe("Stop Charge!\n");
                        pCON->state = STATE_CON_STOPCHARGE;
                    }
                }
                else if((uxBitsCharge & defEventBitChargeCondition) != defEventBitChargeCondition)//除去S2主动断开情况，如果被监测的点有False
                {
                    if((uxBitsCharge & defEventBitCONAuthed) != defEventBitCONAuthed)//刷卡停止
                    {
                        pCON->order.ucStopType = defOrderStopType_RFID;
                    }
                    THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL, "STATE_CON_CHARGING Without \"S2 open\"");
                    vTaskDelay(defRelayDelay);
#ifdef DEBUG_DIAG_DUMMY
                    pCON->status.xCPState = CP_12V;
#endif
                    if(pCON->status.xCPState == CP_12V)
                    {
                        uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                           defEventBitCONS2Opened,
                                                           pdFALSE, pdTRUE, 100);//S1转换到12V后S2应在100ms内断开，否则强制带载断电。
                        //此处应该判断uxbits，但在这里无意义，因为无论如何100ms内或者100ms外都要断电。
                        THROW_ERROR(i, pCON->status.StopCharge(pCON), ERR_LEVEL_CRITICAL, "CP_12V");
                        vTaskDelay(defRelayDelay);
#ifdef DEBUG_DIAG_DUMMY
                        pCON->status.ucRelayLState = SWITCH_OFF;
                        pCON->status.ucRelayNState = SWITCH_OFF;
#endif
                        if(pCON->status.ucRelayLState == SWITCH_OFF &&
                                pCON->status.ucRelayNState == SWITCH_OFF)
                        {   printf_safe("Stop Charge!\n");
                            pCON->state = STATE_CON_STOPCHARGE;
                        }
                    }
                    /** @todo (rgw#1#): 后续会增加判断失效点，并对失效点进行提示。或者在这里不进行提示，而在发现失效时进行提示 */
                }
                break;
            case STATE_CON_STOPCHARGE:
                xEventGroupSync(xHandleEventHMI,
                                defEventBitHMI_ChargeReqDispDone,
                                defeventBitHMI_ChargeReqDispDoneOK,
                                portMAX_DELAY );
                /** @todo (rgw#1#): 等待结费
                                    结费成功后通知HMI显示结费完成,进入idle */
                xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONStartOK);
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                      defEventBitCONOrderFinish,
                                                      pdTRUE, pdTRUE, 0);
                if((uxBitsCharge & defEventBitCONOrderFinish) == defEventBitCONOrderFinish)
                {
                    pCON->state = STATE_CON_IDLE;
                }
                break;
            case STATE_CON_ERROR:
                THROW_ERROR(i, pCON->status.StopCharge(pCON), ERR_LEVEL_CRITICAL, "STATE_CON_ERROR");
                vTaskDelay(defRelayDelay);
                /** @todo (rgw#1#): 等待diag处理完成 */
                pCON->state = STATE_CON_IDLE;
                break;
            }
        }
#endif
        vTaskDelay(10);
    }
}

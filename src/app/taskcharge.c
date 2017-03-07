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

void vTaskEVSECharge(void *pvParameters)
{
    ChargePoint_t *pPoint = NULL;
    uint32_t ulTotalPoint;
    int i;
    EventBits_t uxBitsCharge;
    EventBits_t uxBitsException;
    uint8_t strTimerName[50];

    ulTotalPoint = pListChargePoint->Total;
    uxBitsCharge = 0;
    uxBitsException = 0;
    memset(strTimerName, 0, 50);

    while(1)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            pPoint = ChargePointGetHandle(i);
            switch(pPoint->state)
            {
            case STATE_POINT_IDLE://状态1
                uxBitsCharge = xEventGroupWaitBits(pPoint->status.xHandleEventCharge,
                                                   defEventBitPointPlugOK,
                                                   pdFALSE, pdFALSE, 0);
                if((uxBitsCharge & defEventBitPointPlugOK) == defEventBitPointPlugOK)
                {
                    /** @todo (rgw#1#): HMI */

                    pPoint->state = STATE_POINT_PLUGED;
                }
                break;
            case STATE_POINT_PLUGED://状态2
                if(pPoint->info.ucConnectorType == defConnectorTypeB)
                {
                    uxBitsCharge = xEventGroupWaitBits(pPoint->status.xHandleEventCharge,
                                                       defEventBitPointLocked,
                                                       pdFALSE, pdFALSE, 0);
                    if((uxBitsCharge & defEventBitPointLocked) == defEventBitPointLocked)
                    {
                        /** @todo (rgw#1#): HMI */
                    }
                    else
                    {
                        THROW_ERROR(i, pPoint->status.SetBTypeConnectorLock(pPoint, SWITCH_ON), ERR_LEVEL_CRITICAL);
                        vTaskDelay(defRelayDelay);
                    }
                }
                uxBitsCharge = xEventGroupWaitBits(pPoint->status.xHandleEventCharge,
                                                   defEventBitCPSwitchCondition,
                                                   pdFALSE, pdTRUE, 0);
                if((uxBitsCharge & defEventBitCPSwitchCondition) == defEventBitCPSwitchCondition)
                {
                    pPoint->status.SetLoadPercent(pPoint, 100);/** @fixme (rgw#1#): 设置PWM脉宽，100%负载启动 */
                    THROW_ERROR(i, pPoint->status.SetCPSwitch(pPoint, SWITCH_ON), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pPoint->status.xCPState == CP_9V_PWM || pPoint->status.xCPState == CP_6V_PWM) //后一种情况适用于无S2车辆, 即S1闭合后直接进入6V_PWM状态。
                    {
                        pPoint->state = STATE_POINT_PRECONTRACT;
                    }
                }
                if((uxBitsCharge & defEventBitPointPlugOK) != defEventBitPointPlugOK)
                {
                    pPoint->state = STATE_POINT_IDLE;
                }
                break;
            case STATE_POINT_PRECONTRACT://状态2' 充电设备准备就绪，等待车的S2，由车辆决定，可用于预约充电等
                uxBitsCharge = xEventGroupGetBits(pPoint->status.xHandleEventCharge);
                if((uxBitsCharge & defEventBitPointS2Closed) == defEventBitPointS2Closed)
                {
                    pPoint->state = STATE_POINT_STARTCHARGE;
                }
                if((uxBitsCharge & defEventBitPointPlugOK) != defEventBitPointPlugOK) //状态1'触发条件
                {
                    xsprintf(strTimerName, "TimerPoint%d_Charge_AntiShake", i);
                    pPoint->status.xHandleTimerCharge = xTimerCreate(strTimerName,
                                                        defChargeAntiShakeCyc,
                                                        pdFALSE,
                                                        (void *)i,
                                                        vChargeStateTimerCB);
                    xTimerStart(pPoint->status.xHandleTimerCharge, 0);
                    pPoint->state = STATE_POINT_PRECONTRACT_LOSEPLUG;
                }
                break;
            case STATE_POINT_PRECONTRACT_LOSEPLUG://状态1' 未连接PWM，充电设备准备好后失去连接
                uxBitsException = xEventGroupWaitBits(pPoint->status.xHandleEventException,
                                                      defEventBitExceptionChargeTimer,
                                                      pdTRUE, pdFALSE, 0);
                if((uxBitsException & defEventBitExceptionChargeTimer) == defEventBitExceptionChargeTimer)
                {
                    xTimerDelete(pPoint->status.xHandleTimerCharge, 0);
                    THROW_ERROR(i, pPoint->status.SetCPSwitch(pPoint, SWITCH_OFF), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pPoint->status.xCPState == CP_12V)
                    {
                        pPoint->state = STATE_POINT_IDLE;
                    }
                }
                else
                {
                    uxBitsCharge = xEventGroupGetBits(pPoint->status.xHandleEventCharge);
                    if((uxBitsCharge & defEventBitPointPlugOK) == defEventBitPointPlugOK)
                    {
                        pPoint->state = STATE_POINT_PRECONTRACT;
                    }
                }
                break;
            case STATE_POINT_STARTCHARGE:
                THROW_ERROR(i, pPoint->status.StartCharge(pPoint), ERR_LEVEL_CRITICAL);
                vTaskDelay(defRelayDelay);
                if(pPoint->status.ucRelayLState == SWITCH_ON &&
                        pPoint->status.ucRelayNState == SWITCH_ON)
                {
                    pPoint->state = STATE_POINT_CHARGING;
                }
                /** @todo (rgw#1#): 如果继电器操作失败，转换到ERR状态 */
                break;
            case STATE_POINT_CHARGING:
                uxBitsException = xEventGroupWaitBits(pPoint->status.xHandleEventException,
                                                      defEventBitExceptionCritical,
                                                      pdFALSE, pdFALSE, 0);
                if((uxBitsException & defEventBitExceptionCritical) != 0)
                {
                    pPoint->state = STATE_POINT_ERROR;
                }

                uxBitsCharge = xEventGroupGetBits(pPoint->status.xHandleEventCharge);
                if((uxBitsCharge & defEventBitPointS2Opened) == defEventBitPointS2Opened) //6vpwm->9vpwm S2主动断开
                {
                    THROW_ERROR(i, pPoint->status.StopCharge(pPoint), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pPoint->status.ucRelayLState == SWITCH_OFF &&
                            pPoint->status.ucRelayNState == SWITCH_OFF)
                    {
                        pPoint->state = STATE_POINT_STOPCHARGE;
                    }
                }
                else if((uxBitsCharge & defEventBitChargeCondition) != defEventBitChargeCondition)//除去S2主动断开情况，如果被监测的点有False
                {
                    THROW_ERROR(i, pPoint->status.SetCPSwitch(pPoint, SWITCH_OFF), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pPoint->status.xCPState == CP_12V)
                    {
                        uxBitsCharge = xEventGroupWaitBits(pPoint->status.xHandleEventCharge,
                                                           defEventBitPointS2Opened,
                                                           pdFALSE, pdTRUE, 100);//S1转换到12V后S2应在100ms内断开，否则强制带载断电。
                        //此处应该判断uxbits，但在这里无意义，因为无论如何100ms内或者100ms外都要断电。
                        THROW_ERROR(i, pPoint->status.StopCharge(pPoint), ERR_LEVEL_CRITICAL);
                        vTaskDelay(defRelayDelay);
                        if(pPoint->status.ucRelayLState == SWITCH_OFF &&
                                pPoint->status.ucRelayNState == SWITCH_OFF)
                        {
                            pPoint->state = STATE_POINT_STOPCHARGE;
                        }
                    }
                    /** @todo (rgw#1#): 后续会增加判断失效点，并对失效点进行提示。或者在这里不进行提示，而在发现失效时进行提示 */
                }
                break;
            case STATE_POINT_STOPCHARGE:
                /** @todo (rgw#1#): 等待结费
                                    结费成功后进入idle */

                pPoint->state = STATE_POINT_IDLE;
                break;
            case STATE_POINT_ERROR:
                THROW_ERROR(i, pPoint->status.StopCharge(pPoint), ERR_LEVEL_CRITICAL);
                vTaskDelay(defRelayDelay);
                /** @todo (rgw#1#): 等待diag处理完成 */
                pPoint->state = STATE_POINT_ERROR;
                break;
            }
        }
        vTaskDelay(10);
    }
}

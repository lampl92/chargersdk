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

    ulTotalPoint = pListChargePoint->Total;
    uxBitsCharge = 0;
    uxBitsException = 0;

    while(1)
    {
        #if 1
        for(i = 0; i < ulTotalPoint; i++)
        {
            pPoint = ChargePointGetHandle(i);
            switch(pPoint->state)
            {
            case STATE_POINT_IDLE:
                uxBitsCharge = xEventGroupWaitBits(pPoint->status.xHandleEventCharge,
                                             defEventBitPointPlugOK,
                                             pdFALSE, pdFALSE, 0);
                if((uxBitsCharge & defEventBitPointPlugOK) == defEventBitPointPlugOK)
                {
                    /** @todo (rgw#1#): HMI */

                    pPoint->state = STATE_POINT_PLUGED;
                }
                break;
            case STATE_POINT_PLUGED:
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
                        pPoint->status.SetBTypeConnectorLock(pPoint, SWITCH_ON);
                    }
                }
                uxBitsCharge = xEventGroupWaitBits(pPoint->status.xHandleEventCharge,
                                             defEventBitCPSwitchCondition,
                                             pdFALSE, pdTRUE, 100);
                if((uxBitsCharge & defEventBitCPSwitchCondition) == defEventBitCPSwitchCondition)
                {
                    pPoint->status.SetLoadPercent(pPoint, 100);/** @fixme (rgw#1#): 设置PWM脉宽，100%负载启动 */
                    //THROW_ERROR(i, pPoint->status.SetCPSwitch(pPoint, SWITCH_ON), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pPoint->status.xCPState == CP_9V_PWM || pPoint->status.xCPState == CP_6V_PWM) //后一种情况适用于无S2车辆, 即S1闭合后直接进入6V_PWM状态。
                    {
                        pPoint->state = STATE_POINT_PRECONTRACT;
                    }
                }
                break;
            case STATE_POINT_PRECONTRACT://等待车的S2，由车辆决定，可用于预约充电等
                if(pPoint->status.xCPState == CP_6V_PWM)
                {
                    xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointS2OK);
                    pPoint->state = STATE_POINT_STARTCHARGE;
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

                if((uxBitsCharge & defEventBitChargeCondition) != defEventBitChargeCondition)//如果被监测的点有False
                {
                    THROW_ERROR(i, pPoint->status.SetCPSwitch(pPoint, SWITCH_OFF), ERR_LEVEL_CRITICAL);

                    THROW_ERROR(i, pPoint->status.StopCharge(pPoint), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pPoint->status.ucRelayLState == SWITCH_OFF && pPoint->status.ucRelayNState == SWITCH_OFF)
                    {
                        pPoint->state = STATE_POINT_STOPCHARGE;
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
#endif

#if DEBUG_CHARGE
        xprintf("%s\n", TASKNAME_EVSECharge);
#endif
        vTaskDelay(10);
    }
}

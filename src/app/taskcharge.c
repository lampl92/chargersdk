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
    uint32_t ulTotalPoint = pListChargePoint->Total;
    ChargePoint_t *pPoint[ulTotalPoint];
    int i;
    EventBits_t uxBits;
    for(i = 0; i < ulTotalPoint; i++)
    {
        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray[i]);
    }
    while(1)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            switch(pPoint[i]->state)
            {
            case POINT_IDLE:
                uxBits = xEventGroupWaitBits(pPoint[i]->status.xHandleEventStartCondition,
                                             defEventBitStdPlugOK,
                                             pdFALSE, pdFALSE, 0);
                if((uxBits & defEventBitStdPlugOK) == defEventBitStdPlugOK)
                {
                    /** @todo (rgw#1#): HMI */

                    pPoint[i]->state = POINT_PLUGED;
                }
                break;
            case POINT_PLUGED:
                if(pPoint[i]->info.ucConnectorType == defConnectorTypeB)
                {
                    uxBits = xEventGroupWaitBits(pPoint[i]->status.xHandleEventStartCondition,
                                                 defEventBitStdLocked,
                                                 pdFALSE, pdFALSE, 0);
                    if((uxBits & defEventBitStdLocked) == defEventBitStdLocked)
                    {
                        /** @todo (rgw#1#): HMI */
                    }
                    else
                    {
                        pPoint[i]->status.SetBTypeConnectorLock(pPoint[i], SWITCH_ON);
                    }
                }
                uxBits = xEventGroupWaitBits(pPoint[i]->status.xHandleEventStartCondition,
                                             defEventBitCPSwitchCondition,
                                             pdFALSE, pdTRUE, 100);
                if((uxBits & defEventBitCPSwitchCondition) == defEventBitCPSwitchCondition)
                {
                    THROW_ERROR(i, pPoint[i]->status.SetCPSwitch(pPoint[i], SWITCH_ON), ERR_LEVEL_CRITICAL);
                    vTaskDelay(100);

//                    pPoint[i]->status.StartCharge(pPoint[i]);
//                    pPoint[i]->state = POINT_CHARGING;
//                    xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdOK);
                }
                if(pPoint[i]->status.xCPState == CP_9V_PWM || pPoint[i]->status.xCPState == CP_6V_PWM) //后一种情况适用于无S2车辆
                {
                    pPoint[i]->state = POINT_PRECONTRACT;
                }
                break;
            case POINT_PRECONTRACT://等待车的S2
                if(pPoint[i]->status.xCPState == CP_6V_PWM)
                {
                    pPoint[i]->state = POINT_STARTCHARGE;
                }
                break;
            case POINT_STARTCHARGE:
                THROW_ERROR(i, pPoint[i]->status.StartCharge(pPoint[i]), ERR_LEVEL_CRITICAL);
                if(pPoint[i]->status.ucRelayLState == SWITCH_ON &&
                   pPoint[i]->status.ucRelayNState == SWITCH_ON)
                {
                    pPoint[i]->state = POINT_CHARGING;
                }
                break;
            case POINT_CHARGING:
                uxBits = xEventGroupWaitBits(pPoint[i]->status.xHandleEventException,
                                             defEventBitExceptionCritical
                                             pdFALSE, pdFALSE, 0);
                if((uxBits & defEventBitExceptionCritical) != 0)
                {
                    pPoint[i]->status.StopCharge(pPoint[i]);
                    pPoint[i]->state = POINT_ERROR;
                }
                break;

            case POINT_ERROR:


            }
        }


#if DEBUG_CHARGE
        xprintf("%s\n", TASKNAME_EVSECharge);
#endif
        vTaskDelay(10);
    }
}

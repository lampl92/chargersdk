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
            case STATE_POINT_IDLE://״̬1
                uxBitsCharge = xEventGroupWaitBits(pPoint->status.xHandleEventCharge,
                                                   defEventBitPointPlugOK,
                                                   pdFALSE, pdFALSE, 0);
                if((uxBitsCharge & defEventBitPointPlugOK) == defEventBitPointPlugOK)
                {
                    /** @todo (rgw#1#): HMI */

                    pPoint->state = STATE_POINT_PLUGED;
                }
                break;
            case STATE_POINT_PLUGED://״̬2
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
                    pPoint->status.SetLoadPercent(pPoint, 100);/** @fixme (rgw#1#): ����PWM����100%�������� */
                    THROW_ERROR(i, pPoint->status.SetCPSwitch(pPoint, SWITCH_ON), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pPoint->status.xCPState == CP_9V_PWM || pPoint->status.xCPState == CP_6V_PWM) //��һ�������������S2����, ��S1�պϺ�ֱ�ӽ���6V_PWM״̬��
                    {
                        pPoint->state = STATE_POINT_PRECONTRACT;
                    }
                }
                if((uxBitsCharge & defEventBitPointPlugOK) != defEventBitPointPlugOK)
                {
                    pPoint->state = STATE_POINT_IDLE;
                }
                break;
            case STATE_POINT_PRECONTRACT://״̬2' ����豸׼���������ȴ�����S2���ɳ���������������ԤԼ����
                uxBitsCharge = xEventGroupGetBits(pPoint->status.xHandleEventCharge);
                if((uxBitsCharge & defEventBitPointS2Closed) == defEventBitPointS2Closed)
                {
                    pPoint->state = STATE_POINT_STARTCHARGE;
                }
                if((uxBitsCharge & defEventBitPointPlugOK) != defEventBitPointPlugOK) //״̬1'��������
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
            case STATE_POINT_PRECONTRACT_LOSEPLUG://״̬1' δ����PWM������豸׼���ú�ʧȥ����
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
                /** @todo (rgw#1#): ����̵�������ʧ�ܣ�ת����ERR״̬ */
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
                if((uxBitsCharge & defEventBitPointS2Opened) == defEventBitPointS2Opened) //6vpwm->9vpwm S2�����Ͽ�
                {
                    THROW_ERROR(i, pPoint->status.StopCharge(pPoint), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pPoint->status.ucRelayLState == SWITCH_OFF &&
                            pPoint->status.ucRelayNState == SWITCH_OFF)
                    {
                        pPoint->state = STATE_POINT_STOPCHARGE;
                    }
                }
                else if((uxBitsCharge & defEventBitChargeCondition) != defEventBitChargeCondition)//��ȥS2�����Ͽ��������������ĵ���False
                {
                    THROW_ERROR(i, pPoint->status.SetCPSwitch(pPoint, SWITCH_OFF), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pPoint->status.xCPState == CP_12V)
                    {
                        uxBitsCharge = xEventGroupWaitBits(pPoint->status.xHandleEventCharge,
                                                           defEventBitPointS2Opened,
                                                           pdFALSE, pdTRUE, 100);//S1ת����12V��S2Ӧ��100ms�ڶϿ�������ǿ�ƴ��ضϵ硣
                        //�˴�Ӧ���ж�uxbits���������������壬��Ϊ�������100ms�ڻ���100ms�ⶼҪ�ϵ硣
                        THROW_ERROR(i, pPoint->status.StopCharge(pPoint), ERR_LEVEL_CRITICAL);
                        vTaskDelay(defRelayDelay);
                        if(pPoint->status.ucRelayLState == SWITCH_OFF &&
                                pPoint->status.ucRelayNState == SWITCH_OFF)
                        {
                            pPoint->state = STATE_POINT_STOPCHARGE;
                        }
                    }
                    /** @todo (rgw#1#): �����������ж�ʧЧ�㣬����ʧЧ�������ʾ�����������ﲻ������ʾ�����ڷ���ʧЧʱ������ʾ */
                }
                break;
            case STATE_POINT_STOPCHARGE:
                /** @todo (rgw#1#): �ȴ����
                                    ��ѳɹ������idle */

                pPoint->state = STATE_POINT_IDLE;
                break;
            case STATE_POINT_ERROR:
                THROW_ERROR(i, pPoint->status.StopCharge(pPoint), ERR_LEVEL_CRITICAL);
                vTaskDelay(defRelayDelay);
                /** @todo (rgw#1#): �ȴ�diag������� */
                pPoint->state = STATE_POINT_ERROR;
                break;
            }
        }
        vTaskDelay(10);
    }
}

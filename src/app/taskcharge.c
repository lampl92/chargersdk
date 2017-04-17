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
            case STATE_CON_IDLE://״̬1
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                   defEventBitCONPlugOK,
                                                   pdFALSE, pdFALSE, 0);
                if((uxBitsCharge & defEventBitCONPlugOK) == defEventBitCONPlugOK)
                {
                    /** @todo (rgw#1#): HMI */

                    pCON->state = STATE_CON_PLUGED;
                }
                break;
            case STATE_CON_PLUGED://״̬2
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
                        THROW_ERROR(i, pCON->status.SetBTypeSocketLock(pCON, SWITCH_ON), ERR_LEVEL_CRITICAL);
                        vTaskDelay(defRelayDelay);
                    }
                }
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                   defEventBitCPSwitchCondition,
                                                   pdFALSE, pdTRUE, 0);
                if((uxBitsCharge & defEventBitCPSwitchCondition) == defEventBitCPSwitchCondition)
                {
                    pCON->status.SetLoadPercent(pCON, 100);/** @fixme (rgw#1#): ����PWM����100%�������� */
                    THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_ON), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pCON->status.xCPState == CP_9V_PWM || pCON->status.xCPState == CP_6V_PWM) //��һ�������������S2����, ��S1�պϺ�ֱ�ӽ���6V_PWM״̬��
                    {
                        pCON->state = STATE_CON_PRECONTRACT;
                    }
                }
                if((uxBitsCharge & defEventBitCONPlugOK) != defEventBitCONPlugOK)
                {
                    pCON->state = STATE_CON_IDLE;
                }
                break;
            case STATE_CON_PRECONTRACT://״̬2' ����豸׼���������ȴ�����S2���ɳ���������������ԤԼ����
                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if((uxBitsCharge & defEventBitCONS2Closed) == defEventBitCONS2Closed)
                {
                    pCON->state = STATE_CON_STARTCHARGE;
                }
                if((uxBitsCharge & defEventBitCONPlugOK) != defEventBitCONPlugOK) //״̬1'��������
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
            case STATE_CON_PRECONTRACT_LOSEPLUG://״̬1' δ����PWM������豸׼���ú�ʧȥ����
                uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                                      defEventBitExceptionChargeTimer,
                                                      pdTRUE, pdFALSE, 0);
                if((uxBitsException & defEventBitExceptionChargeTimer) == defEventBitExceptionChargeTimer)
                {
                    xTimerDelete(pCON->status.xHandleTimerCharge, 0);
                    THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL);
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
                    THROW_ERROR(i, pCON->status.StartCharge(pCON), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pCON->status.ucRelayLState == SWITCH_ON &&
                            pCON->status.ucRelayNState == SWITCH_ON)
                    {
                        xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONStartOK);//rfid�����ڵȴ�
                        pCON->state = STATE_CON_CHARGING;
                        printf_safe("Start Charge!\n");
                        vTaskDelay(5000);//����5s֮�䣬��ֹRFID��ˢ�����ȴ������ȶ���
                    }
                    /** @todo (rgw#1#): ����̵�������ʧ�ܣ�ת����ERR״̬ */
                }

                break;
            case STATE_CON_CHARGING:
                uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                                      defEventBitExceptionCritical,
                                                      pdFALSE, pdFALSE, 0);
                if((uxBitsException & defEventBitExceptionCritical) != 0)
                {
                    pCON->state = STATE_CON_ERROR;
                }

                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if((uxBitsCharge & defEventBitCONS2Opened) == defEventBitCONS2Opened) //6vpwm->9vpwm S2�����Ͽ�
                {
                    THROW_ERROR(i, pCON->status.StopCharge(pCON), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
                    if(pCON->status.ucRelayLState == SWITCH_OFF &&
                            pCON->status.ucRelayNState == SWITCH_OFF)
                    {
                        pCON->state = STATE_CON_STOPCHARGE;
                    }
                }
                else if((uxBitsCharge & defEventBitChargeCondition) != defEventBitChargeCondition)//��ȥS2�����Ͽ��������������ĵ���False
                {
                    THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_OFF), ERR_LEVEL_CRITICAL);
                    vTaskDelay(defRelayDelay);
#ifdef DEBUG_DIAG_DUMMY
                    pCON->status.xCPState = CP_12V;
#endif
                    if(pCON->status.xCPState == CP_12V)
                    {
                        uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                           defEventBitCONS2Opened,
                                                           pdFALSE, pdTRUE, 100);//S1ת����12V��S2Ӧ��100ms�ڶϿ�������ǿ�ƴ��ضϵ硣
                        //�˴�Ӧ���ж�uxbits���������������壬��Ϊ�������100ms�ڻ���100ms�ⶼҪ�ϵ硣
                        THROW_ERROR(i, pCON->status.StopCharge(pCON), ERR_LEVEL_CRITICAL);
                        vTaskDelay(defRelayDelay);
#ifdef DEBUG_DIAG_DUMMY
                        pCON->status.ucRelayLState = SWITCH_OFF;
                        pCON->status.ucRelayNState = SWITCH_OFF;
#endif
                        if(pCON->status.ucRelayLState == SWITCH_OFF &&
                                pCON->status.ucRelayNState == SWITCH_OFF)
                        {
                            pCON->state = STATE_CON_STOPCHARGE;
                        }
                    }
                    /** @todo (rgw#1#): �����������ж�ʧЧ�㣬����ʧЧ�������ʾ�����������ﲻ������ʾ�����ڷ���ʧЧʱ������ʾ */
                }
                break;
            case STATE_CON_STOPCHARGE:
                 printf_safe("Stop Charge!\n");
                /** @todo (rgw#1#): �ȴ����
                                    ��ѳɹ������idle */
                xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONStartOK);
                OrderInit(&(pCON->order));
                pCON->state = STATE_CON_IDLE;
                break;
            case STATE_CON_ERROR:
                THROW_ERROR(i, pCON->status.StopCharge(pCON), ERR_LEVEL_CRITICAL);
                vTaskDelay(defRelayDelay);
                /** @todo (rgw#1#): �ȴ�diag������� */
                pCON->state = STATE_CON_IDLE;
                break;
            }
        }
#endif
        vTaskDelay(10);
    }
}

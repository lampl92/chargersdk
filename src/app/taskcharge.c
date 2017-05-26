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
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                   defEventBitCPSwitchCondition,
                                                   pdFALSE, pdTRUE, 0);
                if((uxBitsCharge & defEventBitCPSwitchCondition) == defEventBitCPSwitchCondition)
                {
                    pCON->status.SetLoadPercent(pCON, 100);/** @fixme (rgw#1#): ����PWM����100%�������� */
                    THROW_ERROR(i, pCON->status.SetCPSwitch(pCON, SWITCH_ON), ERR_LEVEL_CRITICAL, "STATE_CON_PLUGED");
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
                    //��ǹ
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
                            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONStartOK);//rfid�����ڵȴ�
                            pCON->state = STATE_CON_CHARGING;
                            printf_safe("Start Charge!\n");
                            xTimerStart(xHandleTimerChargingData, 0);
                            vTaskDelay(5000);//����5s֮�䣬��ֹRFID��ˢ�����ȴ������ȶ���

                        }
                        /** @todo (rgw#1#): ����̵�������ʧ�ܣ�ת����ERR״̬ */
                    }
                }
                break;
            case STATE_CON_CHARGING:
                uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                                      defEventBitExceptionDevFault,
                                                      pdFALSE, pdFALSE, 0);
                if((uxBitsException & defEventBitExceptionDevFault) != 0)
                {
                    printf_safe("Dev Fault Stop Error!\n");
                    pCON->state = STATE_CON_ERROR;
                    break;
                }
                /*** �ж��û����ֹͣ����  ***/
                uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException,
                                                      defEventBitExceptionStopType,
                                                      pdTRUE, pdFALSE, 0);
                if((uxBitsException & defEventBitExceptionLimitFee) == defEventBitExceptionLimitFee)    //�ﵽ���������
                {
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeLimitFee);
                    xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                }
                if((uxBitsException & defEventBitExceptionRemoteStop) == defEventBitExceptionRemoteStop)    //Զ��ֹͣ
                {
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeRemoteStop);
                    xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                }
                if((uxBitsException & defEventBitExceptionRFIDStop) == defEventBitExceptionRFIDStop)    //ˢ��ֹͣ
                {
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeRFIDStop);
                    xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                }
                /******************************/

                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
//                printf_safe("uxBitsCharge = %X\n", uxBitsCharge);
//                printf_safe("CPCondition = %X\n", defEventBitChargeCondition);
                if((uxBitsCharge & defEventBitCONS2Opened) == defEventBitCONS2Opened) //6vpwm->9vpwm S2�����Ͽ�
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
                else if((uxBitsCharge & defEventBitChargeCondition) != defEventBitChargeCondition)//��ȥS2�����Ͽ��������������ĵ���False
                {
                    if((uxBitsCharge & defEventBitCONAuthed) != defEventBitCONAuthed)
                    {
                        //�û�ԭ��ֹͣ
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
                                                           pdFALSE, pdTRUE, 100);//S1ת����12V��S2Ӧ��100ms�ڶϿ�������ǿ�ƴ��ضϵ硣
                        //�˴�Ӧ���ж�uxbits���������������壬��Ϊ�������100ms�ڻ���100ms�ⶼҪ�ϵ硣
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
                    /** @todo (rgw#1#): �����������ж�ʧЧ�㣬����ʧЧ�������ʾ�����������ﲻ������ʾ�����ڷ���ʧЧʱ������ʾ */
                }
                break;
            case STATE_CON_STOPCHARGE:
                xTimerStop(xHandleTimerChargingData, 0);
                /** @todo (rgw#1#): �ȴ����
                                    ��ѳɹ���֪ͨHMI��ʾ������,����idle */
                xEventGroupSync(xHandleEventHMI,
                                defEventBitHMI_ChargeReqDispDone,
                                defeventBitHMI_ChargeReqDispDoneOK,
                                portMAX_DELAY );
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrder_HMIDispOK);//֪ͨOrder HMI��ʾ����ˡ�

                xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONStartOK);
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                   defEventBitCONOrderFinish,
                                                   pdTRUE, pdTRUE, portMAX_DELAY);
                if((uxBitsCharge & defEventBitCONOrderFinish) == defEventBitCONOrderFinish)
                {
                    //����
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
                                pCON->state = STATE_CON_IDLE;
                            }
                        }
                        else
                        {

                        }
                    }
                    else if(pCON->info.ucSocketType == defSocketTypeC)
                    {
                        pCON->state = STATE_CON_IDLE;
                    }
                }
                break;
            case STATE_CON_ERROR:
                THROW_ERROR(i, pCON->status.StopCharge(pCON), ERR_LEVEL_CRITICAL, "STATE_CON_ERROR");
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

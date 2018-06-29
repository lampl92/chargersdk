#include "HMI_Start.h"

typedef enum
{
    State_green_breath,
    State_green_keep_on,
    State_green_flicker,
    State_blue_keep_on,
    State_red_blue_replace,
    State_red_flicker,
    State_red_keep_on
}LED_State;

LED_State led_state;
/** @brief
 *  �ƹ����
 * @param
 * @param
 * @return
 *
 */
static void Led_Show()//����
{
    CON_t *pCON;
    uint8_t ledSignalPool;
    uint8_t led_signal;
    static uint8_t led_signalold;

    pCON = CONGetHandle(0);

    led_signal = 0;
    /**< ��λ˵���й��ϴ�����˸��� */
    if (bittest(winCreateFlag, 1))
    {
        bitset(led_signal, 0);
    }
    else
    {
        switch (pCON->state)
        {
        case STATE_CON_IDLE:
            /**< ����״̬ */
            bitset(led_signal, 1);
            break;
        case STATE_CON_CHARGING:
            /**< �������� */
            bitset(led_signal, 2);
            break;
        default:
            if (pCON->status.xPlugState == PLUG)
            {
                if (pCON->status.xCPState == CP_6V_PWM
                    || pCON->status.xCPState == CP_6V)
                {
                    /**< �ȴ����˲�ǹ */
                    bitset(led_signal, 3);
                }
                else if (pCON->status.xCPState == CP_9V_PWM
                    || pCON->status.xCPState == CP_9V)
                {
                    /**< S1δ�պ� */
                    bitset(led_signal, 4);
                }
            }
            else
            {
                /**< δ֪״̬ */
                bitset(led_signal, 5);
            }
            break;
        }
    }

    ledSignalPool = led_signalold ^ led_signal;
    if (ledSignalPool != 0)
    {
        ledSignalPool = ledSignalPool & led_signal;
        if (bittest(ledSignalPool, 0))
        {
            led_ctrl(1, red, flicker);
        }
        if (bittest(ledSignalPool, 1))
        {
            led_ctrl(1, green, keep_on);
        }
        if (bittest(ledSignalPool, 2))
        {
            led_ctrl(1, green, breath);
        }
        if (bittest(ledSignalPool, 3))
        {
            led_ctrl(1, green, flicker);
        }
        if (bittest(ledSignalPool, 4))
        {
            led_ctrl(1, blue, flicker);
        }
        if (bittest(ledSignalPool, 5))
        {
            led_ctrl(1, green, keep_on);
        }

        led_signalold = led_signal;
    }
}

/**
*�ƹ������
*
*/
static void ledShow1()//����
{
    int i;
    CON_t *pCON;
    uint8_t ledSignalPool[2];
    uint8_t led_signal[2];
    static uint8_t led_signalold[2];
    for (i = 0; i < 2; i++)
    {
        pCON = CONGetHandle(i);
        led_signal[i] = 0;
        /**< ��λ˵���й��ϴ�����˸��� */
        if (pCON->status.ulSignalAlarm != 0 ||
            pCON->status.ulSignalFault != 0 ||
            pEVSE->status.ulSignalAlarm != 0 ||
            pEVSE->status.ulSignalFault != 0)
        {
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltUp) == defSignalCON_Alarm_AC_A_VoltUp)
            {
                bitset(led_signal[i], 0);
            }
            else if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltLow) == defSignalCON_Alarm_AC_A_VoltLow)
            {
                bitset(led_signal[i], 0);
            }
            else if ((pCON->status.ulSignalFault & defSignalCON_Fault_CP) == defSignalCON_Fault_CP)
            {
                bitset(led_signal[i], 1);
            }
            else
            {
                bitset(led_signal[i], 6);
            }
        }
        else
        {
            switch (pCON->state)
            {
            case STATE_CON_IDLE:
                /**< ����״̬ */
                bitset(led_signal[i], 1);
                break;
            case STATE_CON_CHARGING:
                /**< �������� */
                bitset(led_signal[i], 2);
                break;
            default:
                if (pCON->status.xPlugState == PLUG)
                {
                    if (pCON->status.xCPState == CP_6V_PWM
                        || pCON->status.xCPState == CP_6V)
                    {
                        /**< �ȴ����˲�ǹ */
                        bitset(led_signal[i], 3);
                    }
                    else if (pCON->status.xCPState == CP_9V_PWM
                        || pCON->status.xCPState == CP_9V)
                    {
                        /**< S1δ�պ� */
                        bitset(led_signal[i], 4);
                    }
                }
                else
                {
                    /**< δ֪״̬ */
                    bitset(led_signal[i], 5);
                }
                break;
            }
        }
    }
   
    for (i = 0; i < 2; i++)
    {
        ledSignalPool[i] = led_signalold[i] ^ led_signal[i]; //�б仯��ledSignalPool��Ϊ0
        if (ledSignalPool[i] != 0)
        {
            ledSignalPool[i] = ledSignalPool[i] & led_signal[i];
            if (bittest(ledSignalPool[i], 0))
            {
                led_ctrl(i + 1, red, flicker);
            }
            if (bittest(ledSignalPool[i], 1))
            {
                led_ctrl(i + 1, green, keep_on);
            }
            if (bittest(ledSignalPool[i], 2))
            {
                led_ctrl(i + 1, green, breath);
            }
            if (bittest(ledSignalPool[i], 3))
            {
                led_ctrl(i + 1, green, flicker);
            }
            if (bittest(ledSignalPool[i], 4))
            {
                led_ctrl(i + 1, blue, keep_on);
            }
            if (bittest(ledSignalPool[i], 5))
            {
                led_ctrl(i + 1, green, keep_on);
            }
            if (bittest(ledSignalPool[i], 6))
            {
                led_ctrl(i + 1, red, keep_on);
            }
            led_signalold[i] = led_signal[i];
        }
    }
}

static void signal_error(CON_t *pCON, int i)
{
    if ((pEVSE->status.ulSignalState & defSignalEVSE_State_Network_Logined) != defSignalEVSE_State_Network_Logined)
    {
        led_state = State_red_blue_replace;
    }
    if ((pCON->status.ulSignalAlarm & ~defSignalGroupCON_Alarm_Temp_War) != 0 ||
    pCON->status.ulSignalFault != 0 ||
    (pEVSE->status.ulSignalAlarm & ~defSignalGroupEVSE_Alarm_Temp_War) != 0 ||
    pEVSE->status.ulSignalFault != 0)//�����¶ȸ澯�����ظ澯����ʾ
    {
        if ((pCON->status.ulSignalAlarm != 0)&&\
            (pCON->status.ulSignalFault == 0)&&\
            ((pCON->status.ulSignalAlarm | defSignalCON_Alarm_AC_A_VoltUp) == defSignalCON_Alarm_AC_A_VoltUp)&&\
            (pEVSE->status.ulSignalAlarm == 0)&&\
            (pEVSE->status.ulSignalFault == 0))
        {
            led_state = State_red_flicker;
        }
        else if ((pCON->status.ulSignalAlarm != 0)&&\
            (pCON->status.ulSignalFault == 0)&&\
            (pEVSE->status.ulSignalAlarm == 0)&&\
            (pEVSE->status.ulSignalFault == 0)&&\
            ((pCON->status.ulSignalAlarm | defSignalCON_Alarm_AC_A_VoltLow) == defSignalCON_Alarm_AC_A_VoltLow))
        {
            led_state = State_red_flicker;
        }
        else if ((pCON->status.ulSignalFault != 0)&&\
             (pCON->status.ulSignalAlarm == 0)&&\
             (pEVSE->status.ulSignalAlarm == 0)&&\
             (pEVSE->status.ulSignalFault == 0)&&\
    ((pCON->status.ulSignalFault & defSignalCON_Fault_CP) == defSignalCON_Fault_CP))
        {
            ;
        }
        else if (((pEVSE->status.ulSignalFault != 0)&&\
            (pEVSE->status.ulSignalAlarm == 0)&&\
            (pCON->status.ulSignalFault == 0)&&\
            (pCON->status.ulSignalAlarm == 0)&&\
            (pEVSE->status.ulSignalFault | defSignalEVSE_Fault_RFID) == defSignalEVSE_Fault_RFID))
        {
            ;
        }
        else if (((pEVSE->status.ulSignalFault != 0)&&\
            (pEVSE->status.ulSignalAlarm == 0)&&\
            (pCON->status.ulSignalFault != 0)&&\
            (pCON->status.ulSignalAlarm == 0)&&\
            (pEVSE->status.ulSignalFault | defSignalEVSE_Fault_RFID) == defSignalEVSE_Fault_RFID)&&\
            ((pCON->status.ulSignalFault & defSignalCON_Fault_CP) == defSignalCON_Fault_CP))
        {
            ;
        }
        else
        {
            led_state = State_red_keep_on;
        }
    }
}

static void ledShow(int j)
{
    int i;
    CON_t *pCON;
    for (i = 0; i < j; i++)
    {
        pCON = CONGetHandle(i);
        if ((pCON->status.ulSignalState & defSignalCON_State_Working) == defSignalCON_State_Working)//�ڳ����
        {
            led_state = State_green_breath;
            signal_error(pCON, i);
        }
        else//���ڳ���� 
        {
            led_state = State_green_keep_on;
            if (pCON->status.xPlugState == PLUG)
            {
                if (pCON->status.xCPState == CP_6V_PWM
                    || pCON->status.xCPState == CP_6V)
                {
                    /**< �ȴ����˲�ǹ */
                    led_state = State_green_flicker;
                }
                else if (pCON->status.xCPState == CP_9V_PWM
                    || pCON->status.xCPState == CP_9V)
                {
                    /**< S1δ�պ� */
                    led_state = State_blue_keep_on;
                }
            }
            signal_error(pCON, i);
        }
        switch (led_state)
        {
        case State_green_breath:
            led_ctrl(i+1, green, breath);
            break;
        case State_green_keep_on:
            led_ctrl(i+1, green, keep_on);
            break;
        case State_green_flicker:
            led_ctrl(i+1, green, flicker);
            break;
        case State_blue_keep_on:
            led_ctrl(i+1, blue, keep_on);
            break;
        case State_red_blue_replace:
            led_ctrl(1, blue, keep_on);
            led_ctrl(2, blue, keep_on);
            vTaskDelay(500);
            led_ctrl(1, red, keep_on);
            led_ctrl(2, red, keep_on);
            vTaskDelay(400);
            break;
        case State_red_flicker:
            led_ctrl(i+1, red, flicker);
            break;
        case State_red_keep_on:
            led_ctrl(i+1, red, keep_on);
            break;
        }
    }
}

//����ǹ�����Ĳ�ͬ���ò�ͬ�ĵƹ����
void ledcontrl()
{
    led_state = State_green_keep_on;
    while (1)
    {
        if (pEVSE->info.ucTotalCON == 1)
        {
            ledShow(1);
        }
        else
        {
            ledShow(2);
        } 
        vTaskDelay(100);
    }
}
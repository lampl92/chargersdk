#include "includes.h"
#include "task.h"
#include "interface.h"
uint8_t acTaskStatusBuffer[500];

void cli_tasklist_fnt(int argc, char **argv)
{
    uint32_t FreeBytesRemaining = 0;
    uint32_t memused;
    double usedkb;
    uint8_t paddr[20];

    printf_safe("\r\n");
    vTaskList((char *)&acTaskStatusBuffer);
    printf_safe("=================================================\r\n");
    printf_safe("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
    printf_safe("%s\r\n", acTaskStatusBuffer);
    printf_safe("\r\n������       ���м���         ʹ����\r\n");
    vTaskGetRunTimeStats((char *)&acTaskStatusBuffer);
    printf_safe("%s\r\n", acTaskStatusBuffer);

    FreeBytesRemaining = xPortGetFreeHeapSize();
    memused = (configTOTAL_HEAP_SIZE - FreeBytesRemaining) * 100 / configTOTAL_HEAP_SIZE;
    xsprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
    usedkb = (configTOTAL_HEAP_SIZE - FreeBytesRemaining) / 1024;
    printf_safe("\nSDRAM ʹ����: %s��%.2lf KB\n", paddr, usedkb);
}

void cli_evsestatus_fnt(int argc, char **argv)
{
    CON_t *pCON;
    int i;
    printf_safe("����=========״̬��1���쳣 0��������=======   EVSE\r\n");
    printf_safe("���ף�        %d\n", pEVSE->status.ulArresterState);
    printf_safe("��ײ�Ƕȣ�    %d\n", pEVSE->status.ulKnockState );
    printf_safe("PE״̬��      %d\n", pEVSE->status.ulPEState);
    printf_safe("���磺        %d\n", pEVSE->status.ulPowerOffState);
    printf_safe("\n");
    for(i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        printf_safe("����=========״̬=======   CONID %d\r\n", i);
        printf_safe("CP״̬��      ");
        switch(pCON->status.xCPState)
        {
        case CP_ERR:
            printf_safe("CP_ERR");
            break;
        case CP_12V:
            printf_safe("CP_12V");
            break;
        case CP_12V_PWM:
            printf_safe("CP_12V_PWM");
            break;
        case CP_9V:
            printf_safe("CP_9V");
            break;
        case CP_9V_PWM:
            printf_safe("CP_9V_PWM");
            break;
        case CP_6V:
            printf_safe("CP_6V");
            break;
        case CP_6V_PWM:
            printf_safe("CP_6V_PWM");
            break;
        default:
            break;
        }
        printf_safe("\n");
        ///////////CC
        printf_safe("CC״̬��      ");
        switch(pCON->status.xCCState)
        {
        case CC_NO:
            printf_safe("CC_NO");
            break;
        case CC_PE:
            printf_safe("CC_PE");
            break;
        default:
            break;
        }
        printf_safe("\n");
        /////////// PLUG
        printf_safe("��ǹ״̬��    ");
        switch(pCON->status.xPlugState)
        {
        case PLUG:
            printf_safe("PLUG");
            break;
        case UNPLUG:
            printf_safe("UNPLUG");
            break;
        default:
            break;
        }
        printf_safe("\n");
        ///////////
        printf_safe("ǹ��״̬��      ");
        switch(pCON->status.xBTypeSocketLockState)
        {
        case LOCK:
            printf_safe("LOCK");
            break;
        case UNLOCK:
            printf_safe("UNLOCK");
            break;
        default:
            break;
        }
        printf_safe("\n");

        printf_safe("���ذٷֱȣ�  %d\n", pCON->status.ucLoadPercent);
        printf_safe("L�¶ȣ�       %.2lf\n", pCON->status.dACLTemp);
        printf_safe("N�¶ȣ�       %.2lf\n", pCON->status.dACNTemp);
        printf_safe("ǹ���¶�1��   %.2lf\n", pCON->status.dBTypeSocketTemp1);
        printf_safe("ǹ���¶�2��   %.2lf\n", pCON->status.dBTypeSocketTemp2);
        printf_safe("����ѹ��    %.2lf\n", pCON->status.dChargingVoltage);
        printf_safe("��������    %.2lf\n", pCON->status.dChargingCurrent);
        printf_safe("��ѹƵ�ʣ�    %.2lf\n", pCON->status.dChargingFrequence);
        printf_safe("��������    %.2lf\n", pCON->status.dChargingPower);
        printf_safe("�̵�����      L %d N %d\n", pCON->status.ucRelayLState, pCON->status.ucRelayNState);

        printf_safe("\n");
    }
}
tinysh_cmd_t cli_tasklist_cmd =
{
    0,
    "tasklist",
    "display task status",
    0,
    cli_tasklist_fnt,
    "<cr>", 0, 0
};
tinysh_cmd_t cli_evsestatus_cmd =
{
    0,
    "evsestatus",
    "display evse status",
    0,
    cli_evsestatus_fnt,
    "<cr>", 0, 0
};

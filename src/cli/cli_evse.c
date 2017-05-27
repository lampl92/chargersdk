/**
* @file cli_evse.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-18
*/

#include "includes.h"
#include "task.h"
#include "interface.h"
#include "gdsl_types.h"
#include "gdsl_list.h"
#include "gdsl_perm.h"
#include <time.h>

void cli_evseinfo_fnt(int argc, char **argv)
{
    CON_t *pCON;
    int i;
    /**/
    printf_safe("=============������Ϣ=============\n");
    printf_safe("EVSE SN:       ");
    for(i = 0; i < pEVSE->info.ucSNLength; i++)
    {
        printf_safe("%02d", pEVSE->info.ucSN[i]);
    }
    printf_safe("\n");
    /**/
    printf_safe("EVSE ID:       ");
    for(i = 0; i < pEVSE->info.ucIDLength; i++)
    {
        printf_safe("%02d", pEVSE->info.ucID[i]);
    }
    printf_safe("\n");
    /**/
    printf_safe("�豸���ͣ�     ");
    switch(pEVSE->info.ucType)
    {
    case defEVSEType_DC:
        printf_safe("ֱ�����׮");
        break;
    case defEVSEType_AC:
        printf_safe("�������׮");
        break;
    case defEVSEType_AC_DC:
        printf_safe("��ֱ��һ��");
        break;
    case defEVSEType_Wireless:
        printf_safe("�����豸");
        break;
    case defEVSEType_Other:
        printf_safe("����");
        break;
    default:
        break;
    }
    printf_safe("\n");
    /**/
    printf_safe("���ǹ����:     %d\n", pEVSE->info.ucTotalCON);
    /**/
    printf_safe("��γ�� Lng,Lat (%.6lf , %.6lf)\n", pEVSE->info.dLng, pEVSE->info.dLat);
    /**/
    printf_safe("���������:     ");
    switch(pEVSE->info.ucServiceFeeType)
    {
    case defOrderSerType_Order:
        printf_safe("����");
        break;
    case defOrderSerType_Power:
        printf_safe("����");
        break;
    default:
        break;
    }
    printf_safe("\n");
    /**/
    printf_safe("�����:         %.2lf\n", pEVSE->info.dServiceFee);
    /**/
    printf_safe("Ĭ�϶ε��:     %.2lf\n", pEVSE->info.dDefSegFee);
    /**/
    uint8_t listsize_dbg = gdsl_list_get_size(pEVSE->info.plTemplSeg);
    printf_safe("��ʱ�θ���:     %d\n", listsize_dbg);
    struct tm *ts_dbg;
    TemplSeg_t *tmlseg_dgb;

    for(i = 1; i <= listsize_dbg; i++)
    {
        tmlseg_dgb = (TemplSeg_t *)(gdsl_list_search_by_position(pEVSE->info.plTemplSeg, i));
        ts_dbg = localtime(&(tmlseg_dgb->tStartTime));
        printf_safe("ʱ�� %d  StartTime:%02d:%02d | ",
                    i , ts_dbg->tm_hour, ts_dbg->tm_min  );
        ts_dbg = localtime(&(tmlseg_dgb->tEndTime));
        printf_safe("EndTime:%02d:%02d | ",
                    ts_dbg->tm_hour, ts_dbg->tm_min  );
        printf_safe("SegFee:%.2lf\n",
                    tmlseg_dgb->dSegFee );
    }

    for(i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        printf_safe("\n");
        pCON = CONGetHandle(i);
        printf_safe("[ ǹID: %d ]\n", pCON->info.ucCONID);

        printf_safe("ǹ����:        ");
        switch(pCON->info.ucCONType)
        {
        case defCONType_AC:
            printf_safe("����");
            break;
        case defCONType_DC:
            printf_safe("ֱ��");
            break;
        default:
            break;
        }
        printf_safe("\n");

        printf_safe("�ӿ�����:      ");
        switch(pCON->info.ucSocketType)
        {
        case defSocketTypeB:
            printf_safe("B������");
            break;
        case defSocketTypeC:
            printf_safe("C������");
            break;
        default:
            break;
        }
        printf_safe("\n");

        printf_safe("��ѹ��Χ:      %.2lf ~ %.2lf\n", pCON->info.dVolatageLowerLimits, pCON->info.dVolatageUpperLimits);
        printf_safe("AC�¶ȷ�Χ:    %.2lf ~ %.2lf\n", pCON->info.dACTempLowerLimits, pCON->info.dACTempUpperLimits);
        printf_safe("B��ǹ�¶ȷ�Χ: %.2lf ~ %.2lf\n", pCON->info.dSocketTempLowerLimits, pCON->info.dSocketTempUpperLimits);
        printf_safe("�����:      %.2lf\n", pCON->info.dRatedCurrent);
        printf_safe("�����:      %.2lf\n", pCON->info.dRatedPower);
        printf_safe("QRCode  :      %s\n", pCON->info.strQRCode);
    }
}
void cli_evseorder_fnt(int argc, char **argv)
{

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
tinysh_cmd_t cli_evseinfo_cmd =
{
    0,
    "evseinfo",
    "display evse info",
    0,
    cli_evseinfo_fnt,
    "<cr>", 0, 0
};
tinysh_cmd_t cli_evseorder_cmd =
{
    0,
    "evseorder",
    "display evse order",
    0,
    cli_evseorder_fnt,
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

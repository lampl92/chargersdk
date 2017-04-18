#include "includes.h"
#include "task.h"
#include "interface.h"
/**
* @file D:\Documents\Projects\chargersdk\src\cli\cli_evse.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-18
*/
void cli_evsecfg_fnt(int argc, char **argv)
{

}
void cli_evseorder_fnt(int argc, char **argv)
{

}
void cli_evsestatus_fnt(int argc, char **argv)
{
    CON_t *pCON;
    int i;
    printf_safe("Ãû³Æ=========×´Ì¬£¨1£ºÒì³£ 0£ºÕý³££©=======   EVSE\r\n");
    printf_safe("·ÀÀ×£º        %d\n", pEVSE->status.ulArresterState);
    printf_safe("·À×²½Ç¶È£º    %d\n", pEVSE->status.ulKnockState );
    printf_safe("PE×´Ì¬£º      %d\n", pEVSE->status.ulPEState);
    printf_safe("µôµç£º        %d\n", pEVSE->status.ulPowerOffState);
    printf_safe("\n");
    for(i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        printf_safe("Ãû³Æ=========×´Ì¬=======   CONID %d\r\n", i);
        printf_safe("CP×´Ì¬£º      ");
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
        printf_safe("CC×´Ì¬£º      ");
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
        printf_safe("²åÇ¹×´Ì¬£º    ");
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
        printf_safe("Ç¹Ëø×´Ì¬£º      ");
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

        printf_safe("¸ºÔØ°Ù·Ö±È£º  %d\n", pCON->status.ucLoadPercent);
        printf_safe("LÎÂ¶È£º       %.2lf\n", pCON->status.dACLTemp);
        printf_safe("NÎÂ¶È£º       %.2lf\n", pCON->status.dACNTemp);
        printf_safe("Ç¹×ùÎÂ¶È1£º   %.2lf\n", pCON->status.dBTypeSocketTemp1);
        printf_safe("Ç¹×ùÎÂ¶È2£º   %.2lf\n", pCON->status.dBTypeSocketTemp2);
        printf_safe("³äµçµçÑ¹£º    %.2lf\n", pCON->status.dChargingVoltage);
        printf_safe("³äµçµçÁ÷£º    %.2lf\n", pCON->status.dChargingCurrent);
        printf_safe("µçÑ¹ÆµÂÊ£º    %.2lf\n", pCON->status.dChargingFrequence);
        printf_safe("³äµçµçÁ¿£º    %.2lf\n", pCON->status.dChargingPower);
        printf_safe("¼ÌµçÆ÷£º      L %d N %d\n", pCON->status.ucRelayLState, pCON->status.ucRelayNState);

        printf_safe("\n");
    }
}
tinysh_cmd_t cli_evsecfg_cmd =
{
    0,
    "evsecfg",
    "display evse cfg",
    0,
    cli_evsecfg_fnt,
    "<cr>", 0, 0
};
tinysh_cmd_t cli_evseorder_cmd =
{
    0,
    "evsestatus",
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

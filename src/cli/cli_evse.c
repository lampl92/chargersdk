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
    printf_safe("=============配置信息=============\n");
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
        printf_safe("%02X", pEVSE->info.ucID[i]);
    }
    printf_safe("\n");
    /**/
    printf_safe("设备类型：     ");
    switch(pEVSE->info.ucType)
    {
    case defEVSEType_DC:
        printf_safe("直流充电桩");
        break;
    case defEVSEType_AC:
        printf_safe("交流充电桩");
        break;
    case defEVSEType_AC_DC:
        printf_safe("交直流一体");
        break;
    case defEVSEType_Wireless:
        printf_safe("无线设备");
        break;
    case defEVSEType_Other:
        printf_safe("其他");
        break;
    default:
        break;
    }
    printf_safe("\n");
    /**/
    printf_safe("充电枪个数:     %d\n", pEVSE->info.ucTotalCON);
    /**/
    printf_safe("经纬度 Lng,Lat (%.6lf , %.6lf)\n", pEVSE->info.dLng, pEVSE->info.dLat);
    /**/
    printf_safe("服务费类型:     ");
    switch(pEVSE->info.ucServiceFeeType)
    {
    case defOrderSerType_Order:
        printf_safe("按单");
        break;
    case defOrderSerType_Power:
        printf_safe("按度");
        break;
    default:
        break;
    }
    printf_safe("\n");
    /**/
    printf_safe("服务费:         %.2lf\n", pEVSE->info.dServiceFee);
    /**/
    printf_safe("默认段电费:     %.2lf\n", pEVSE->info.dDefSegFee);
    /**/
    uint8_t listsize_dbg = gdsl_list_get_size(pEVSE->info.plTemplSeg);
    printf_safe("总时段个数:     %d\n", listsize_dbg);
    struct tm *ts_dbg;
    TemplSeg_t *tmlseg_dgb;

    for(i = 1; i <= listsize_dbg; i++)
    {
        tmlseg_dgb = (TemplSeg_t *)(gdsl_list_search_by_position(pEVSE->info.plTemplSeg, i));
        ts_dbg = localtime(&(tmlseg_dgb->tStartTime));
        printf_safe("时段 %d  StartTime:%02d:%02d | ",
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
        printf_safe("[ 枪ID: %d ]\n", pCON->info.ucCONID);

        printf_safe("枪类型:        ");
        switch(pCON->info.ucCONType)
        {
        case defCONType_AC:
            printf_safe("交流");
            break;
        case defCONType_DC:
            printf_safe("直流");
            break;
        default:
            break;
        }
        printf_safe("\n");

        printf_safe("接口类型:      ");
        switch(pCON->info.ucSocketType)
        {
        case defSocketTypeB:
            printf_safe("B型连接");
            break;
        case defSocketTypeC:
            printf_safe("C型连接");
            break;
        default:
            break;
        }
        printf_safe("\n");

        printf_safe("电压范围:      %.2lf ~ %.2lf\n", pCON->info.dVolatageLowerLimits, pCON->info.dVolatageUpperLimits);
        printf_safe("AC温度范围:    %.2lf ~ %.2lf\n", pCON->info.dACTempLowerLimits, pCON->info.dACTempUpperLimits);
        printf_safe("B型枪温度范围: %.2lf ~ %.2lf\n", pCON->info.dSocketTempLowerLimits, pCON->info.dSocketTempUpperLimits);
        printf_safe("额定电流:      %.2lf\n", pCON->info.dRatedCurrent);
        printf_safe("额定功率:      %.2lf\n", pCON->info.dRatedPower);
        printf_safe("QRCode  :      %s\n", pCON->info.strQRCode);
    }
}

void cli_evseorder_fnt(int argc, char **argv)
{
    CON_t *pCON;
    OrderState_t statOrder;//记录订单状态
    char buf [80];
    struct tm *ts;
    int i;

    for(i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        printf_safe("名称=========状态=======   CONID %d\r\n", i);
        switch(statOrder)
        {
        case STATE_ORDER_IDLE:
            printf_safe("订单状态：IDLE");
            break;
        case STATE_ORDER_TMP:
            printf_safe("订单状态：IDLE");
            break;
        case STATE_ORDER_WAITSTART:
            printf_safe("订单状态：WAITSTART");
            break;
        case STATE_ORDER_MAKE:
            printf_safe("订单状态：MAKE");
            break;
        case STATE_ORDER_UPDATE:
            printf_safe("订单状态：UPDATE");
            break;
        case STATE_ORDER_FINISH:
            printf_safe("订单状态：FINISH");
            break;
        }
        //Card ID
        printf_safe("CardID:        ");
        for(i = 0; i < defCardIDLength; i++)
        {
            printf_safe("%02X ", pCON->order.ucCardID[i]);
        }
        printf_safe("\n");
         //帐户状态 1：注册卡 2:欠费 0：未注册卡
        printf_safe("账户状态:      ");
        switch(pCON->order.ucAccountStatus)
        {
        case 1:
                printf_safe(" 注册卡");
                break;
        case 2:
                printf_safe("欠费");
                break;
        case 0:
                printf_safe("未注册");
                break;
        }
        printf_safe("\n");
        //double  dBalance;           //余额
        printf_safe("余额:            %.2lf", pCON->order.dBalance);
        //uint8_t ucStartType;   //4 有卡 5 无卡
        if(pCON->order.ucStartType == 4)
        {
            printf_safe("启动方式:        有卡\n");
        }
        else
        {
            printf_safe("启动方式:        网络\n");
        }
        //uint8_t strOrderSN[defOrderSNLength]; //交易流水号
        printf_safe("交易流水号:         %s\n", pCON->order.strOrderSN);
        //double dLimitFee;                      //充电截至金额
        printf_safe("充电截止金额:        %.2lf\n", pCON->order.dLimitFee);
        ts = localtime (& pCON->order.tStartTime);
        strftime (buf, sizeof (buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
        printf_safe("启动时间:          %s \n", buf);
        printf_safe("启动时电表读数    %.2lf\n", pCON->order.dStartPower);
        printf_safe("服务费类型:     ");
        switch(pCON->order.ucServiceFeeType)
        {
        case defOrderSerType_Order:
            printf_safe("按单");
            break;
        case defOrderSerType_Power:
            printf_safe("按度");
            break;
        default:
            break;
        }
        printf_safe("\n");
        printf_safe("========充电过程数据=========\n");
        printf_safe("总电量:       %.2lf\n", pCON->order.dTotalPower);
        printf_safe("总电费:       %.2lf\n", pCON->order.dTotalPowerFee);
        printf_safe("总服务费:     %.2lf\n", pCON->order.dTotalServiceFee);
        printf_safe("总费用:       %.2lf\n", pCON->order.dTotalFee);
        printf_safe("充电明细段数: %d\n", pCON->order.ucTotalSegment);
        printf_safe("默认段起始电量: %.2lf\n", pCON->order.dDefSegStartPower);
        printf_safe("默认段电量:   %.2lf\n", pCON->order.dDefSegPower);
        printf_safe("默认段电费:   %.2lf\n", pCON->order.dDefSegFee);
        printf_safe("========停止时数据=========\n");
        if(pCON->order.ucPayType == defOrderPayType_Online)
        {
            printf_safe("支付方式:      在线支付\n");
        }
        else
        {
            printf_safe("支付方式:      离线支付\n");
        }
        printf_safe("停止类型:         ");
        switch(pCON->order.ucStopType)
        {
        case defOrderStopType_RFID:
             printf_safe("RFID\n");
             break;
        case defOrderStopType_Remote:
            printf_safe("remote\n");
            break;
        case defOrderStopType_Full:
            printf_safe("充满停止\n");
            break;
        case defOrderStopType_Fee:
             printf_safe("达到充电金额\n");//达到充电金额
            break;
        case defOrderStopType_Scram:
        case defOrderStopType_NetLost:
        case defOrderStopType_Poweroff:
        case defOrderStopType_OverCurr:
        case defOrderStopType_Knock:
            printf_safe("异常停止\n");//异常停止
            break;
        default:
            printf_safe("其他原因停止\n");;//其他原因停止
            break;
        }
        ts = localtime (& pCON->order.tStopTime);
        strftime (buf, sizeof (buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
        printf_safe("停止时间:          %s \n", buf);
    }
}
void cli_evsestatus_fnt(int argc, char **argv)
{
    CON_t *pCON;
    int i;
    printf_safe("名称=========状态（1：异常 0：正常）=======   EVSE\r\n");
    printf_safe("防雷：        %d\n", pEVSE->status.ulArresterState);
    printf_safe("防撞角度：    %d\n", pEVSE->status.ulKnockState );
    printf_safe("PE状态：      %d\n", pEVSE->status.ulPEState);
    printf_safe("掉电：        %d\n", pEVSE->status.ulPowerOffState);
    printf_safe("\n");
    for(i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        printf_safe("名称=========状态=======   CONID %d\r\n", i);
        printf_safe("CP状态：      ");
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
        printf_safe("CC状态：      ");
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
        printf_safe("插枪状态：    ");
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
        printf_safe("枪锁状态：      ");
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

        printf_safe("负载百分比：  %d\n", pCON->status.ucLoadPercent);
        printf_safe("L温度：       %.2lf\n", pCON->status.dACLTemp);
        printf_safe("N温度：       %.2lf\n", pCON->status.dACNTemp);
        printf_safe("枪座温度1：   %.2lf\n", pCON->status.dBTypeSocketTemp1);
        printf_safe("枪座温度2：   %.2lf\n", pCON->status.dBTypeSocketTemp2);
        printf_safe("充电电压：    %.2lf\n", pCON->status.dChargingVoltage);
        printf_safe("充电电流：    %.2lf\n", pCON->status.dChargingCurrent);
        printf_safe("电压频率：    %.2lf\n", pCON->status.dChargingFrequence);
        printf_safe("电表读数：    %.2lf\n", pCON->status.dChargingPower);
        printf_safe("继电器：      L %d N %d\n", pCON->status.ucRelayLState, pCON->status.ucRelayNState);

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

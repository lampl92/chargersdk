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
#include "modem.h"
#include "stringName.h" 
#include "cfg_order.h"
#include "ifconfig.h"
#include "evse_globals.h"

void cli_networkinfo_fnt(int argc, char **argv)
{
    printf_safe("=============信息=============\n");
    
    printf_safe("HostName:\t%s\n", ifconfig.info.strHostName);
    printf_safe("Adapter:\t%d\n", ifconfig.info.ucAdapterSel);
    printf_safe("DHCP:\t%d\n", ifconfig.info.ucDHCPEnable);
    printf_safe("MAC:\t%s\n", ifconfig.info.strMAC);
    printf_safe("IP:\t%s\n", ifconfig.info.strIP);
    printf_safe("Gate:\t%s\n", ifconfig.info.strGate);
    printf_safe("Mask:\t%s\n", ifconfig.info.strMask);
    printf_safe("DNS1:\t%s\n", ifconfig.info.strDNS1);
    printf_safe("DNS2:\t%s\n", ifconfig.info.strDNS2);
    
    printf_safe("=============状态=============\n");
    
    printf_safe("IP:\t%s\n", ifconfig.status.strIP);
    printf_safe("Gate:\t%s\n", ifconfig.status.strGate);
    printf_safe("Mask:\t%s\n", ifconfig.status.strMask);
    printf_safe("DNS1:\t%s\n", ifconfig.status.strDNS1);
    printf_safe("DNS2:\t%s\n", ifconfig.status.strDNS2);
}
void cli_modeminfo_fnt(int argc, char **argv)
{
    printf_safe("=============模块信息=============\n");
    printf_safe("制造商: %s\n", pModem->info.strManufacturer);
    printf_safe("模块: %s\n", pModem->info.strDeviceModule);
    printf_safe("=============SIM卡状态=============\n");
    printf_safe("ICCID: %s\n", pModem->info.strICCID);
    printf_safe("插卡状态：      ");
    if(pModem->status.eSimStat == CPIN_READY)
    {
        printf_safe("READY\n");
    }
    else if(pModem->status.eSimStat == CPIN_OTHER)
    {
        printf_safe("Other\n");
    }
    printf_safe("网络注册信息：    ");
    switch(pModem->status.eNetReg)
    {
    case REG_LOCAl:
        printf_safe("本地卡\n");
        break;
    case REG_ROAMING:
        printf_safe("漫游卡\n");
        break;
    default:
        printf_safe("有问题\n");
        break;
    }
    printf_safe("GPRS网络注册信息：");
    switch(pModem->status.eGprsReg)
    {
    case REG_LOCAl:
        printf_safe("本地卡\n");
        break;
    case REG_ROAMING:
        printf_safe("漫游卡\n");
        break;
    default:
        printf_safe("有问题\n");
        break;
    }
    printf_safe("信号强度：  %d\n", pModem->status.ucSignalQuality);
    printf_safe("本地地址：  %s\n", pModem->status.strLocIP);
}
void cli_protoinfo_fnt(int argc, char **argv)
{
    char tbuf [80] = {0};
    int i;
    struct tm *ts;

    if (pechProto->info.tNewKeyChangeTime != 0)
    {
        ts = localtime(&pechProto->info.tNewKeyChangeTime);
        strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", ts);
    }

    printf_safe("=============协议信息=============\n");
    printf_safe("服务器IP：\t%s\n", pechProto->info.strServerIP);
    printf_safe("服务器端口：\t%d\n", pechProto->info.usServerPort);
    printf_safe("用户名：\t%s\n", pechProto->info.strUserName);
    printf_safe("用户密码：\t%s\n", pechProto->info.strUserPwd);
    printf_safe("密钥：\t\t%s\n", pechProto->info.strKey);
    printf_safe("新密钥：\t%s\n", pechProto->info.strNewKey);
    printf_safe("密钥更改时间：\t%s\n", tbuf);
    printf_safe("协议版本号：\t%d\n", pechProto->info.ucProtoVer);
    printf_safe("心跳周期：\t%d\n", pechProto->info.ulHeartBeatCyc_ms);
    printf_safe("状态周期：\t%d\n", pechProto->info.ulStatusCyc_ms);
    printf_safe("实时数据周期：\t%d\n", pechProto->info.ulRTDataCyc_ms);
    printf_safe("重启标志：\t%d\n", pechProto->info.ucResetAct);
    printf_safe("=== 尖 ===\n");
    printf_safe("尖电费率：\t%.4lf\n", pechProto->info.dSegEnergyFee[0]);
    printf_safe("尖服务费率：\t%.4lf\n", pechProto->info.dSegServFee[0]);
    printf_safe("尖时段数量：\t%d\n", pechProto->info.SegTime[0].ucPeriodCont);
    printf_safe("时段:\t");
    for(i = 0; i < pechProto->info.SegTime[0].ucPeriodCont; i++)
    {
        printf_safe("%d：%d-%d\t", i + 1, pechProto->info.SegTime[0].ucStart[i], pechProto->info.SegTime[0].ucEnd[i]);
    }
    printf_safe("\n");
    printf_safe("=== 峰 ===\n");
    printf_safe("峰电费率：\t%.4lf\n", pechProto->info.dSegEnergyFee[1]);
    printf_safe("峰服务费率：\t%.4lf\n", pechProto->info.dSegServFee[1]);
    printf_safe("峰时段数量：\t%d\n", pechProto->info.SegTime[1].ucPeriodCont);
    printf_safe("时段: \t");
    for(i = 0; i < pechProto->info.SegTime[1].ucPeriodCont; i++)
    {
        printf_safe("%d：%d-%d\t", i + 1, pechProto->info.SegTime[1].ucStart[i], pechProto->info.SegTime[1].ucEnd[i]);
    }
    printf_safe("\n");
    printf_safe("=== 平 ===\n");
    printf_safe("平电费率：\t%.4lf\n", pechProto->info.dSegEnergyFee[2]);
    printf_safe("平服务费率：\t%.4lf\n", pechProto->info.dSegServFee[2]);
    printf_safe("平时段数量：\t%d\n", pechProto->info.SegTime[2].ucPeriodCont);
    printf_safe("时段: \t");
    for(i = 0; i < pechProto->info.SegTime[2].ucPeriodCont; i++)
    {
        printf_safe("%d：%d-%d\t", i + 1, pechProto->info.SegTime[2].ucStart[i], pechProto->info.SegTime[2].ucEnd[i]);
    }
    printf_safe("\n");
    printf_safe("=== 谷 ===\n");
    printf_safe("谷电费率：\t%.4lf\n", pechProto->info.dSegEnergyFee[3]);
    printf_safe("谷服务费率：\t%.4lf\n", pechProto->info.dSegServFee[3]);
    printf_safe("谷时段数量：\t%d\n", pechProto->info.SegTime[3].ucPeriodCont);
    printf_safe("时段: \t");
    for(i = 0; i < pechProto->info.SegTime[3].ucPeriodCont; i++)
    {
        printf_safe("%d：%d-%d\t", i + 1, pechProto->info.SegTime[3].ucStart[i], pechProto->info.SegTime[3].ucEnd[i]);
    }
    printf_safe("\n");
}
void cli_evseinfo_fnt(int argc, char **argv)
{
    CON_t *pCON;
    int i;
    /**/
    printf_safe("=============配置信息=============\n");
    printf_safe("EVSE SN:       %s\n", pEVSE->info.strSN);
    /**/
    printf_safe("EVSE ID:       %s\n", pEVSE->info.strID);
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
    case defOrderSerType_Energy:
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
    printf_safe("Version:%s\n", xSysconf.strVersion);
}


extern ErrorCode_t testmakeOrder(CON_t *pCON, time_t testtime, OrderState_t statOrder);
extern int  testSearchOrderCfg(char *path, time_t time_start, time_t time_end);
void cli_evseorder_fnt(int argc, char **argv)
{
    CON_t *pCON;
    char buf [80];
    struct tm *ts;
    int i, t;
    time_t now_dummy;
    int id;

    if (argc == 1)
    {
        testSearchOrderCfg(pathOrder, 0, 0);
        return;
    }
    else if (argc == 2)
    {
        pCON = CONGetHandle(atoi(argv[1]));
        if (pCON != NULL)
        {
            printf_safe("名称=========状态=======   CONID %d\r\n", id);
            switch (pCON->order.statOrder)
            {
            default:
            case STATE_ORDER_IDLE:
                printf_safe("订单状态：\tIDLE");
                break;
            case STATE_ORDER_TMP:
                printf_safe("订单状态：\tIDLE");
                break;
            case STATE_ORDER_WAITSTART:
                printf_safe("订单状态：\tWAITSTART");
                break;
            case STATE_ORDER_MAKE:
                printf_safe("订单状态：\tMAKE");
                break;
            case STATE_ORDER_UPDATE:
                printf_safe("订单状态：\tUPDATE");
                break;
            case STATE_ORDER_FINISH:
                printf_safe("订单状态：\tFINISH");
                break;
            }
            printf_safe("\n");
            //Card ID
            printf_safe("CardID:\t%s\n", pCON->order.strCardID);
            //帐户状态 1：注册卡 2:欠费 0：未注册卡
            printf_safe("账户状态:\t");
            switch (pCON->order.ucAccountStatus)
            {
            case 1:
                printf_safe("注册卡");
                break;
            case 2:
                printf_safe("欠费");
                break;
            case 0:
                printf_safe("未注册");
                break;
            }
            printf_safe("\n");
            //dBalance;           //余额
            printf_safe("余额:\t\t%.2lf\n", pCON->order.dBalance);
            //ucStartType;   //4 有卡 5 无卡
            if(pCON->order.ucStartType == 4)
            {
                printf_safe("启动方式:\t有卡\n");
            }
            else
            {
                printf_safe("启动方式:\t网络\n");
            }
            //ullOrderSN; //交易流水号
            printf_safe("交易流水号:\t%:.0lf\n", (double)(pCON->order.ullOrderSN));
            //dLimitFee;                      //充电截至金额
            printf_safe("充电截止金额:\t%.2lf\n", pCON->order.dLimitFee);
            ts = localtime(& pCON->order.tStartTime);
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
            printf_safe("启动时间:\t%s \n", buf);
            printf_safe("启动时电表读数\t%.2lf\n", pCON->order.dStartEnergy);

            printf_safe("========充电过程数据=========\n");
            printf_safe("总电量:       %.2lf\n", pCON->order.dTotalEnergy);
            printf_safe("总电费:       %.2lf\n", pCON->order.dTotalEnergyFee);
            printf_safe("总服务费:     %.2lf\n", pCON->order.dTotalServFee);
            printf_safe("总费用:       %.2lf\n", pCON->order.dTotalFee);
            printf_safe("尖电价：      %.4lf\n", pechProto->info.dSegEnergyFee[0]);
            printf_safe("尖服务费单价  %.4lf\n", pechProto->info.dSegServFee[0]);
            printf_safe("尖电量        %.2lf\n", pCON->order.dSegTotalEnergy[0]);
            printf_safe("尖充电金额    %.2lf\n", pCON->order.dSegTotalEnergyFee[0]);
            printf_safe("尖服务费金额  %.2lf\n", pCON->order.dSegTotalServFee[0]);
            printf_safe("尖充电时长    %d\n", pCON->order.ulSegTotalTime[0]);
            printf_safe("尖详细数据：\n");
            for (i = 0; i < 5; i++)
            {
                if (pCON->order.chargeSegStatus[0][i].tStartTime > 0)
                {
                    printf_safe("\t时段 %d\n", i + 1);

                    ts = localtime(& pCON->order.chargeSegStatus[0][i].tStartTime);
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t开始时间：%s\n", buf);

                    ts = localtime(& pCON->order.chargeSegStatus[0][i].tEndTime);
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t结束时间：%s\n", buf);

                    printf_safe("\t开始电量：%.2lf\n", pCON->order.chargeSegStatus[0][i].dStartEnergy);
                    printf_safe("\t时段电量：%.2lf\n", pCON->order.chargeSegStatus[0][i].dEnergy);
                }
            }
            printf_safe("峰电价：      %.4lf\n", pechProto->info.dSegEnergyFee[1]);
            printf_safe("峰服务费单价  %.4lf\n", pechProto->info.dSegServFee[1]);
            printf_safe("峰电量        %.2lf\n", pCON->order.dSegTotalEnergy[1]);
            printf_safe("峰充电金额    %.2lf\n", pCON->order.dSegTotalEnergyFee[1]);
            printf_safe("峰服务费金额  %.2lf\n", pCON->order.dSegTotalServFee[1]);
            printf_safe("峰充电时长    %d\n", pCON->order.ulSegTotalTime[1]);
            printf_safe("峰详细数据：\n");
            for (i = 0; i < 5; i++)
            {
                if (pCON->order.chargeSegStatus[1][i].tStartTime > 0)
                {
                    printf_safe("\t时段 %d\n", i + 1);

                    ts = localtime(& pCON->order.chargeSegStatus[1][i].tStartTime);
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t开始时间：%s\n", buf);

                    ts = localtime(& pCON->order.chargeSegStatus[1][i].tEndTime);
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t结束时间：%s\n", buf);

                    printf_safe("\t开始电量：%.2lf\n", pCON->order.chargeSegStatus[1][i].dStartEnergy);
                    printf_safe("\t时段电量：%.2lf\n", pCON->order.chargeSegStatus[1][i].dEnergy);
                }
            }
            printf_safe("平电价：      %.4lf\n", pechProto->info.dSegEnergyFee[2]);
            printf_safe("平服务费单价  %.4lf\n", pechProto->info.dSegServFee[2]);
            printf_safe("平电量        %.2lf\n", pCON->order.dSegTotalEnergy[2]);
            printf_safe("平充电金额    %.2lf\n", pCON->order.dSegTotalEnergyFee[2]);
            printf_safe("平服务费金额  %.2lf\n", pCON->order.dSegTotalServFee[2]);
            printf_safe("平充电时长    %d\n", pCON->order.ulSegTotalTime[2]);
            printf_safe("平详细数据：\n");
            for (i = 0; i < 5; i++)
            {
                if (pCON->order.chargeSegStatus[2][i].tStartTime > 0)
                {
                    printf_safe("\t时段 %d\n", i + 1);

                    ts = localtime(& pCON->order.chargeSegStatus[2][i].tStartTime);
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t开始时间：%s\n", buf);

                    ts = localtime(& pCON->order.chargeSegStatus[2][i].tEndTime);
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t结束时间：%s\n", buf);

                    printf_safe("\t开始电量：%.2lf\n", pCON->order.chargeSegStatus[2][i].dStartEnergy);
                    printf_safe("\t时段电量：%.2lf\n", pCON->order.chargeSegStatus[2][i].dEnergy);
                }
            }
            printf_safe("谷电价：      %.4lf\n", pechProto->info.dSegEnergyFee[3]);
            printf_safe("谷服务费单价  %.4lf\n", pechProto->info.dSegServFee[3]);
            printf_safe("谷电量        %.2lf\n", pCON->order.dSegTotalEnergy[3]);
            printf_safe("谷充电金额    %.2lf\n", pCON->order.dSegTotalEnergyFee[3]);
            printf_safe("谷服务费金额  %.2lf\n", pCON->order.dSegTotalServFee[3]);
            printf_safe("谷充电时长    %d\n", pCON->order.ulSegTotalTime[3]);
            printf_safe("谷详细数据：\n");
            for (i = 0; i < 5; i++)
            {
                if (pCON->order.chargeSegStatus[3][i].tStartTime > 0)
                {
                    printf_safe("\t时段 %d\n", i + 1);

                    ts = localtime(& pCON->order.chargeSegStatus[3][i].tStartTime);
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t开始时间：%s\n", buf);

                    ts = localtime(& pCON->order.chargeSegStatus[3][i].tEndTime);
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t结束时间：%s\n", buf);

                    printf_safe("\t开始电量：%.2lf\n", pCON->order.chargeSegStatus[3][i].dStartEnergy);
                    printf_safe("\t时段电量：%.2lf\n", pCON->order.chargeSegStatus[3][i].dEnergy);
                }
            }
            printf_safe("========停止时数据=========\n");
            if (pCON->order.ucPayType == defOrderPayType_Online)
            {
                printf_safe("支付方式:\t在线支付\n");
            }
            else
            {
                printf_safe("支付方式:\t离线支付\n");
            }
            printf_safe("停止类型:\t");
            switch (pCON->order.ucStopType)
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
                printf_safe("达到充电金额\n"); //达到充电金额
                break;
            case defOrderStopType_Scram:
            case defOrderStopType_Offline:
            case defOrderStopType_Poweroff:
            case defOrderStopType_OverCurr:
            case defOrderStopType_Knock:
                printf_safe("异常停止\n"); //异常停止
                break;
            default:
                printf_safe("其他原因停止\n");
                 ;//其他原因停止
                break;
            }
            ts = localtime(& pCON->order.tStopTime);
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
            printf_safe("停止时间:\t%s \n", buf);
        }
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
	printf_safe("进线A温度：   %.2lf\n", pEVSE->status.dAC_A_Temp_IN);
	printf_safe("进线B温度：   %.2lf\n", pEVSE->status.dAC_B_Temp_IN);
	printf_safe("进线C温度：   %.2lf\n", pEVSE->status.dAC_C_Temp_IN);
	printf_safe("进线N温度：   %.2lf\n", pEVSE->status.dAC_N_Temp_IN);
    printf_safe("\n");
    for(i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        printf_safe("名称=========状态=======   CONID %d\r\n", i);
        printf_safe("CP Volt   %.2lf\n", pCON->status.dCPVolt);
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
        printf_safe("电表功率：    %.2lf\n", pCON->status.dChargingPower);
        printf_safe("电表读数：    %.2lf\n", pCON->status.dChargingEnergy);
        printf_safe("继电器：      L %d N %d\n", pCON->status.ucRelayLState, pCON->status.ucRelayNState);

        printf_safe("\n");
    }
}

extern int  testSearchEVSELogByTime(char *path, time_t time_start, time_t time_end);
void cli_evselog_fnt(int argc, char **argv)
{
    testSearchEVSELogByTime(pathEVSELog, 0, 0);
}
void cli_setload_fnt(int argc, char **argv)
{
    CON_t *pCON;
    uint8_t id;
    uint8_t percent;
    if (argc == 3)
    {
        id = atoi(argv[1]);
        pCON = CONGetHandle(id);
        if (pCON != NULL)
        {
            percent = atoi(argv[2]);
            if (percent >= 0 && percent <= 100)
            {
                pCON->status.SetLoadPercent(pCON, percent);
            }
            else
            {
                printf_safe("Percent range(0~100) error.\n");
            }
        }
        else
        {
            printf_safe("CON ID error, total con is %d.\n", pEVSE->info.ucTotalCON);
        }
    }
    else
    {
        printf_safe("help:\"setload 0 50\" means set con0 load to 50%%\n");
    }
}
void cli_meter_fnt(int argc, char **argv)
{
    printf("\r\n=========电表配置=========\n");
    switch (xSysconf.xModule.use_meter)
    {
    case 0:
        printf("电表：无电表\n");
        break;
    case 1:
        printf("电表：内部模块\n");
        break;
    case 2:
        printf("电表：DDSD1352-C(安科瑞单相)\n");
        break;
    case 3:
        printf("电表：DTSD1352(III)\n");
        break;
    case 4:
        printf("电表：DTSF1352安科瑞三相\n");
        break;
    default:
        printf("电表：假电表\n");
        break;
    }
    printf("寄存器:\n");
    printf("电度:\t%04XH\n", meter->regs.energy_addr);
    printf("电压:\t%04XH\n", meter->regs.volt_addr);
    printf("电流:\t%04XH\n", meter->regs.curr_addr);
    printf("功率:\t%04XH\n", meter->regs.pwr_addr);
    printf("频率:\t%04XH\n", meter->regs.freq_addr);
    
}
int dummyordersn;
void cli_dummyordersn_fnt(int argc, char **argv)
{
    if (argc == 2)
    {
        dummyordersn = atoi(argv[1]);
    }
    else
    {
        printf_safe("dummyordersn %d\n", dummyordersn);
    }
}
tinysh_cmd_t cli_networkinfo_cmd =
{
    0,
    "ifconfig",
    "display network info",
    0,
    cli_networkinfo_fnt,
    "<cr>",
    0,
    0
};
tinysh_cmd_t cli_protoinfo_cmd =
{
    0,
    "protoinfo",
    "display proto info",
    0,
    cli_protoinfo_fnt,
    "<cr>", 0, 0
};
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
tinysh_cmd_t cli_modeminfo_cmd =
{
    0,
    "modeminfo",
    "display modem info",
    0,
    cli_modeminfo_fnt,
    "<cr>", 0, 0
};
tinysh_cmd_t cli_evselog_cmd =
{
    0,
    "evselog",
    "display evse log",
    0,
    cli_evselog_fnt,
    "<cr>",
    0,
    0
};
tinysh_cmd_t cli_setload_cmd =
{
    0,
    "setload",
    "set evse load present",
    0,
    cli_setload_fnt,
    "<cr>",
    0,
    0
};

tinysh_cmd_t cli_meter_cmd =
{
    0,
    "meter",
    "meter info",
    0,
    cli_meter_fnt,
    "<cr>",
    0,
    0
};
tinysh_cmd_t cli_dummyordersn_cmd =
{
    0,
    "dummyordersn",
    "dummy order sn",
    0,
    cli_dummyordersn_fnt,
    "<cr>",
    0,
    0
};
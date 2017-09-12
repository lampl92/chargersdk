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

void cli_modeminfo_fnt(int argc, char **argv)
{
    printf_safe("=============信息=============\n");

    printf_safe("=============状态=============\n");
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
    printf_safe("尖电费率：\t%.4lf\n", pechProto->info.dPowerFee_sharp);
    printf_safe("尖服务费率：\t%.4lf\n", pechProto->info.dServFee_sharp);
    printf_safe("尖时段数量：\t%d\n", pechProto->info.SegTime_sharp.ucSegCont);
    printf_safe("时段:\t");
    for(i = 0; i < pechProto->info.SegTime_sharp.ucSegCont; i++)
    {
        printf_safe("%d：%d-%d\t", i + 1, pechProto->info.SegTime_sharp.ucStart[i], pechProto->info.SegTime_sharp.ucEnd[i]);
    }
    printf_safe("\n");
    printf_safe("=== 峰 ===\n");
    printf_safe("峰电费率：\t%.4lf\n", pechProto->info.dPowerFee_peak);
    printf_safe("峰服务费率：\t%.4lf\n", pechProto->info.dServFee_peak);
    printf_safe("峰时段数量：\t%d\n", pechProto->info.SegTime_peak.ucSegCont);
    printf_safe("时段: \t");
    for(i = 0; i < pechProto->info.SegTime_peak.ucSegCont; i++)
    {
        printf_safe("%d：%d-%d\t", i + 1, pechProto->info.SegTime_peak.ucStart[i], pechProto->info.SegTime_peak.ucEnd[i]);
    }
    printf_safe("\n");
    printf_safe("=== 平 ===\n");
    printf_safe("平电费率：\t%.4lf\n", pechProto->info.dPowerFee_shoulder);
    printf_safe("平服务费率：\t%.4lf\n", pechProto->info.dServFee_shoulder);
    printf_safe("平时段数量：\t%d\n", pechProto->info.SegTime_shoulder.ucSegCont);
    printf_safe("时段: \t");
    for(i = 0; i < pechProto->info.SegTime_shoulder.ucSegCont; i++)
    {
        printf_safe("%d：%d-%d\t", i + 1, pechProto->info.SegTime_shoulder.ucStart[i], pechProto->info.SegTime_shoulder.ucEnd[i]);
    }
    printf_safe("\n");
    printf_safe("=== 谷 ===\n");
    printf_safe("谷电费率：\t%.4lf\n", pechProto->info.dPowerFee_off_peak);
    printf_safe("谷服务费率：\t%.4lf\n", pechProto->info.dServFee_off_peak);
    printf_safe("谷时段数量：\t%d\n", pechProto->info.SegTime_off_peak.ucSegCont);
    printf_safe("时段: \t");
    for(i = 0; i < pechProto->info.SegTime_off_peak.ucSegCont; i++)
    {
        printf_safe("%d：%d-%d\t", i + 1, pechProto->info.SegTime_off_peak.ucStart[i], pechProto->info.SegTime_off_peak.ucEnd[i]);
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
    printf_safe("软件版本:      %s\n", pEVSE->info.strSoftVer);
}


extern ErrorCode_t testmakeOrder(CON_t *pCON, time_t testtime, OrderState_t statOrder);
extern int  testSearchOrderCfg(char *path, time_t time_start, time_t time_end);
void cli_evseorder_fnt(int argc, char **argv)
{
    CON_t *pCON;
    OrderState_t statOrder;//记录订单状态
    char buf [80];
    struct tm *ts;
    int i, t;
    time_t now_dummy;
    int id;

    if(strcmp(argv[1], "--all") == 0 || strcmp(argv[1], "-a") == 0 )
    {
        testSearchOrderCfg(pathOrder, 0, 0);
        return;
    }
    else
    {
        for(id = 0; id < pEVSE->info.ucTotalCON; id++)
        {
            pCON = CONGetHandle(id);

//            now_dummy = 1500944498;//2017-07-25 09:01:38
//            now_dummy = 1500987698;//21 //跨天，同时段
            now_dummy = 1501141978;
//            now_dummy = 1501142404;//2017/7/27 16:0:4  //跨天，不同时段
            testmakeOrder(pCON, 0 , STATE_ORDER_TMP);
            testmakeOrder(pCON, now_dummy , STATE_ORDER_MAKE); //2017-07-25 09:01:38
            testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE); //2017-07-25 09:01:38
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 10:01:38 22
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 10:01:38
//                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 11:01:38 23
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 11:01:38
//                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 12:01:38 24
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 12:01:38
//                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 13:01:38 1
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 13:01:38
//                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 14:01:38 2
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 14:01:38
//                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 15:01:38 3
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 15:01:38
//                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 16:01:38 4
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 16:01:38
//                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 17:01:38 5
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 17:01:38
//                vTaskDelay(1);
            }

            testmakeOrder(pCON, now_dummy+1 , STATE_ORDER_FINISH); //2017-07-25 17:01:39

            printf_safe("名称=========状态=======   CONID %d\r\n", id);
            switch(statOrder)
            {
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
            printf_safe("CardID:\t");
            for(i = 0; i < defCardIDLength; i++)
            {
                printf_safe("%02X ", pCON->order.ucCardID[i]);
            }
            printf_safe("\n");
            //帐户状态 1：注册卡 2:欠费 0：未注册卡
            printf_safe("账户状态:\t");
            switch(pCON->order.ucAccountStatus)
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
            //strOrderSN[defOrderSNLength]; //交易流水号
            printf_safe("交易流水号:\t%s\n", pCON->order.strOrderSN);
            //dLimitFee;                      //充电截至金额
            printf_safe("充电截止金额:\t%.2lf\n", pCON->order.dLimitFee);
            ts = localtime (& pCON->order.tStartTime);
            strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
            printf_safe("启动时间:\t%s \n", buf);
            printf_safe("启动时电表读数\t%.2lf\n", pCON->order.dStartPower);

            printf_safe("========充电过程数据=========\n");
            printf_safe("总电量:       %.2lf\n", pCON->order.dTotalPower);
            printf_safe("总电费:       %.2lf\n", pCON->order.dTotalPowerFee);
            printf_safe("总服务费:     %.2lf\n", pCON->order.dTotalServFee);
            printf_safe("总费用:       %.2lf\n", pCON->order.dTotalFee);
            printf_safe("尖电价：      %.4lf\n", pechProto->info.dPowerFee_sharp);
            printf_safe("尖服务费单价  %.4lf\n", pechProto->info.dServFee_sharp);
            printf_safe("尖电量        %.2lf\n", pCON->order.dTotalPower_sharp);
            printf_safe("尖充电金额    %.2lf\n", pCON->order.dTotalPowerFee_sharp);
            printf_safe("尖服务费金额  %.2lf\n", pCON->order.dTotalServFee_sharp);
            printf_safe("尖充电时长    %d\n", pCON->order.ulTotalTime_sharp);
            printf_safe("尖详细数据：\n");
            for(i = 0; i < 5; i++)
            {
                if(pCON->order.chargeSegStatus_sharp[i].tStartTime > 0)
                {
                    printf_safe("\t时段 %d\n", i+1);

                    ts = localtime (& pCON->order.chargeSegStatus_sharp[i].tStartTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t开始时间：%s\n", buf);

                    ts = localtime (& pCON->order.chargeSegStatus_sharp[i].tEndTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t结束时间：%s\n", buf);

                    printf_safe("\t开始电量：%.2lf\n", pCON->order.chargeSegStatus_sharp[i].dStartPower);
                    printf_safe("\t时段电量：%.2lf\n", pCON->order.chargeSegStatus_sharp[i].dPower);
                }
            }
            printf_safe("峰电价：      %.4lf\n", pechProto->info.dPowerFee_peak);
            printf_safe("峰服务费单价  %.4lf\n", pechProto->info.dServFee_peak);
            printf_safe("峰电量        %.2lf\n", pCON->order.dTotalPower_peak);
            printf_safe("峰充电金额    %.2lf\n", pCON->order.dTotalPowerFee_peak);
            printf_safe("峰服务费金额  %.2lf\n", pCON->order.dTotalServFee_peak);
            printf_safe("峰充电时长    %d\n", pCON->order.ulTotalTime_peak);
            printf_safe("峰详细数据：\n");
            for(i = 0; i < 5; i++)
            {
                if(pCON->order.chargeSegStatus_peak[i].tStartTime > 0)
                {
                    printf_safe("\t时段 %d\n", i+1);

                    ts = localtime (& pCON->order.chargeSegStatus_peak[i].tStartTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t开始时间：%s\n", buf);

                    ts = localtime (& pCON->order.chargeSegStatus_peak[i].tEndTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t结束时间：%s\n", buf);

                    printf_safe("\t开始电量：%.2lf\n", pCON->order.chargeSegStatus_peak[i].dStartPower);
                    printf_safe("\t时段电量：%.2lf\n", pCON->order.chargeSegStatus_peak[i].dPower);
                }
            }
            printf_safe("平电价：      %.4lf\n", pechProto->info.dPowerFee_shoulder);
            printf_safe("平服务费单价  %.4lf\n", pechProto->info.dServFee_shoulder);
            printf_safe("平电量        %.2lf\n", pCON->order.dTotalPower_shoulder);
            printf_safe("平充电金额    %.2lf\n", pCON->order.dTotalPowerFee_shoulder);
            printf_safe("平服务费金额  %.2lf\n", pCON->order.dTotalServFee_shoulder);
            printf_safe("平充电时长    %d\n", pCON->order.ulTotalTime_shoulder);
            printf_safe("平详细数据：\n");
            for(i = 0; i < 5; i++)
            {
                if(pCON->order.chargeSegStatus_shoulder[i].tStartTime > 0)
                {
                    printf_safe("\t时段 %d\n", i+1);

                    ts = localtime (& pCON->order.chargeSegStatus_shoulder[i].tStartTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t开始时间：%s\n", buf);

                    ts = localtime (& pCON->order.chargeSegStatus_shoulder[i].tEndTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t结束时间：%s\n", buf);

                    printf_safe("\t开始电量：%.2lf\n", pCON->order.chargeSegStatus_shoulder[i].dStartPower);
                    printf_safe("\t时段电量：%.2lf\n", pCON->order.chargeSegStatus_shoulder[i].dPower);
                }
            }
            printf_safe("谷电价：      %.4lf\n", pechProto->info.dPowerFee_off_peak);
            printf_safe("谷服务费单价  %.4lf\n", pechProto->info.dServFee_off_peak);
            printf_safe("谷电量        %.2lf\n", pCON->order.dTotalPower_off_peak);
            printf_safe("谷充电金额    %.2lf\n", pCON->order.dTotalPowerFee_off_peak);
            printf_safe("谷服务费金额  %.2lf\n", pCON->order.dTotalServFee_off_peak);
            printf_safe("谷充电时长    %d\n", pCON->order.ulTotalTime_off_peak);
            printf_safe("谷详细数据：\n");
            for(i = 0; i < 5; i++)
            {
                if(pCON->order.chargeSegStatus_off_peak[i].tStartTime > 0)
                {
                    printf_safe("\t时段 %d\n", i+1);

                    ts = localtime (& pCON->order.chargeSegStatus_off_peak[i].tStartTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t开始时间：%s\n", buf);

                    ts = localtime (& pCON->order.chargeSegStatus_off_peak[i].tEndTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t结束时间：%s\n", buf);

                    printf_safe("\t开始电量：%.2lf\n", pCON->order.chargeSegStatus_off_peak[i].dStartPower);
                    printf_safe("\t时段电量：%.2lf\n", pCON->order.chargeSegStatus_off_peak[i].dPower);
                }
            }
            printf_safe("========停止时数据=========\n");
            if(pCON->order.ucPayType == defOrderPayType_Online)
            {
                printf_safe("支付方式:\t在线支付\n");
            }
            else
            {
                printf_safe("支付方式:\t离线支付\n");
            }
            printf_safe("停止类型:\t");
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
            strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
            printf_safe("停止时间:\t%s \n", buf);
            printf_safe("add start time = %d\n", clock());
            for (i = 0; i < 1; i++)
            {
                pCON->order.tStartTime = time(NULL) + i * 100;
                AddOrderCfg(pathOrder, pCON, pechProto);
            }
            printf_safe("end time = %d\n", clock());
            //OrderDBInsertItem(&(pCON->order));
            OrderInit(&(pCON->order));
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

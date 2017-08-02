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
#include "dbparser.h"
#include "query_output.h"

void cli_modeminfo_fnt(int argc, char **argv)
{
    printf_safe("=============��Ϣ=============\n");

    printf_safe("=============״̬=============\n");
    printf_safe("�忨״̬��      ");
    if(pModem->status.eSimStat == CPIN_READY)
    {
        printf_safe("READY\n");
    }
    else if(pModem->status.eSimStat == CPIN_OTHER)
    {
        printf_safe("Other\n");
    }
    printf_safe("����ע����Ϣ��    ");
    switch(pModem->status.eNetReg)
    {
    case REG_LOCAl:
        printf_safe("���ؿ�\n");
        break;
    case REG_ROAMING:
        printf_safe("���ο�\n");
        break;
    default:
        printf_safe("������\n");
        break;
    }
    printf_safe("GPRS����ע����Ϣ��");
    switch(pModem->status.eGprsReg)
    {
    case REG_LOCAl:
        printf_safe("���ؿ�\n");
        break;
    case REG_ROAMING:
        printf_safe("���ο�\n");
        break;
    default:
        printf_safe("������\n");
        break;
    }
    printf_safe("�ź�ǿ�ȣ�  %d\n", pModem->status.ucSignalQuality);
    printf_safe("���ص�ַ��  %s\n", pModem->status.strLocIP);
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

    printf_safe("=============Э����Ϣ=============\n");
    printf_safe("������IP��\t%s\n", pechProto->info.strServerIP);
    printf_safe("�������˿ڣ�\t%d\n", pechProto->info.usServerPort);
    printf_safe("�û�����\t%s\n", pechProto->info.strUserName);
    printf_safe("�û����룺\t%s\n", pechProto->info.strUserPwd);
    printf_safe("��Կ��\t\t%s\n", pechProto->info.strKey);
    printf_safe("����Կ��\t%s\n", pechProto->info.strNewKey);
    printf_safe("��Կ����ʱ�䣺\t%s\n", tbuf);
    printf_safe("Э��汾�ţ�\t%d\n", pechProto->info.ucProtoVer);
    printf_safe("�������ڣ�\t%d\n", pechProto->info.ulHeartBeatCyc_ms);
    printf_safe("״̬���ڣ�\t%d\n", pechProto->info.ulStatusCyc_ms);
    printf_safe("ʵʱ�������ڣ�\t%d\n", pechProto->info.ulRTDataCyc_ms);
    printf_safe("������־��\t%d\n", pechProto->info.ucResetAct);
    printf_safe("=== �� ===\n");
    printf_safe("�����ʣ�\t%.4lf\n", pechProto->info.dPowerFee_sharp);
    printf_safe("�������ʣ�\t%.4lf\n", pechProto->info.dServFee_sharp);
    printf_safe("��ʱ��������\t%d\n", pechProto->info.SegTime_sharp.ucSegCont);
    printf_safe("ʱ��:\t");
    for(i = 0; i < pechProto->info.SegTime_sharp.ucSegCont; i++)
    {
        printf_safe("%d��%d-%d\t", i + 1, pechProto->info.SegTime_sharp.ucStart[i], pechProto->info.SegTime_sharp.ucEnd[i]);
    }
    printf_safe("\n");
    printf_safe("=== �� ===\n");
    printf_safe("�����ʣ�\t%.4lf\n", pechProto->info.dPowerFee_peak);
    printf_safe("�������ʣ�\t%.4lf\n", pechProto->info.dServFee_peak);
    printf_safe("��ʱ��������\t%d\n", pechProto->info.SegTime_peak.ucSegCont);
    printf_safe("ʱ��: \t");
    for(i = 0; i < pechProto->info.SegTime_peak.ucSegCont; i++)
    {
        printf_safe("%d��%d-%d\t", i + 1, pechProto->info.SegTime_peak.ucStart[i], pechProto->info.SegTime_peak.ucEnd[i]);
    }
    printf_safe("\n");
    printf_safe("=== ƽ ===\n");
    printf_safe("ƽ����ʣ�\t%.4lf\n", pechProto->info.dPowerFee_shoulder);
    printf_safe("ƽ������ʣ�\t%.4lf\n", pechProto->info.dServFee_shoulder);
    printf_safe("ƽʱ��������\t%d\n", pechProto->info.SegTime_shoulder.ucSegCont);
    printf_safe("ʱ��: \t");
    for(i = 0; i < pechProto->info.SegTime_shoulder.ucSegCont; i++)
    {
        printf_safe("%d��%d-%d\t", i + 1, pechProto->info.SegTime_shoulder.ucStart[i], pechProto->info.SegTime_shoulder.ucEnd[i]);
    }
    printf_safe("\n");
    printf_safe("=== �� ===\n");
    printf_safe("�ȵ���ʣ�\t%.4lf\n", pechProto->info.dPowerFee_off_peak);
    printf_safe("�ȷ�����ʣ�\t%.4lf\n", pechProto->info.dServFee_off_peak);
    printf_safe("��ʱ��������\t%d\n", pechProto->info.SegTime_off_peak.ucSegCont);
    printf_safe("ʱ��: \t");
    for(i = 0; i < pechProto->info.SegTime_off_peak.ucSegCont; i++)
    {
        printf_safe("%d��%d-%d\t", i + 1, pechProto->info.SegTime_off_peak.ucStart[i], pechProto->info.SegTime_off_peak.ucEnd[i]);
    }
    printf_safe("\n");
}
void cli_evseinfo_fnt(int argc, char **argv)
{
    CON_t *pCON;
    int i;
    /**/
    printf_safe("=============������Ϣ=============\n");
    printf_safe("EVSE SN:       %s\n", pEVSE->info.strSN);
    /**/
    printf_safe("EVSE ID:       %s\n", pEVSE->info.strID);
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


extern ErrorCode_t testmakeOrder(CON_t *pCON, time_t testtime, OrderState_t statOrder);
void cli_evseorder_fnt(int argc, char **argv)
{
    CON_t *pCON;
    OrderState_t statOrder;//��¼����״̬
    char buf [80];
    struct tm *ts;
    int i, t;
    time_t now_dummy;
    int id;

    char memseg[1024];
    db_query_mm_t mm;
    db_op_base_t* root;

    if(strcmp(argv[1], "--all") == 0 || strcmp(argv[1], "-a") == 0 )
    {
        init_query_mm(&mm, memseg, 1024);
        root = parse("SELECT * FROM OrderDB;", &mm);
        printQuery(root, &mm);
        closeexecutiontree(root, &mm);
        return;
    }
    else
    {
        for(id = 0; id < pEVSE->info.ucTotalCON; id++)
        {
            pCON = CONGetHandle(id);

            now_dummy = 1500944498;//2017-07-25 09:01:38
//            now_dummy = 1500987698;//21 //���죬ͬʱ��
//            now_dummy = 1501141978;
//            now_dummy = 1501142404;//2017/7/27 16:0:4  //���죬��ͬʱ��
            testmakeOrder(pCON, 0 , STATE_ORDER_TMP);
            testmakeOrder(pCON, now_dummy , STATE_ORDER_MAKE); //2017-07-25 09:01:38
            testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE); //2017-07-25 09:01:38
            printf_safe("start time = %d\n", clock());
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 10:01:38 22
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 10:01:38
                vTaskDelay(1);
            }
            printf_safe("end time = %d\n", clock());
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 11:01:38 23
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 11:01:38
                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 12:01:38 24
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 12:01:38
                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 13:01:38 1
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 13:01:38
                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 14:01:38 2
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 14:01:38
                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 15:01:38 3
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 15:01:38
                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 16:01:38 4
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 16:01:38
                vTaskDelay(1);
            }
            for(t = 0; t < 3600; t++)
            {
                testmakeOrder(pCON, ++now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 17:01:38 5
                testmakeOrder(pCON, now_dummy , STATE_ORDER_UPDATE);    //2017-07-25 17:01:38
                vTaskDelay(1);
            }

            testmakeOrder(pCON, now_dummy+1 , STATE_ORDER_FINISH); //2017-07-25 17:01:39

            printf_safe("����=========״̬=======   CONID %d\r\n", id);
            switch(statOrder)
            {
            case STATE_ORDER_IDLE:
                printf_safe("����״̬��\tIDLE");
                break;
            case STATE_ORDER_TMP:
                printf_safe("����״̬��\tIDLE");
                break;
            case STATE_ORDER_WAITSTART:
                printf_safe("����״̬��\tWAITSTART");
                break;
            case STATE_ORDER_MAKE:
                printf_safe("����״̬��\tMAKE");
                break;
            case STATE_ORDER_UPDATE:
                printf_safe("����״̬��\tUPDATE");
                break;
            case STATE_ORDER_FINISH:
                printf_safe("����״̬��\tFINISH");
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
            //�ʻ�״̬ 1��ע�Ῠ 2:Ƿ�� 0��δע�Ῠ
            printf_safe("�˻�״̬:\t");
            switch(pCON->order.ucAccountStatus)
            {
            case 1:
                printf_safe("ע�Ῠ");
                break;
            case 2:
                printf_safe("Ƿ��");
                break;
            case 0:
                printf_safe("δע��");
                break;
            }
            printf_safe("\n");
            //dBalance;           //���
            printf_safe("���:\t\t%.2lf\n", pCON->order.dBalance);
            //ucStartType;   //4 �п� 5 �޿�
            if(pCON->order.ucStartType == 4)
            {
                printf_safe("������ʽ:\t�п�\n");
            }
            else
            {
                printf_safe("������ʽ:\t����\n");
            }
            //strOrderSN[defOrderSNLength]; //������ˮ��
            printf_safe("������ˮ��:\t%s\n", pCON->order.strOrderSN);
            //dLimitFee;                      //���������
            printf_safe("����ֹ���:\t%.2lf\n", pCON->order.dLimitFee);
            ts = localtime (& pCON->order.tStartTime);
            strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
            printf_safe("����ʱ��:\t%s \n", buf);
            printf_safe("����ʱ������\t%.2lf\n", pCON->order.dStartPower);

            printf_safe("========����������=========\n");
            printf_safe("�ܵ���:       %.2lf\n", pCON->order.dTotalPower);
            printf_safe("�ܵ��:       %.2lf\n", pCON->order.dTotalPowerFee);
            printf_safe("�ܷ����:     %.2lf\n", pCON->order.dTotalServFee);
            printf_safe("�ܷ���:       %.2lf\n", pCON->order.dTotalFee);
            printf_safe("���ۣ�      %.4lf\n", pechProto->info.dPowerFee_sharp);
            printf_safe("�����ѵ���  %.4lf\n", pechProto->info.dServFee_sharp);
            printf_safe("�����        %.2lf\n", pCON->order.dTotalPower_sharp);
            printf_safe("������    %.2lf\n", pCON->order.dTotalPowerFee_sharp);
            printf_safe("�����ѽ��  %.2lf\n", pCON->order.dTotalServFee_sharp);
            printf_safe("����ʱ��    %d\n", pCON->order.ulTotalTime_sharp);
            printf_safe("����ϸ���ݣ�\n");
            for(i = 0; i < 5; i++)
            {
                if(pCON->order.chargeSegStatus_sharp[i].tStartTime > 0)
                {
                    printf_safe("\tʱ�� %d\n", i+1);

                    ts = localtime (& pCON->order.chargeSegStatus_sharp[i].tStartTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t��ʼʱ�䣺%s\n", buf);

                    ts = localtime (& pCON->order.chargeSegStatus_sharp[i].tEndTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t����ʱ�䣺%s\n", buf);

                    printf_safe("\t��ʼ������%.2lf\n", pCON->order.chargeSegStatus_sharp[i].dStartPower);
                    printf_safe("\tʱ�ε�����%.2lf\n", pCON->order.chargeSegStatus_sharp[i].dPower);
                }
            }
            printf_safe("���ۣ�      %.4lf\n", pechProto->info.dPowerFee_peak);
            printf_safe("�����ѵ���  %.4lf\n", pechProto->info.dServFee_peak);
            printf_safe("�����        %.2lf\n", pCON->order.dTotalPower_peak);
            printf_safe("������    %.2lf\n", pCON->order.dTotalPowerFee_peak);
            printf_safe("�����ѽ��  %.2lf\n", pCON->order.dTotalServFee_peak);
            printf_safe("����ʱ��    %d\n", pCON->order.ulTotalTime_peak);
            printf_safe("����ϸ���ݣ�\n");
            for(i = 0; i < 5; i++)
            {
                if(pCON->order.chargeSegStatus_peak[i].tStartTime > 0)
                {
                    printf_safe("\tʱ�� %d\n", i+1);

                    ts = localtime (& pCON->order.chargeSegStatus_peak[i].tStartTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t��ʼʱ�䣺%s\n", buf);

                    ts = localtime (& pCON->order.chargeSegStatus_peak[i].tEndTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t����ʱ�䣺%s\n", buf);

                    printf_safe("\t��ʼ������%.2lf\n", pCON->order.chargeSegStatus_peak[i].dStartPower);
                    printf_safe("\tʱ�ε�����%.2lf\n", pCON->order.chargeSegStatus_peak[i].dPower);
                }
            }
            printf_safe("ƽ��ۣ�      %.4lf\n", pechProto->info.dPowerFee_shoulder);
            printf_safe("ƽ����ѵ���  %.4lf\n", pechProto->info.dServFee_shoulder);
            printf_safe("ƽ����        %.2lf\n", pCON->order.dTotalPower_shoulder);
            printf_safe("ƽ�����    %.2lf\n", pCON->order.dTotalPowerFee_shoulder);
            printf_safe("ƽ����ѽ��  %.2lf\n", pCON->order.dTotalServFee_shoulder);
            printf_safe("ƽ���ʱ��    %d\n", pCON->order.ulTotalTime_shoulder);
            printf_safe("ƽ��ϸ���ݣ�\n");
            for(i = 0; i < 5; i++)
            {
                if(pCON->order.chargeSegStatus_shoulder[i].tStartTime > 0)
                {
                    printf_safe("\tʱ�� %d\n", i+1);

                    ts = localtime (& pCON->order.chargeSegStatus_shoulder[i].tStartTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t��ʼʱ�䣺%s\n", buf);

                    ts = localtime (& pCON->order.chargeSegStatus_shoulder[i].tEndTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t����ʱ�䣺%s\n", buf);

                    printf_safe("\t��ʼ������%.2lf\n", pCON->order.chargeSegStatus_shoulder[i].dStartPower);
                    printf_safe("\tʱ�ε�����%.2lf\n", pCON->order.chargeSegStatus_shoulder[i].dPower);
                }
            }
            printf_safe("�ȵ�ۣ�      %.4lf\n", pechProto->info.dPowerFee_off_peak);
            printf_safe("�ȷ���ѵ���  %.4lf\n", pechProto->info.dServFee_off_peak);
            printf_safe("�ȵ���        %.2lf\n", pCON->order.dTotalPower_off_peak);
            printf_safe("�ȳ����    %.2lf\n", pCON->order.dTotalPowerFee_off_peak);
            printf_safe("�ȷ���ѽ��  %.2lf\n", pCON->order.dTotalServFee_off_peak);
            printf_safe("�ȳ��ʱ��    %d\n", pCON->order.ulTotalTime_off_peak);
            printf_safe("����ϸ���ݣ�\n");
            for(i = 0; i < 5; i++)
            {
                if(pCON->order.chargeSegStatus_off_peak[i].tStartTime > 0)
                {
                    printf_safe("\tʱ�� %d\n", i+1);

                    ts = localtime (& pCON->order.chargeSegStatus_off_peak[i].tStartTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t��ʼʱ�䣺%s\n", buf);

                    ts = localtime (& pCON->order.chargeSegStatus_off_peak[i].tEndTime);
                    strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
                    printf_safe("\t����ʱ�䣺%s\n", buf);

                    printf_safe("\t��ʼ������%.2lf\n", pCON->order.chargeSegStatus_off_peak[i].dStartPower);
                    printf_safe("\tʱ�ε�����%.2lf\n", pCON->order.chargeSegStatus_off_peak[i].dPower);
                }
            }
            printf_safe("========ֹͣʱ����=========\n");
            if(pCON->order.ucPayType == defOrderPayType_Online)
            {
                printf_safe("֧����ʽ:\t����֧��\n");
            }
            else
            {
                printf_safe("֧����ʽ:\t����֧��\n");
            }
            printf_safe("ֹͣ����:\t");
            switch(pCON->order.ucStopType)
            {
            case defOrderStopType_RFID:
                printf_safe("RFID\n");
                break;
            case defOrderStopType_Remote:
                printf_safe("remote\n");
                break;
            case defOrderStopType_Full:
                printf_safe("����ֹͣ\n");
                break;
            case defOrderStopType_Fee:
                printf_safe("�ﵽ�����\n");//�ﵽ�����
                break;
            case defOrderStopType_Scram:
            case defOrderStopType_NetLost:
            case defOrderStopType_Poweroff:
            case defOrderStopType_OverCurr:
            case defOrderStopType_Knock:
                printf_safe("�쳣ֹͣ\n");//�쳣ֹͣ
                break;
            default:
                printf_safe("����ԭ��ֹͣ\n");;//����ԭ��ֹͣ
                break;
            }
            ts = localtime (& pCON->order.tStopTime);
            strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", ts);
            printf_safe("ֹͣʱ��:\t%s \n", buf);
            OrderDBInsertItem(&(pCON->order));
            OrderInit(&(pCON->order));
        }
    }
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

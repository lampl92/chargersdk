/**
* @file interface_remote.c
* @brief ��ƽ̨�ӿ�
* @author rgw
* @version v1.0
* @date 2017-02-06
*/

#include <time.h>
#include "includes.h"
#include "bsp.h"
#include "interface.h"
#include "mbedtls/base64.h"
#include "utils.h"
#include "stringName.h"
#include "cfg_parse.h"
#include "libEcharge/ech_globals.h"

#if 0
ErrorCode_t RemoteInit()
{
    uint8_t  ucAESKey_BASE64[] = "oFqTg0a0VrjiVU76M1WRVw==";
    uint8_t *pucAESKey_BASE64;

    pucAESKey_BASE64 = ucAESKey_BASE64;

    uint8_t ucAESKey[64];
    uint8_t ucAESKeyLen;
    uint8_t olen;
    int i;
    pucAESKey_BASE64 = ucAESKey_BASE64;
    mbedtls_base64_decode(ucAESKey, sizeof(ucAESKey), &olen, pucAESKey_BASE64, strlen(pucAESKey_BASE64));
    printf_safe("AESKEYBASE64 : %s\n", pucAESKey_BASE64);
    printf_safe("AESKey �� ");
    for(i = 0; i < olen; i++)
    {
        printf_safe("%02x ", ucAESKey[i]);
    }
    printf_safe("\n");
}

ErrorCode_t RemoteGetTime(struct tm *pTimeBlock)
{
    struct tm tmpTimeBlock;
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /** @todo (rgw#1#): ��ȡ������ʱ�� */


    *pTimeBlock = tmpTimeBlock;

    return errcode;
}
#endif

/** @brief
 *
 * @param pucRetVal uint8_t*     1ע��ɹ�  0ע��ʧ��
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteIF_SendRegist(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /** @todo (rgw#1#): ����ƽ̨ע��ӿ� */
    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_REGISTER, 10, 3);
    /**********/

    return errcode;
}

ErrorCode_t RemoteRecvHandle(echProtocol_t *pProto, uint16_t usSendID, uint8_t *pbuff, uint32_t *pLen)
{
    ErrorCode_t errcode;
    echCMD_t *pCMD;
    echCmdElem_t * pechCmdElem;
    gdsl_list_cursor_t cur;

    errcode = ERR_REMOTE_NODATA;
    pCMD = pProto->pCMD[usSendID];
    if(xSemaphoreTake(pCMD->xMutexCmd, 1000) == pdTRUE)
    {
        cur = gdsl_list_cursor_alloc (pCMD->plRecvCmd);
        gdsl_list_cursor_move_to_head (cur);
        while(pechCmdElem = gdsl_list_cursor_get_content (cur))
        {
            memmove(pbuff, pCMD->ucRecvdOptData, pCMD->ulRecvdOptLen);
            *pLen = pCMD->ulRecvdOptLen;
            errcode = ERR_NO;
            break;
        }
        gdsl_list_cursor_move_to_head (cur);
        while(pechCmdElem = gdsl_list_cursor_get_content (cur))
        {
            gdsl_list_cursor_delete(cur);
        }
        gdsl_list_cursor_free(cur);
        xSemaphoreGive(pCMD->xMutexCmd);
        if(errcode != ERR_REMOTE_NODATA)
        {
            xEventGroupSetBits(pCMD->xHandleEventCmd, defEventBitProtoCmdHandled);
        }
    }

    return errcode;
}

ErrorCode_t RemoteIF_RecvRegist(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_REGISTER, pbuff, &len);

    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        switch(pbuff[0])//��½���
        {
        case 1: //����
        case 3:
            errcode = ERR_NO;
            *psiRetVal = 1;
            break;
        case 2: //�豸�����ڣ��ر�����
        case 4: //��ԿʧЧ���ر�����
        case 5: //�������󣬹ر�����
        default:
            errcode = ERR_REMOTE_REGEDIT;
            *psiRetVal = 0;
            break;
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }
    return errcode;
}

ErrorCode_t RemoteIF_SendHeart(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_HEARTBEAT, 20, 3);

    return errcode;
}

ErrorCode_t RemoteIF_RecvHeart(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_HEARTBEAT, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        break;
    default:
        *psiRetVal = 0;
        break;
    }
    errcode = handle_errcode;
    return errcode;
}

/** @brief ϵͳ�����󣬷����������
 *
 * @param pEVSE EVSE_t*
 * @param pProto echProtocol_t*
 * @param succ uint32_t �ɹ���־ 1 �ɹ���2 ʧ��
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteIF_SendReset(EVSE_t *pEVSE, echProtocol_t *pProto, uint32_t succ)
{
    uint8_t *pbuff;
    uint32_t ulOptSN;
    ErrorCode_t errcode;
    errcode = ERR_NO;
    ul2uc ultmpNetSeq;

    pbuff = pProto->pCMD[ECH_CMDID_RESET]->ucRecvdOptData;
    //pbuff[0...3] �������к�
    ultmpNetSeq.ulVal = htonl(pProto->info.ulOptSN);
    pbuff[0] = ultmpNetSeq.ucVal[0];
    pbuff[1] = ultmpNetSeq.ucVal[1];
    pbuff[2] = ultmpNetSeq.ucVal[2];
    pbuff[3] = ultmpNetSeq.ucVal[3];
    //�ɹ���־
    pbuff[4] = succ;
    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_RESET, 0xffff, 0);

    ulOptSN = 0;
    pProto->info.SetProtoCfg(jnProtoOptSN, ParamTypeU32, NULL, 0, &ulOptSN);

    return errcode;
}
ErrorCode_t RemoteIF_RecvReset(echProtocol_t *pProto, uint32_t *pulOptSN, int *psiRetVal)
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode;
    ul2uc ultmpNetSeq;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_RESET, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        //pbuff[0...3] �������к�
        ultmpNetSeq.ucVal[0] = pbuff[0];
        ultmpNetSeq.ucVal[1] = pbuff[1];
        ultmpNetSeq.ucVal[2] = pbuff[2];
        ultmpNetSeq.ucVal[3] = pbuff[3];
        *pulOptSN = ntohl(ultmpNetSeq.ulVal);
        break;
    default:
        *psiRetVal = 0;
        break;
    }
    errcode = handle_errcode;
    return errcode;
}
ErrorCode_t RemoteIF_SendStatus(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_STATUS, 0xffff, 0);

    return errcode;
}
ErrorCode_t RemoteIF_RecvStatus(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    CON_t *pCON;
    uint8_t id;
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode = ERR_NO;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_STATUS, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        //pbuff[0...3] �������к�
        //pbuff[4] ���׮�ӿ�
        id = EchRemoteIDtoCONID(pbuff[4]);
        pCON = CONGetHandle(id);
        if(pCON != NULL)
        {
            RemoteIF_SendStatus(pEVSE, pProto, pCON);
            *psiRetVal = 1;
        }
        else
        {
            *psiRetVal = 0;
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

/** @brief ׮�ظ�ƽ̨��ͣ����
 *
 * @param pEVSE EVSE_t*
 * @param pProto echProtocol_t*
 * @param pCON CON_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteIF_SendRemoteCtrl(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t succ, uint8_t reason)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    /*** ���²���Ϊ�ƻ��˳���ṹ����pCMD�еĻ���ռ����һЩ��Ҫ���ݵĲ���*/
    /*** 2017��6��16�գ�����pCMD�Ļ���ר���ڽ����봫�ݲ���������ṹ�ϲ��õ�����*/
    pbuff = pProto->pCMD[ECH_CMDID_REMOTE_CTRL]->ucRecvdOptData;
    if(succ == 1)
    {
        pbuff[14] = 1;
    }
    else if(succ == 0)
    {
        pbuff[14] = 2;
    }
    pbuff[15] = reason;
    /*********************/
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_REMOTE_CTRL, 30, 3);

    return errcode;
}

/** @brief ƽ̨�·�����ͣ����
 *
 * @param pEVSE EVSE_t*
 * @param pProto echProtocol_t*
 * @param pid uint8_t* ����ID������
 * @param pctrl uint8_t* ���ز���������
 * @param psiRetVal int* ���ؽ��������
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteIF_RecvRemoteCtrl(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t *pid, uint8_t *pctrl, int *psiRetVal )// ��_��
{
    CON_t *pCON;
    uint8_t id;

    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode;

    uint8_t strOrderSN_tmp[17];
    double dLimetFee;
    ul2uc ulTmp;

    id = 0;
    errcode = ERR_NO;
    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_REMOTE_CTRL, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        //pbuff[0...3] ����ID ������������ucRecvdOptData�д��ظ�ʱʹ��

        //pbuff[12] ���׮�ӿ�
        id = EchRemoteIDtoCONID(pbuff[12]);
        pCON = CONGetHandle(id);
        if(pCON != NULL)
        {
            //pbuff[13] ���� 1������2ֹͣ
            if(pbuff[13] == 1)
            {
                //pbuff[4...11] ������ˮ��
                HexToStr(&pbuff[4], pCON->order.strOrderSN, 8);

                *pctrl = pbuff[13];

                //pbuff[14...17] �����
                ulTmp.ucVal[0] = pbuff[14];
                ulTmp.ucVal[1] = pbuff[15];
                ulTmp.ucVal[2] = pbuff[16];
                ulTmp.ucVal[3] = pbuff[17];
                dLimetFee = (double)(ntohl(ulTmp.ulVal)) * 0.01;
                pCON->order.dLimitFee = dLimetFee;
                pCON->order.ucStartType = 5;//Remote�޿�

                xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
            }
            else if(pbuff[13] == 2)
            {
                /**�������жϽ��׺��Ƿ���� */
                HexToStr(&pbuff[4], strOrderSN_tmp, 8);
                if(strcmp(strOrderSN_tmp, pCON->order.strOrderSN) == 0)
                {
                    *pctrl = pbuff[13];
                    xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionRemoteStop);
                }
                else //�����Ų����
                {
                    *psiRetVal = 0;
                    return ERR_REMOTE_ORDERSN;
                }
            }
            *psiRetVal = 1;
            *pid = id;
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

/** @brief
 *
 * @param pEVSE EVSE_t*
 * @param pProto echProtocol_t*
 * @param pCON CON_t*
 * @param ctrl uint8_t     ���׮״̬ 1 ������2 ͣ��
 * @param reason uint8_t   ֹͣԭ��
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteIF_SendRTData(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t ctrl, uint8_t reason)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_RTDATA]->ucRecvdOptData;

    pbuff[39] = ctrl;
    pbuff[40] = reason;
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_RTDATA, 0xffff, 0);

    return errcode;
}
ErrorCode_t RemoteIF_SendOrder(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_ORDER]->ucRecvdOptData;

    pbuff[0] = pCON->order.ucStartType;//4 �п���5 �޿�
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_ORDER, 20, 3);

    return errcode;
}
ErrorCode_t RemoteIF_RecvOrder(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    CON_t *pCON;
    uint8_t id;

    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode;
    uint8_t strOrderSN_tmp[17];

    id = 0;
    memset(strOrderSN_tmp, 0, 17);
    errcode = ERR_NO;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_ORDER, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        //[0] ���޿�
        //[1...8] ������ˮ��
        //[9] ���׮�ӿ�
        id = EchRemoteIDtoCONID(pbuff[9]);
        pCON = CONGetHandle(id);
        if(pCON != NULL)
        {
            HexToStr(&pbuff[1], strOrderSN_tmp, 8);
            if(strcmp(strOrderSN_tmp, pCON->order.strOrderSN) == 0)
            {
                *psiRetVal = 1;
            }
            else //�����Ų����
            {
                *psiRetVal = 0;
                errcode = ERR_REMOTE_ORDERSN;
            }
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_SendSetPowerFee(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_HEARTBEAT, 20, 3);

    return errcode;
}

ErrorCode_t RemoteIF_RecvSetPowerFee(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t flag_set, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t set_errcode_sharp;
    ErrorCode_t set_errcode_peak;
    ErrorCode_t set_errcode_shoulder;
    ErrorCode_t set_errcode_off_peak;
    ErrorCode_t errcode;
    ul2uc ultmpNetSeq;
    double dtmpPowerFee;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_POWERFEE, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        errcode = handle_errcode;
        break;
    case ERR_NO:
        if(flag_set == 0)
        {
            *psiRetVal = 1;
            errcode = ERR_NO;
            //pbuff[0...3] ����ID
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
            break;//ֱ�ӽ��� ����
        }
        *psiRetVal = 1;

        //pbuff[4...7] �����
        ultmpNetSeq.ucVal[0] = pbuff[4];
        ultmpNetSeq.ucVal[1] = pbuff[5];
        ultmpNetSeq.ucVal[2] = pbuff[6];
        ultmpNetSeq.ucVal[3] = pbuff[7];
        dtmpPowerFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_sharp = pProto->info.SetProtoCfg(jnProtoPowerFee_sharp, ParamTypeDouble, NULL, 0, &dtmpPowerFee);
        //pbuff[8...11] �����
        ultmpNetSeq.ucVal[0] = pbuff[8];
        ultmpNetSeq.ucVal[1] = pbuff[9];
        ultmpNetSeq.ucVal[2] = pbuff[10];
        ultmpNetSeq.ucVal[3] = pbuff[11];
        dtmpPowerFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_peak = pProto->info.SetProtoCfg(jnProtoPowerFee_peak, ParamTypeDouble, NULL, 0, &dtmpPowerFee);
        //pbuff[12...15] ƽ����
        ultmpNetSeq.ucVal[0] = pbuff[12];
        ultmpNetSeq.ucVal[1] = pbuff[13];
        ultmpNetSeq.ucVal[2] = pbuff[14];
        ultmpNetSeq.ucVal[3] = pbuff[15];
        dtmpPowerFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_shoulder = pProto->info.SetProtoCfg(jnProtoPowerFee_shoulder, ParamTypeDouble, NULL, 0, &dtmpPowerFee);
        //pbuff[16...19] �ȷ���
        ultmpNetSeq.ucVal[0] = pbuff[16];
        ultmpNetSeq.ucVal[1] = pbuff[17];
        ultmpNetSeq.ucVal[2] = pbuff[18];
        ultmpNetSeq.ucVal[3] = pbuff[19];
        dtmpPowerFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_off_peak = pProto->info.SetProtoCfg(jnProtoPowerFee_off_peak, ParamTypeDouble, NULL, 0, &dtmpPowerFee);

        //pbuff[0...3] ����ID
        if(set_errcode_sharp == ERR_NO &&
           set_errcode_peak == ERR_NO &&
           set_errcode_shoulder == ERR_NO &&
           set_errcode_off_peak == ERR_NO)
        {
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_NO;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0xffff, 0);
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_FILE_RW;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}
ErrorCode_t RemoteIF_RecvSetServFee(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t flag_set, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t set_errcode_sharp;
    ErrorCode_t set_errcode_peak;
    ErrorCode_t set_errcode_shoulder;
    ErrorCode_t set_errcode_off_peak;
    ErrorCode_t errcode;
    ul2uc ultmpNetSeq;
    double dtmpServFee;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_SERVFEE, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        errcode = handle_errcode;
        break;
    case ERR_NO:
        if(flag_set == 0)
        {
            *psiRetVal = 1;
            errcode = ERR_NO;
            //pbuff[0...3] ����ID
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
            break;//ֱ�ӽ��� ����
        }
        *psiRetVal = 1;
        //pbuff[0...3] ����ID
        //pbuff[4...7] ������
        ultmpNetSeq.ucVal[0] = pbuff[4];
        ultmpNetSeq.ucVal[1] = pbuff[5];
        ultmpNetSeq.ucVal[2] = pbuff[6];
        ultmpNetSeq.ucVal[3] = pbuff[7];
        dtmpServFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_sharp = pProto->info.SetProtoCfg(jnProtoServFee_sharp, ParamTypeDouble, NULL, 0, &dtmpServFee);
        //pbuff[8...11] ������
        ultmpNetSeq.ucVal[0] = pbuff[8];
        ultmpNetSeq.ucVal[1] = pbuff[9];
        ultmpNetSeq.ucVal[2] = pbuff[10];
        ultmpNetSeq.ucVal[3] = pbuff[11];
        dtmpServFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_peak = pProto->info.SetProtoCfg(jnProtoServFee_peak, ParamTypeDouble, NULL, 0, &dtmpServFee);
        //pbuff[12...15] ƽ�����
        ultmpNetSeq.ucVal[0] = pbuff[12];
        ultmpNetSeq.ucVal[1] = pbuff[13];
        ultmpNetSeq.ucVal[2] = pbuff[14];
        ultmpNetSeq.ucVal[3] = pbuff[15];
        dtmpServFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_shoulder = pProto->info.SetProtoCfg(jnProtoServFee_shoulder, ParamTypeDouble, NULL, 0, &dtmpServFee);
        //pbuff[16...19] �ȷ����
        ultmpNetSeq.ucVal[0] = pbuff[16];
        ultmpNetSeq.ucVal[1] = pbuff[17];
        ultmpNetSeq.ucVal[2] = pbuff[18];
        ultmpNetSeq.ucVal[3] = pbuff[19];
        dtmpServFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_off_peak = pProto->info.SetProtoCfg(jnProtoServFee_off_peak, ParamTypeDouble, NULL, 0, &dtmpServFee);

        //pbuff[0...3] ����ID
        if(set_errcode_sharp == ERR_NO &&
           set_errcode_peak == ERR_NO &&
           set_errcode_shoulder == ERR_NO &&
           set_errcode_off_peak == ERR_NO)
        {
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_NO;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0xffff, 0);
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_FILE_RW;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}
ErrorCode_t RemoteIF_RecvSetCyc(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t set_errcode_stat;
    ErrorCode_t set_errcode_rt;
    ErrorCode_t errcode;
    uint32_t ultmpTimCyc_ms;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_CYC, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        errcode = handle_errcode;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        //pbuff[0...3] ����ID
        //pbuff[4] ״̬�����ϱ����
        ultmpTimCyc_ms = pbuff[4] * 1000;
        set_errcode_stat = pProto->info.SetProtoCfg(jnProtoStatusCyc_ms, ParamTypeU32, NULL, 0, &ultmpTimCyc_ms);
        //pbuff[5] ʵʱ�����ϱ����
        ultmpTimCyc_ms = pbuff[5] * 1000;
        set_errcode_rt = pProto->info.SetProtoCfg(jnProtoRTDataCyc_ms, ParamTypeU32, NULL, 0, &ultmpTimCyc_ms);

        //pbuff[0...3] ����ID
        if(set_errcode_stat == ERR_NO &&
           set_errcode_rt == ERR_NO)
        {
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_NO;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0xffff, 0);
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_FILE_RW;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}
ErrorCode_t RemoteIF_RecvSetTimeSeg(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t flag_set, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t set_errcode_sharp;
    ErrorCode_t set_errcode_peak;
    ErrorCode_t set_errcode_shoulder;
    ErrorCode_t set_errcode_off_peak;
    ErrorCode_t errcode;
    EchSegTime_t tmpSegTime;
    uint8_t ucOffset;
    int i;

    ucOffset = 0;
    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_TIMESEG, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        errcode = handle_errcode;
        break;
    case ERR_NO:
        if(flag_set == 0)
        {
            *psiRetVal = 1;
            errcode = ERR_NO;
            //pbuff[0...3] ����ID
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
            break;//ֱ�ӽ��� ����
        }
        *psiRetVal = 1;
        //pbuff[0...3] ����ID
        //pbuff[4] ��ʱ��θ���
        ucOffset = 4;
        //��
        tmpSegTime.ucSegCont = pbuff[ucOffset++];
        set_errcode_sharp = pProto->info.SetProtoCfg(jnProtoSegTime_sharp, ParamTypeObj, jnProtoSegCont, ParamTypeU8, &(tmpSegTime.ucSegCont));
        //pbuff[5...SegCont]
        for(i = 0; i < tmpSegTime.ucSegCont; i++)
        {
            tmpSegTime.ucStart[i] = pbuff[ucOffset++];
            tmpSegTime.ucEnd[i] = pbuff[ucOffset++];
            pProto->info.SetProtoCfg(jnProtoSegTime_sharp, ParamTypeObj, jnProtoSegStart[i], ParamTypeU8, &(tmpSegTime.ucStart[i]));
            pProto->info.SetProtoCfg(jnProtoSegTime_sharp, ParamTypeObj, jnProtoSegEnd[i],   ParamTypeU8, &(tmpSegTime.ucEnd[i]));
        }
        //��
        tmpSegTime.ucSegCont = pbuff[ucOffset++];
        set_errcode_peak = pProto->info.SetProtoCfg(jnProtoSegTime_peak, ParamTypeObj, jnProtoSegCont, ParamTypeU8, &(tmpSegTime.ucSegCont));
        for(i = 0; i < tmpSegTime.ucSegCont; i++)
        {
            tmpSegTime.ucStart[i] = pbuff[ucOffset++];
            tmpSegTime.ucEnd[i] = pbuff[ucOffset++];
            pProto->info.SetProtoCfg(jnProtoSegTime_peak, ParamTypeObj, jnProtoSegStart[i], ParamTypeU8, &(tmpSegTime.ucStart[i]));
            pProto->info.SetProtoCfg(jnProtoSegTime_peak, ParamTypeObj, jnProtoSegEnd[i],   ParamTypeU8, &(tmpSegTime.ucEnd[i]));
        }
        //ƽ
        tmpSegTime.ucSegCont = pbuff[ucOffset++];
        set_errcode_shoulder = pProto->info.SetProtoCfg(jnProtoSegTime_shoulder, ParamTypeObj, jnProtoSegCont, ParamTypeU8, &(tmpSegTime.ucSegCont));
        for(i = 0; i < tmpSegTime.ucSegCont; i++)
        {
            tmpSegTime.ucStart[i] = pbuff[ucOffset++];
            tmpSegTime.ucEnd[i] = pbuff[ucOffset++];
            pProto->info.SetProtoCfg(jnProtoSegTime_shoulder, ParamTypeObj, jnProtoSegStart[i], ParamTypeU8, &(tmpSegTime.ucStart[i]));
            pProto->info.SetProtoCfg(jnProtoSegTime_shoulder, ParamTypeObj, jnProtoSegEnd[i],   ParamTypeU8, &(tmpSegTime.ucEnd[i]));
        }
        //��
        tmpSegTime.ucSegCont = pbuff[ucOffset++];
        set_errcode_off_peak = pProto->info.SetProtoCfg(jnProtoSegTime_off_peak, ParamTypeObj, jnProtoSegCont, ParamTypeU8, &(tmpSegTime.ucSegCont));
        for(i = 0; i < tmpSegTime.ucSegCont; i++)
        {
            tmpSegTime.ucStart[i] = pbuff[ucOffset++];
            tmpSegTime.ucEnd[i] = pbuff[ucOffset++];
            pProto->info.SetProtoCfg(jnProtoSegTime_off_peak, ParamTypeObj, jnProtoSegStart[i], ParamTypeU8, &(tmpSegTime.ucStart[i]));
            pProto->info.SetProtoCfg(jnProtoSegTime_off_peak, ParamTypeObj, jnProtoSegEnd[i],   ParamTypeU8, &(tmpSegTime.ucEnd[i]));
        }

        //pbuff[0...3] ����ID
        if(set_errcode_sharp == ERR_NO &&
           set_errcode_peak == ERR_NO &&
           set_errcode_shoulder == ERR_NO &&
           set_errcode_off_peak == ERR_NO)
        {
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_NO;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0xffff, 0);
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_FILE_RW;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}
ErrorCode_t RemoteIF_RecvSetKey(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t set_errcode_key;
    ErrorCode_t set_errcode_time;
    ErrorCode_t errcode;
    uint8_t strTmpKey[16+1] = {0};
    uint32_t ultmpTim;
    ul2uc ultmpNetSeq;
    int i;
    uint8_t ucOffset;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_KEY, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        errcode = handle_errcode;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        //pbuff[0...3] ����ID
        //pbuff[4...19] ��Կ
        ucOffset = 4;
        for(i = 0; i < 16; i++)
        {
            strTmpKey[i] = pbuff[ucOffset++];
        }
        set_errcode_key = pProto->info.SetProtoCfg(jnProtoNewKey, ParamTypeString, NULL, 0, strTmpKey);
        //pbuff[20...23] ��Կ���ʱ��
        ultmpNetSeq.ucVal[0] = pbuff[ucOffset++];
        ultmpNetSeq.ucVal[1] = pbuff[ucOffset++];
        ultmpNetSeq.ucVal[2] = pbuff[ucOffset++];
        ultmpNetSeq.ucVal[3] = pbuff[ucOffset++];
        ultmpTim = ntohl(ultmpNetSeq.ulVal);
        set_errcode_time = pProto->info.SetProtoCfg(jnProtoNewKeyChangeTime, ParamTypeU32, NULL, 0, &ultmpTim);

        if(set_errcode_key == ERR_NO &&
           set_errcode_time == ERR_NO)
        {
            errcode = ERR_NO;
        }
        else
        {
            errcode = ERR_FILE_RW;
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_RecvSetQR(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t flag_set, int *psiRetVal )
{
    CON_t *pCON = NULL;
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t set_errcode;
    ErrorCode_t errcode;
    uint8_t id_cont;
    uint8_t id;//����ǹID,��0��ʼ
    uint8_t len_qr;
    uint8_t qrcode[64] = {0};
    uint8_t ucOffset;
    int i,j;

    set_errcode = ERR_NO;
    ucOffset = 0;
    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_QR, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        errcode = handle_errcode;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        if(flag_set == 0)
        {
            *psiRetVal = 1;
            errcode = ERR_NO;
            //pbuff[0...3] ����ID
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
            break;//ֱ�ӽ��� ����
        }
        //pbuff[4] ǹ�ڸ���
        ucOffset = 4;
        id_cont = pbuff[ucOffset++];

        for(i = 0; i < id_cont; i++)
        {
            //pbuff[5] ���׮�ӿ�
            id = EchRemoteIDtoCONID(pbuff[ucOffset++]);
            pCON = CONGetHandle(id);
            if(pCON == NULL)
            {
                set_errcode = ERR_REMOTE_PARAM;
                break; //���޴�ǹ, �˳�ѭ��
            }
            //pbuff[6] ��ά�볤��
            len_qr = pbuff[ucOffset++];
            //pbuff[7...] ��ά��
            for(j = 0; j < len_qr; j++)
            {
                qrcode[j] = pbuff[ucOffset++];
            }
            qrcode[len_qr] = '\0';
            pCON->info.SetCONCfg(pCON, jnQRCode, qrcode, ParamTypeString);
        }

        //pbuff[0...3] ����ID
        if(set_errcode == ERR_NO)
        {
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_NO;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0xffff, 0);
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_REMOTE_PARAM;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_RecvSetBnWList(uint16_t usCmdID, EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    int set_errcode;
    ErrorCode_t errcode;
    us2uc ustmpNetSeq;
    uint16_t usListCont;
    uint8_t i,j;
    uint8_t ucOffset = 0;
    uint8_t strID[16+1] = {0};
    uint8_t path[64];

    if(usCmdID == ECH_CMDID_SET_BLACK)
    {
        strcpy(path, pathBlackList);
    }
    else if(usCmdID == ECH_CMDID_SET_WHITE)
    {
        strcpy(path, pathWhiteList);
    }

    handle_errcode = RemoteRecvHandle(pProto, usCmdID, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        errcode = handle_errcode;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        pProto->info.BnWFlushListCfg(path);//���úڰ�������Ҫ���ԭ������
        //pbuff[0...3] ����ID
        //pbuff[4,5] ��������
        ucOffset = 4;
        ustmpNetSeq.ucVal[0] = pbuff[ucOffset++];
        ustmpNetSeq.ucVal[1] = pbuff[ucOffset++];
        usListCont = ntohs(ustmpNetSeq.usVal);
        //����,16λ
        for (i = 0; i < usListCont; i++)
        {
            for(j = 0; j < 16; j++)
            {
                strID[j] = pbuff[ucOffset++];
            }
            set_errcode = pProto->info.BnWAddListCfg(path, strID);
            if(set_errcode == 0)
            {
                break;
            }
        }

        if(set_errcode == 1)
        {
            errcode = ERR_NO;
            //[0...3]����ID
            //[4] ���ý��
            pProto->pCMD[usCmdID]->ucRecvdOptData[4] = 1;
            pProto->sendCommand(pProto, pEVSE, NULL, usCmdID, 0xffff, 0);
        }
        else
        {
            errcode = ERR_FILE_RW;
            //[0...3]����ID
            //[4] ���ý��
            pProto->pCMD[usCmdID]->ucRecvdOptData[4] = 2;
            pProto->sendCommand(pProto, pEVSE, NULL, usCmdID, 0xffff, 0);
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}
ErrorCode_t RemoteIF_RecvAddDelBnWList(uint16_t usCmdID, EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    int set_errcode;
    ErrorCode_t errcode;
    us2uc ustmpNetSeq;
    uint16_t usListCont;
    uint8_t i,j,k;          //i:������  j:�����ֽڳ���  k:��������
    uint8_t ucOffset = 0;
    uint8_t strID[16+1] = {0};
    uint8_t path[64];

    handle_errcode = RemoteRecvHandle(pProto, usCmdID, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        errcode = handle_errcode;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        //pbuff[0...3] ����ID

        ucOffset = 4;
        for(k = 0; k < 2; k++) //�ȴ�������� 0, �ٴ�������� 1
        {
            if(k == 1)
            {
                strcpy(path, pathBlackList);
            }
            else if(k == 0)
            {
                strcpy(path, pathWhiteList);
            }
            //pbuff[4,5] ��������
            ustmpNetSeq.ucVal[0] = pbuff[ucOffset++];
            ustmpNetSeq.ucVal[1] = pbuff[ucOffset++];
            usListCont = ntohs(ustmpNetSeq.usVal);
            //����,16λ
            for (i = 0; i < usListCont; i++)
            {
                for(j = 0; j < 16; j++)
                {
                    strID[j] = pbuff[ucOffset++];
                }
                switch(usCmdID)
                {
                case ECH_CMDID_ADD_BNW:
                    set_errcode = pProto->info.BnWAddListCfg(path, strID);
                    break;
                case ECH_CMDID_DEL_BNW:
                    set_errcode = pProto->info.BnWDeleteListCfg(path, strID);
                    break;
                }

                if(set_errcode == 0)
                {
                    break;
                }
            }
        }


        if(set_errcode == 1)
        {
            //pbuff[0...3] ����ID
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_NO;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0xffff, 0);
        }
        else
        {
            //pbuff[0...3] ����ID
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_REMOTE_PARAM;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}
static ErrorCode_t RemoteIF_SendReqCmdid(uint16_t usCmdID, EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pProto->sendCommand(pProto, pEVSE, NULL, usCmdID, 0xffff, 0);

    *psiRetVal = 1;

    return errcode;
}
static ErrorCode_t RemoteIF_RecvReqCmdid(uint16_t usCmdID, EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode;

    handle_errcode = RemoteRecvHandle(pProto, usCmdID, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        errcode = handle_errcode;
        break;
    case ERR_NO:
        RemoteIF_SendReqCmdid(usCmdID, pEVSE, pProto, psiRetVal);
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}
ErrorCode_t RemoteIF_RecvReq(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal)
{
    int res;

    RemoteIF_RecvReqCmdid(ECH_CMDID_REQ_POWERFEE, pEVSE, pProto, &res);
    RemoteIF_RecvReqCmdid(ECH_CMDID_REQ_SERVFEE,  pEVSE, pProto, &res);
    RemoteIF_RecvReqCmdid(ECH_CMDID_REQ_CYC,      pEVSE, pProto, &res);
    RemoteIF_RecvReqCmdid(ECH_CMDID_REQ_TIMESEG,  pEVSE, pProto, &res);
    RemoteIF_RecvReqCmdid(ECH_CMDID_REQ_KEY,      pEVSE, pProto, &res);
    RemoteIF_RecvReqCmdid(ECH_CMDID_REQ_SOFTVER,  pEVSE, pProto, &res);
    RemoteIF_RecvReqCmdid(ECH_CMDID_REQ_QR,       pEVSE, pProto, &res);
    RemoteIF_RecvReqCmdid(ECH_CMDID_REQ_BLACK,    pEVSE, pProto, &res);
    RemoteIF_RecvReqCmdid(ECH_CMDID_REQ_WHITE,    pEVSE, pProto, &res);

    RemoteIF_RecvSetBnWList(ECH_CMDID_SET_BLACK, pEVSE, pProto, &res);
    RemoteIF_RecvSetBnWList(ECH_CMDID_SET_WHITE, pEVSE, pProto, &res);

    RemoteIF_RecvAddDelBnWList(ECH_CMDID_ADD_BNW, pEVSE, pProto, &res);
    RemoteIF_RecvAddDelBnWList(ECH_CMDID_DEL_BNW, pEVSE, pProto, &res);
}

/** @brief
 *
 * @param pucID uint8_t*
 * @param ucIDLength uint8_t
 * @param pucAccountStatus uint8_t*     0 δע�� ��1 ע��
 * @param pdBalance double*             �ʻ����
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteGetBalance(uint8_t *pucID, uint8_t ucIDLength, uint8_t *pucAccountStatus, double *pdBalance)
{
    double tmpBalance;
    uint8_t tmpAccountStat;
    ErrorCode_t errcode;

    tmpAccountStat = 1;
    tmpBalance = 888.88;
    errcode = ERR_NO;

    /** @todo (rgw#1#): ������ƽ̨�ӿ� */

    *pucAccountStatus = tmpAccountStat;
    *pdBalance = tmpBalance;

//    errcode = ERR_NET_TIMEOUT;
//    errcode = ERR_NONET;
    return errcode;
}

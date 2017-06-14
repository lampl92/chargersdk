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

uint8_t EchRemoteIDtoCONID(uint8_t remote_id)
{
    uint8_t id;
    switch(remote_id)
    {
    case 0:
    case 1:
        id = 0;
        break;
    case 2:
        id = 1;
        break;
    default:
        id = 0xff;
        break;
    }
    return id;
}


/** @brief
 *
 * @param pucRetVal uint8_t*     1ע��ɹ�  0ע��ʧ��
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteRegist(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /** @todo (rgw#1#): ����ƽ̨ע��ӿ� */
    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_REGISTER, 10, 3);
    /**********/

    return errcode;
}

ErrorCode_t RemoteRegistRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    ErrorCode_t errcode;
    echCMD_t *pCMD;
    echCmdElem_t * pechCmdElem;
    gdsl_list_cursor_t cur;
    uint8_t *pbuff; //���ݲ���
    uint32_t len;

    *psiRetVal = 0;
    pCMD = pProto->pCMD[ECH_CMDID_REGISTER];
    cur = gdsl_list_cursor_alloc (pCMD->plRecvCmd);
    gdsl_list_cursor_move_to_head (cur);
    while(pechCmdElem = gdsl_list_cursor_get_content (cur))
    {
        pbuff = pCMD->ucRecvdOptData;
        len = pCMD->ulRecvdOptLen;
        printf_safe("len = %d\n", len);
        errcode = ERR_NO;
        *psiRetVal = 0;
        switch(pbuff[0])//��½���
        {
        case 1: //����
        case 3:
            *psiRetVal = 1;
            break;
        case 2: //�豸�����ڣ��ر�����
        case 4: //��ԿʧЧ���ر�����
        case 5: //�������󣬹ر�����
        default:
            *psiRetVal = 0;
            break;
        }
        pechCmdElem->status = 1; //�������
        gdsl_list_cursor_step_forward (cur);
    }

    return errcode;
}

ErrorCode_t RemoteHeart(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_HEARTBEAT, 10, 3);

    return errcode;
}

ErrorCode_t RemoteHeartRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    ErrorCode_t errcode;
    echCMD_t *pCMD;
    echCmdElem_t * pechCmdElem;
    gdsl_list_cursor_t cur;
    errcode = ERR_NO;

    pCMD = pProto->pCMD[ECH_CMDID_HEARTBEAT];
    cur = gdsl_list_cursor_alloc (pCMD->plRecvCmd);
    *psiRetVal = 0;

    gdsl_list_cursor_move_to_head (cur);
    while(pechCmdElem = gdsl_list_cursor_get_content (cur))
    {
        *psiRetVal = 1;
        pechCmdElem->status = 1; //�������
        gdsl_list_cursor_step_forward (cur);
    }

    return errcode;
}
ErrorCode_t RemoteStatus(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_STATUS, 1, 1);

    return errcode;
}
ErrorCode_t RemoteStatusRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    CON_t *pCON;
    uint8_t *pbuff;
    uint32_t len;
    uint8_t id;
    ErrorCode_t errcode;
    *psiRetVal = 0;

    errcode = ERR_NO;
    id = 0;

    pbuff = pProto->pCMD[ECH_CMDID_STATUS]->ucRecvdOptData;
    len = pProto->pCMD[ECH_CMDID_STATUS]->ulRecvdOptLen;
    if(len > 0)
    {
        //pbuff[0...3] �������к�
        //pbuff[4] ���׮�ӿ�
        id = EchRemoteIDtoCONID(pbuff[4]);
        pCON = CONGetHandle(id);
        if(pCON != NULL)
        {
            RemoteStatus(pEVSE, pProto, pCON);
            *psiRetVal = 1;
        }
        else
        {
            *psiRetVal = 0;
        }
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
ErrorCode_t RemoteRemoteCtrl(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t succ, uint8_t reason)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    /*** ���²���Ϊ�ƻ��˳���ṹ����pCMD�еĻ���ռ����һЩ��Ҫ���ݵĲ���*/
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
ErrorCode_t RemoteRemoteCtrlRes(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t *pid, uint8_t *pctrl, int *psiRetVal )// ��_��
{
    CON_t *pCON;
    uint8_t *pbuff;
    uint32_t len;
    uint8_t strOrderSN_tmp[17];
    double dLimetFee;
    ul2uc ulTmp;
    uint8_t id;
    ErrorCode_t errcode;
    *psiRetVal = 0;

    id = 0;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_REMOTE_CTRL]->ucRecvdOptData;
    len = pProto->pCMD[ECH_CMDID_REMOTE_CTRL]->ulRecvdOptLen;
    if(len > 0)
    {
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
                pCON->order.ucStartType = 5;//�޿�

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
        else // pCON = NULL
        {
            *psiRetVal = 0;
        }
    }
    else//len = 0
    {
        *psiRetVal = 0;
    }

    return  errcode;
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
ErrorCode_t RemoteRTData(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t ctrl, uint8_t reason)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_RTDATA]->ucRecvdOptData;

    pbuff[39] = ctrl;
    pbuff[40] = reason;
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_RTDATA, 10, 1);

    return errcode;
}
ErrorCode_t RemoteOrder(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_ORDER]->ucRecvdOptData;

    pbuff[0] = pCON->order.ucStartType;//4 �п���5 �޿�
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_ORDER, 20, 3);

    return errcode;
}
ErrorCode_t RemoteOrderRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    CON_t *pCON;
    uint8_t *pbuff;
    uint32_t len;
    uint8_t id;
    uint8_t strOrderSN_tmp[17];
    ErrorCode_t errcode;
    *psiRetVal = 0;

    id = 0;
    memset(strOrderSN_tmp, 0, 17);
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_ORDER]->ucRecvdOptData;
    len = pProto->pCMD[ECH_CMDID_ORDER]->ulRecvdOptLen;
    if(len > 0)
    {
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
                return ERR_REMOTE_ORDERSN;
            }
        }
    }

    return errcode;
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

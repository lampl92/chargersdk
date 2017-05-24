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
ErrorCode_t RemoteRegist(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /** @todo (rgw#1#): ����ƽ̨ע��ӿ� */
    pProto->sendCommand(pProto, NULL, NULL, ECH_CMDID_REGISTER, 10, 3);
    /**********/

    return errcode;
}

ErrorCode_t RemoteRegistRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    ErrorCode_t errcode;
    uint8_t *pbuff; //���ݲ���
    uint32_t len;
    if(pProto->pCMD[ECH_CMDID_REGISTER]->uiRecvdOptLen > 0)
    {
        pbuff = pProto->pCMD[ECH_CMDID_REGISTER]->ucRecvdOptData;
        len = pProto->pCMD[ECH_CMDID_REGISTER]->uiRecvdOptLen;
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
    }
    return errcode;
}

ErrorCode_t RemoteHeart(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    /** @todo (rgw#1#): ����ƽ̨ע��ӿ� */
    pProto->sendCommand(pProto, NULL, NULL, ECH_CMDID_HEARTBEAT, 10, 3);
    /**********/

    return errcode;
}

ErrorCode_t RemoteHeartRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    ErrorCode_t errcode;
    uint8_t *pbuff; //���ݲ���
    uint32_t len;

    len = pProto->pCMD[ECH_CMDID_HEARTBEAT]->uiRecvdOptLen;
    if(len > 0)
    {
        *psiRetVal = 1;
    }

    errcode = ERR_NO;
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

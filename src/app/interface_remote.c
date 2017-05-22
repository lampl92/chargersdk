/**
* @file interface_remote.c
* @brief 云平台接口
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
    printf_safe("AESKey ： ");
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

    /** @todo (rgw#1#): 获取服务器时间 */


    *pTimeBlock = tmpTimeBlock;

    return errcode;
}
#endif
/** @brief
 *
 * @param pucRetVal uint8_t*     1注册成功  0注册失败
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteRegist(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /** @todo (rgw#1#): 调用平台注册接口 */
    pProto->sendCommand(pProto, ECH_CMDID_REGISTER, 10000, 3);
    /**********/

    return errcode;
}

ErrorCode_t RemoteRegistRes(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t *pucRetVal )
{
    ErrorCode_t errcode;
    uint8_t *pbuff; //数据部分
    uint32_t len;
    if(pProto->pCMD[ECH_CMDID_REGISTER]->uiRecvdOptLen > 0)
    {
        pbuff = pProto->pCMD[ECH_CMDID_REGISTER]->ucRecvdOptData;
        len = pProto->pCMD[ECH_CMDID_REGISTER]->uiRecvdOptLen;
        errcode = ERR_NO;
        *pucRetVal = 0;
        switch(pbuff[0])//登陆结果
        {
        case 1: //正常
        case 3:
            *pucRetVal = 1;
            break;
        case 2: //设备不存在，关闭连接
        case 4: //密钥失效，关闭连接
        case 5: //其他错误，关闭连接
        default:
            *pucRetVal = 0;
            break;
        }
    }
    return errcode;
}

ErrorCode_t RemoteHeart(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    /** @todo (rgw#1#): 调用平台注册接口 */
    pProto->sendCommand(pProto, ECH_CMDID_HEARTBEAT, 10000, 3);
    /**********/

    return errcode;
}

ErrorCode_t RemoteHeartRes(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t *pucRetVal )
{
    ErrorCode_t errcode;
    uint8_t *pbuff; //数据部分
    uint32_t len;

    len = pProto->pCMD[ECH_CMDID_HEARTBEAT]->uiRecvdOptLen;
    if(len > 0)
    {
        *pucRetVal = 1;
    }

    errcode = ERR_NO;
}
/** @brief
 *
 * @param pucID uint8_t*
 * @param ucIDLength uint8_t
 * @param pucAccountStatus uint8_t*     0 未注册 、1 注册
 * @param pdBalance double*             帐户余额
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

    /** @todo (rgw#1#): 调用云平台接口 */

    *pucAccountStatus = tmpAccountStat;
    *pdBalance = tmpBalance;

//    errcode = ERR_NET_TIMEOUT;
//    errcode = ERR_NONET;
    return errcode;
}

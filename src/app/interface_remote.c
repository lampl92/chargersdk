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
#include "utils.h"
#include "lwip/sockets.h"

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
    default:
        id = 0xff;
        break;
    }
    return id;
}


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
    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_REGISTER, 10, 3);
    /**********/

    return errcode;
}

ErrorCode_t RemoteRegistRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    ErrorCode_t errcode;
    uint8_t *pbuff; //数据部分
    uint32_t len;
    if(pProto->pCMD[ECH_CMDID_REGISTER]->uiRecvdOptLen > 0)
    {
        pbuff = pProto->pCMD[ECH_CMDID_REGISTER]->ucRecvdOptData;
        len = pProto->pCMD[ECH_CMDID_REGISTER]->uiRecvdOptLen;
        errcode = ERR_NO;
        *psiRetVal = 0;
        switch(pbuff[0])//登陆结果
        {
        case 1: //正常
        case 3:
            *psiRetVal = 1;
            break;
        case 2: //设备不存在，关闭连接
        case 4: //密钥失效，关闭连接
        case 5: //其他错误，关闭连接
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

    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_HEARTBEAT, 10, 3);

    return errcode;
}

ErrorCode_t RemoteHeartRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    ErrorCode_t errcode;
    uint8_t *pbuff; //数据部分
    uint32_t len;

    len = pProto->pCMD[ECH_CMDID_HEARTBEAT]->uiRecvdOptLen;
    if(len > 0)
    {
        *psiRetVal = 1;
    }

    errcode = ERR_NO;
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

    errcode = ERR_NO;
    id = 0;

    pbuff = pProto->pCMD[ECH_CMDID_STATUS]->ucRecvdOptData;
    len = pProto->pCMD[ECH_CMDID_STATUS]->uiRecvdOptLen;
    if(len > 0)
    {
        //pbuff[0...3] 操作序列号
        //pbuff[4] 充电桩接口
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

/** @brief 桩回复平台启停命令
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

    /*** 如下操作为破坏了程序结构，用pCMD中的缓存空间带入一些需要传递的参数*/
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

/** @brief 平台下发的启停命令
 *
 * @param pEVSE EVSE_t*
 * @param pProto echProtocol_t*
 * @param pid uint8_t* 返回ID到程序
 * @param pctrl uint8_t* 返回操作到程序
 * @param psiRetVal int* 返回结果到程序
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteRemoteCtrlRes(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t *pid, uint8_t *pctrl, int *psiRetVal )// →_→
{
    CON_t *pCON;
    uint8_t *pbuff;
    uint32_t len;
    uint8_t strOrderSN_tmp[17];
    double dLimetFee;
    ul2uc ulTmp;
    uint8_t id;
    ErrorCode_t errcode;

    id = 0;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_REMOTE_CTRL]->ucRecvdOptData;
    len = pProto->pCMD[ECH_CMDID_REMOTE_CTRL]->uiRecvdOptLen;
    if(len > 0)
    {
        //pbuff[0...3] 操作ID ，不处理，留在ucRecvdOptData中待回复时使用

        //pbuff[12] 充电桩接口
        id = EchRemoteIDtoCONID(pbuff[12]);
        pCON = CONGetHandle(id);
        if(pCON != NULL)
        {
            //pbuff[13] 操作 1启动，2停止
            if(pbuff[13] == 1)
            {
                //pbuff[4...11] 交易流水号
                HexToStr(&pbuff[4], pCON->order.strOrderSN, 8);

                *pctrl = pbuff[13];

                //pbuff[14...17] 充电金额
                ulTmp.ucVal[0] = pbuff[14];
                ulTmp.ucVal[1] = pbuff[15];
                ulTmp.ucVal[2] = pbuff[16];
                ulTmp.ucVal[3] = pbuff[17];
                dLimetFee = (double)(ntohl(ulTmp.ulVal)) * 0.01;
                pCON->order.dLimitFee = dLimetFee;
                pCON->order.ucStartType = 5;//无卡

                xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
            }
            else if(pbuff[13] == 2)
            {
                /**在这里判断交易号是否相等 */
                HexToStr(&pbuff[4], strOrderSN_tmp, 8);
                if(strcmp(strOrderSN_tmp, pCON->order.strOrderSN) == 0)
                {
                    *pctrl = pbuff[13];
                    xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionRemoteStop);
                }
                else //订单号不相等
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
 * @param ctrl uint8_t     充电桩状态 1 开机，2 停机
 * @param reason uint8_t   停止原因
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

    pbuff[0] = pCON->order.ucStartType;//4 有卡，5 无卡
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

    id = 0;
    memset(strOrderSN_tmp, 0, 17);
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_ORDER]->ucRecvdOptData;
    len = pProto->pCMD[ECH_CMDID_ORDER]->uiRecvdOptLen;
    if(len > 0)
    {
        //[0] 有无卡
        //[1...8] 交易流水号
        //[9] 充电桩接口
        id = EchRemoteIDtoCONID(pbuff[9]);
        pCON = CONGetHandle(id);
        if(pCON != NULL)
        {
            HexToStr(&pbuff[1], strOrderSN_tmp, 8);
            if(strcmp(strOrderSN_tmp, pCON->order.strOrderSN) == 0)
            {
                *psiRetVal = 1;
            }
            else //订单号不相等
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

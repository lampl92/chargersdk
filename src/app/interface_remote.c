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
#include "stringName.h"
#include "cfg_parse.h"

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
        break;
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
ErrorCode_t RemoteIF_SendRegist(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /** @todo (rgw#1#): 调用平台注册接口 */
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
        switch(pbuff[0])//登陆结果
        {
        case 1: //正常
        case 3:
            errcode = ERR_NO;
            *psiRetVal = 1;
            break;
        case 2: //设备不存在，关闭连接
        case 4: //密钥失效，关闭连接
        case 5: //其他错误，关闭连接
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

/** @brief 系统重启后，发送重启结果
 *
 * @param pEVSE EVSE_t*
 * @param pProto echProtocol_t*
 * @param succ uint32_t 成功标志 1 成功，2 失败
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
    //pbuff[0...3] 操作序列号
    ultmpNetSeq.ulVal = htonl(pProto->info.ulOptSN);
    pbuff[0] = ultmpNetSeq.ucVal[0];
    pbuff[1] = ultmpNetSeq.ucVal[1];
    pbuff[2] = ultmpNetSeq.ucVal[2];
    pbuff[3] = ultmpNetSeq.ucVal[3];
    //成功标志
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
        //pbuff[0...3] 操作序列号
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
        //pbuff[0...3] 操作序列号
        //pbuff[4] 充电桩接口
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

/** @brief 桩回复平台启停命令
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

    /*** 如下操作为破坏了程序结构，用pCMD中的缓存空间带入一些需要传递的参数*/
    /*** 2017年6月16日：现在pCMD的缓存专用于接收与传递参数，程序结构上不用担心了*/
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
ErrorCode_t RemoteIF_RecvRemoteCtrl(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t *pid, uint8_t *pctrl, int *psiRetVal )// →_→
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
                pCON->order.ucStartType = 5;//Remote无卡

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
 * @param ctrl uint8_t     充电桩状态 1 开机，2 停机
 * @param reason uint8_t   停止原因
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

    pbuff[0] = pCON->order.ucStartType;//4 有卡，5 无卡
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
            //pbuff[0...3] 操作ID
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
            break;//直接结束 返回
        }
        *psiRetVal = 1;

        //pbuff[4...7] 尖费率
        ultmpNetSeq.ucVal[0] = pbuff[4];
        ultmpNetSeq.ucVal[1] = pbuff[5];
        ultmpNetSeq.ucVal[2] = pbuff[6];
        ultmpNetSeq.ucVal[3] = pbuff[7];
        dtmpPowerFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_sharp = pProto->info.SetProtoCfg(jnProtoPowerFee_sharp, ParamTypeDouble, NULL, 0, &dtmpPowerFee);
        //pbuff[8...11] 峰费率
        ultmpNetSeq.ucVal[0] = pbuff[8];
        ultmpNetSeq.ucVal[1] = pbuff[9];
        ultmpNetSeq.ucVal[2] = pbuff[10];
        ultmpNetSeq.ucVal[3] = pbuff[11];
        dtmpPowerFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_peak = pProto->info.SetProtoCfg(jnProtoPowerFee_peak, ParamTypeDouble, NULL, 0, &dtmpPowerFee);
        //pbuff[12...15] 平费率
        ultmpNetSeq.ucVal[0] = pbuff[12];
        ultmpNetSeq.ucVal[1] = pbuff[13];
        ultmpNetSeq.ucVal[2] = pbuff[14];
        ultmpNetSeq.ucVal[3] = pbuff[15];
        dtmpPowerFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_shoulder = pProto->info.SetProtoCfg(jnProtoPowerFee_shoulder, ParamTypeDouble, NULL, 0, &dtmpPowerFee);
        //pbuff[16...19] 谷费率
        ultmpNetSeq.ucVal[0] = pbuff[16];
        ultmpNetSeq.ucVal[1] = pbuff[17];
        ultmpNetSeq.ucVal[2] = pbuff[18];
        ultmpNetSeq.ucVal[3] = pbuff[19];
        dtmpPowerFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_off_peak = pProto->info.SetProtoCfg(jnProtoPowerFee_off_peak, ParamTypeDouble, NULL, 0, &dtmpPowerFee);

        //pbuff[0...3] 操作ID
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
            //pbuff[0...3] 操作ID
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
            break;//直接结束 返回
        }
        *psiRetVal = 1;
        //pbuff[0...3] 操作ID
        //pbuff[4...7] 尖服务费
        ultmpNetSeq.ucVal[0] = pbuff[4];
        ultmpNetSeq.ucVal[1] = pbuff[5];
        ultmpNetSeq.ucVal[2] = pbuff[6];
        ultmpNetSeq.ucVal[3] = pbuff[7];
        dtmpServFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_sharp = pProto->info.SetProtoCfg(jnProtoServFee_sharp, ParamTypeDouble, NULL, 0, &dtmpServFee);
        //pbuff[8...11] 峰服务费
        ultmpNetSeq.ucVal[0] = pbuff[8];
        ultmpNetSeq.ucVal[1] = pbuff[9];
        ultmpNetSeq.ucVal[2] = pbuff[10];
        ultmpNetSeq.ucVal[3] = pbuff[11];
        dtmpServFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_peak = pProto->info.SetProtoCfg(jnProtoServFee_peak, ParamTypeDouble, NULL, 0, &dtmpServFee);
        //pbuff[12...15] 平服务费
        ultmpNetSeq.ucVal[0] = pbuff[12];
        ultmpNetSeq.ucVal[1] = pbuff[13];
        ultmpNetSeq.ucVal[2] = pbuff[14];
        ultmpNetSeq.ucVal[3] = pbuff[15];
        dtmpServFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_shoulder = pProto->info.SetProtoCfg(jnProtoServFee_shoulder, ParamTypeDouble, NULL, 0, &dtmpServFee);
        //pbuff[16...19] 谷服务费
        ultmpNetSeq.ucVal[0] = pbuff[16];
        ultmpNetSeq.ucVal[1] = pbuff[17];
        ultmpNetSeq.ucVal[2] = pbuff[18];
        ultmpNetSeq.ucVal[3] = pbuff[19];
        dtmpServFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_off_peak = pProto->info.SetProtoCfg(jnProtoServFee_off_peak, ParamTypeDouble, NULL, 0, &dtmpServFee);

        //pbuff[0...3] 操作ID
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
        //pbuff[0...3] 操作ID
        //pbuff[4] 状态数据上报间隔
        ultmpTimCyc_ms = pbuff[4] * 1000;
        set_errcode_stat = pProto->info.SetProtoCfg(jnProtoStatusCyc_ms, ParamTypeU32, NULL, 0, &ultmpTimCyc_ms);
        //pbuff[5] 实时数据上报间隔
        ultmpTimCyc_ms = pbuff[5] * 1000;
        set_errcode_rt = pProto->info.SetProtoCfg(jnProtoRTDataCyc_ms, ParamTypeU32, NULL, 0, &ultmpTimCyc_ms);

        //pbuff[0...3] 操作ID
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
            //pbuff[0...3] 操作ID
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
            break;//直接结束 返回
        }
        *psiRetVal = 1;
        //pbuff[0...3] 操作ID
        //pbuff[4] 尖时间段个数
        ucOffset = 4;
        //尖
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
        //峰
        tmpSegTime.ucSegCont = pbuff[ucOffset++];
        set_errcode_peak = pProto->info.SetProtoCfg(jnProtoSegTime_peak, ParamTypeObj, jnProtoSegCont, ParamTypeU8, &(tmpSegTime.ucSegCont));
        for(i = 0; i < tmpSegTime.ucSegCont; i++)
        {
            tmpSegTime.ucStart[i] = pbuff[ucOffset++];
            tmpSegTime.ucEnd[i] = pbuff[ucOffset++];
            pProto->info.SetProtoCfg(jnProtoSegTime_peak, ParamTypeObj, jnProtoSegStart[i], ParamTypeU8, &(tmpSegTime.ucStart[i]));
            pProto->info.SetProtoCfg(jnProtoSegTime_peak, ParamTypeObj, jnProtoSegEnd[i],   ParamTypeU8, &(tmpSegTime.ucEnd[i]));
        }
        //平
        tmpSegTime.ucSegCont = pbuff[ucOffset++];
        set_errcode_shoulder = pProto->info.SetProtoCfg(jnProtoSegTime_shoulder, ParamTypeObj, jnProtoSegCont, ParamTypeU8, &(tmpSegTime.ucSegCont));
        for(i = 0; i < tmpSegTime.ucSegCont; i++)
        {
            tmpSegTime.ucStart[i] = pbuff[ucOffset++];
            tmpSegTime.ucEnd[i] = pbuff[ucOffset++];
            pProto->info.SetProtoCfg(jnProtoSegTime_shoulder, ParamTypeObj, jnProtoSegStart[i], ParamTypeU8, &(tmpSegTime.ucStart[i]));
            pProto->info.SetProtoCfg(jnProtoSegTime_shoulder, ParamTypeObj, jnProtoSegEnd[i],   ParamTypeU8, &(tmpSegTime.ucEnd[i]));
        }
        //谷
        tmpSegTime.ucSegCont = pbuff[ucOffset++];
        set_errcode_off_peak = pProto->info.SetProtoCfg(jnProtoSegTime_off_peak, ParamTypeObj, jnProtoSegCont, ParamTypeU8, &(tmpSegTime.ucSegCont));
        for(i = 0; i < tmpSegTime.ucSegCont; i++)
        {
            tmpSegTime.ucStart[i] = pbuff[ucOffset++];
            tmpSegTime.ucEnd[i] = pbuff[ucOffset++];
            pProto->info.SetProtoCfg(jnProtoSegTime_off_peak, ParamTypeObj, jnProtoSegStart[i], ParamTypeU8, &(tmpSegTime.ucStart[i]));
            pProto->info.SetProtoCfg(jnProtoSegTime_off_peak, ParamTypeObj, jnProtoSegEnd[i],   ParamTypeU8, &(tmpSegTime.ucEnd[i]));
        }

        //pbuff[0...3] 操作ID
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
        //pbuff[0...3] 操作ID
        //pbuff[4...19] 密钥
        ucOffset = 4;
        for(i = 0; i < 16; i++)
        {
            strTmpKey[i] = pbuff[ucOffset++];
        }
        set_errcode_key = pProto->info.SetProtoCfg(jnProtoNewKey, ParamTypeString, NULL, 0, strTmpKey);
        //pbuff[20...23] 密钥变更时间
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

ErrorCode_t RemoteIF_SendReqFee(uint16_t usSendID, EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    ErrorCode_t errcode;
    errcode = ERR_NO;
    
    pProto->sendCommand(pProto, pEVSE, NULL, usSendID, 0xffff, 0);
    
    *psiRetVal = 1;
    
    return errcode;
}
ErrorCode_t RemoteIF_RecvReqPowerFee(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_REQ_POWERFEE, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        errcode = handle_errcode;
        break;
    case ERR_NO:
        RemoteIF_SendReqFee(ECH_CMDID_REQ_POWERFEE, pEVSE, pProto, psiRetVal);
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_RecvReqServFee(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_REQ_SERVFEE, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        errcode = handle_errcode;
        break;
    case ERR_NO:
        RemoteIF_SendReqFee(ECH_CMDID_REQ_SERVFEE, pEVSE, pProto, psiRetVal);
        break;
    default:
        *psiRetVal = 0;
        break;
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

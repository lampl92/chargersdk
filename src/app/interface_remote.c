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
        while((pechCmdElem = gdsl_list_cursor_get_content (cur)) != NULL)
        {
            printf_safe("RemoteRecvHandle: RecvCmd %d\n", pCMD->CMDType.usRecvCmd);
            memcpy(pbuff, pCMD->ucRecvdOptData, pCMD->ulRecvdOptLen);
            *pLen = pCMD->ulRecvdOptLen;
            errcode = ERR_NO;
            break;
        }
        gdsl_list_cursor_move_to_head (cur);
        while((pechCmdElem = gdsl_list_cursor_get_content(cur)) != NULL)
        {
            printf_safe("RemoteRecvHandle: RecvCmd %d Delete\n", pCMD->CMDType.usRecvCmd);
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
ErrorCode_t RemoteIF_SendCardRTData(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t ctrl, uint8_t reason)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_CARD_RTDATA]->ucRecvdOptData;

    pbuff[55] = ctrl;
    pbuff[56] = reason;
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_CARD_RTDATA, 0xffff, 0);

    return errcode;
}
ErrorCode_t RemoteIF_SendOrder(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_ORDER]->ucRecvdOptData;

    pbuff[0] = pCON->order.ucStartType;//4 有卡，5 无卡
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_ORDER, 20, 5);

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

ErrorCode_t RemoteIF_RecvSetQR(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t flag_set, int *psiRetVal )
{
    CON_t *pCON = NULL;
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t set_errcode;
    ErrorCode_t errcode;
    uint8_t id_cont;
    uint8_t id;//本地枪ID,从0开始
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
            //pbuff[0...3] 操作ID
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0xffff, 0);
            break;//直接结束 返回
        }
        //pbuff[4] 枪口个数
        ucOffset = 4;
        id_cont = pbuff[ucOffset++];

        for(i = 0; i < id_cont; i++)
        {
            //pbuff[5] 充电桩接口
            id = EchRemoteIDtoCONID(pbuff[ucOffset++]);
            pCON = CONGetHandle(id);
            if(pCON == NULL)
            {
                set_errcode = ERR_REMOTE_PARAM;
                break; //查无此枪, 退出循环
            }
            //pbuff[6] 二维码长度
            len_qr = pbuff[ucOffset++];
            //pbuff[7...] 二维码
            for(j = 0; j < len_qr; j++)
            {
                qrcode[j] = pbuff[ucOffset++];
            }
            qrcode[len_qr] = '\0';
            pCON->info.SetCONCfg(pCON, jnQRCode, qrcode, ParamTypeString);
        }

        //pbuff[0...3] 操作ID
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
        pProto->info.BnWFlushListCfg(path);//设置黑白名单需要清除原有名单
        //pbuff[0...3] 操作ID
        //pbuff[4,5] 名单个数
        ucOffset = 4;
        ustmpNetSeq.ucVal[0] = pbuff[ucOffset++];
        ustmpNetSeq.ucVal[1] = pbuff[ucOffset++];
        usListCont = ntohs(ustmpNetSeq.usVal);
        //卡号,16位
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
            //[0...3]操作ID
            //[4] 设置结果
            pProto->pCMD[usCmdID]->ucRecvdOptData[4] = 1;
            pProto->sendCommand(pProto, pEVSE, NULL, usCmdID, 0xffff, 0);
        }
        else
        {
            errcode = ERR_FILE_RW;
            //[0...3]操作ID
            //[4] 设置结果
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
    uint8_t i,j,k;          //i:名单数  j:名单字节长度  k:名单种类
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
        //pbuff[0...3] 操作ID

        ucOffset = 4;
        for(k = 0; k < 2; k++) //先处理白名单 0, 再处理黑名单 1
        {
            if(k == 1)
            {
                strcpy(path, pathBlackList);
            }
            else if(k == 0)
            {
                strcpy(path, pathWhiteList);
            }
            //pbuff[4,5] 名单个数
            ustmpNetSeq.ucVal[0] = pbuff[ucOffset++];
            ustmpNetSeq.ucVal[1] = pbuff[ucOffset++];
            usListCont = ntohs(ustmpNetSeq.usVal);
            //卡号,16位
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
            //pbuff[0...3] 操作ID
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_NO;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0xffff, 0);
        }
        else
        {
            //pbuff[0...3] 操作ID
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

ErrorCode_t RemoteIF_SendCardStart(EVSE_t *pEVSE, echProtocol_t *pProto, RFIDDev_t *pRfid)
{
    uint8_t ucOrderSN[8] = {0};
    uint8_t strOrderSN[17] = {0};
    uint8_t strCardID[17] = {0};
    ul2uc ultmpNetSeq;
    ErrorCode_t errcode = ERR_NO;

    HexToStr(pRfid->order.ucCardID, strCardID, 8);
    if (pProto->info.BnWIsListCfg(pathBlackList, strCardID) == 1)
    {
        pRfid->order.ucAccountStatus = 0;
        pRfid->order.dBalance = 0;

        return ERR_BLACK_LIST;
    }
    else if(pProto->info.BnWIsListCfg(pathWhiteList, strCardID) == 1)
    //else if(1)    
    {
        pRfid->order.ucAccountStatus = 1;
        pRfid->order.dBalance = 9999.99;

        ultmpNetSeq.ulVal = time(NULL); // 采用时间戳作为交易流水号, 协议中标识为BIN 8, 因此不做字节序转换
        ucOrderSN[0] = 0;
        ucOrderSN[1] = 0;
        ucOrderSN[2] = 0;
        ucOrderSN[3] = 0;
        ucOrderSN[4] = ultmpNetSeq.ucVal[0];
        ucOrderSN[5] = ultmpNetSeq.ucVal[1];
        ucOrderSN[6] = ultmpNetSeq.ucVal[2];
        ucOrderSN[7] = ultmpNetSeq.ucVal[3];

        //保存流水号到order
        HexToStr(ucOrderSN, strOrderSN, 8);
        strcpy(pRfid->order.strOrderSN, strOrderSN);

        return ERR_WHITE_LIST;
    }

    pProto->sendCommand(pProto, pEVSE, pRfid, ECH_CMDID_CARD_START, 20, 3); //注意传的参数是 pRfid
    return errcode;
}
ErrorCode_t RemoteIF_RecvCardStart(echProtocol_t *pProto, RFIDDev_t *pRfid, uint8_t *pucVaild, int *psiRetVal)
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode;
    uint8_t con_id;
    uint8_t strCardID[17] = {0};
    uint8_t strOrderCardID[17] = {0};
    uint8_t ucOrderSN[8] = {0};
    uint8_t strOrderSN[17] = {0};
    ul2uc ultmpNetSeq;
    uint8_t ucOffset;
    int i;

    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_CARD_START, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        ucOffset = 0;
        //pbuff[0] 充电桩接口
        con_id = EchRemoteIDtoCONID(pbuff[ucOffset++]);
        if(con_id != pRfid->order.ucCONID)
        {
            *psiRetVal = 0;
            break;
        }
        //pbuff[1...16] 卡号
        for(i = 0; i < 16; i++)
        {
            strCardID[i] = pbuff[ucOffset++];
        }
        HexToStr(pRfid->order.ucCardID, strOrderCardID, 8);
        if(strcmp(strCardID, strOrderCardID) != 0)
        {
            *psiRetVal = 0;
            break;
        }
        //pbuff[17...24] 流水号
        for(i = 0; i < 8; i++)
        {
            ucOrderSN[i] = pbuff[ucOffset++];
        }
        HexToStr(ucOrderSN, strOrderSN, 8);
        if(strcmp(strOrderSN, pRfid->order.strOrderSN) != 0)
        {
            *psiRetVal = 0;
            break;
        }
        //pbuff[25] 验证结果
        *pucVaild = pbuff[ucOffset++];
        //pbuff[26...29] 可用余额
        ultmpNetSeq.ucVal[0] = pbuff[ucOffset++];
        ultmpNetSeq.ucVal[1] = pbuff[ucOffset++];
        ultmpNetSeq.ucVal[2] = pbuff[ucOffset++];
        ultmpNetSeq.ucVal[3] = pbuff[ucOffset++];
        pRfid->order.dBalance = (double)(ntohl(ultmpNetSeq.ulVal)) / 100.0;
        if(pRfid->order.dBalance > 0)
        {
            pRfid->order.ucAccountStatus = 1;
        }
        else
        {
            pRfid->order.ucAccountStatus = 2;
        }

        break;
    default:
        *psiRetVal = 0;
        break;
    }
    errcode = handle_errcode;
    return errcode;
}

ErrorCode_t RemoteIF_SendCardStartRes(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t succ)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_CARD_START_RES]->ucRecvdOptData;
    //[18] 启动结果
    if(succ == 1)
    {
        pbuff[18] = 1;
    }
    else if(succ == 0)
    {
        pbuff[18] = 2;
    }
    /*********************/
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_CARD_START_RES, 30, 3);

    return errcode;
}
ErrorCode_t RemoteIF_RecvCardStartRes(echProtocol_t *pProto, int *psiRetVal)
{
    CON_t *pCON;
    uint8_t id;

    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode;

    id = 0;
    errcode = ERR_NO;
    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_CARD_START_RES, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        //pbuff[0] 充电桩接口
        id = EchRemoteIDtoCONID(pbuff[0]);
        pCON = CONGetHandle(id);
        if(pCON != NULL)
        {
            *psiRetVal = 1;
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_SendCardStopRes(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON)
{
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_CARD_STOP_RES, 30, 3);

    return ERR_NO;
}
ErrorCode_t RemoteIF_RecvCardStopRes(echProtocol_t *pProto, int *psiRetVal)
{
    CON_t *pCON;
    uint8_t id;

    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t handle_errcode;
    ErrorCode_t errcode;

    id = 0;
    errcode = ERR_NO;
    handle_errcode = RemoteRecvHandle(pProto, ECH_CMDID_CARD_STOP_RES, pbuff, &len);
    switch(handle_errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        //pbuff[0] 充电桩接口
        id = EchRemoteIDtoCONID(pbuff[0]);
        pCON = CONGetHandle(id);
        if(pCON != NULL)
        {
            *psiRetVal = 1;
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_SendUpFault(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    CON_t *pCON;
    uint32_t ulTotalCON;
    uint8_t data_old[6] = {0};
    uint8_t *pbuff;
    EventBits_t uxBit;
    int i;

    ulTotalCON = pListCON->Total;

    memcpy(data_old, pProto->status.fault, 6);

    //1-1 绝缘故障
    if(pEVSE->status.ulPEState == 1)
    {
        SET_BIT(pProto->status.fault[0], BIT_0);
    }
    else
    {
        CLEAR_BIT(pProto->status.fault[0], BIT_0);
    }
    //1-2 输出连接器过温故障
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventCharge);
        if((uxBit & defEventBitCONACTempOK) != defEventBitCONACTempOK)
        {
            SET_BIT(pProto->status.fault[0], BIT_1);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.fault[0], BIT_1);
        }
    }
    //1-7 电流过大
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventCharge);
        if((uxBit & defEventBitCONCurrOK) != defEventBitCONCurrOK)
        {
            SET_BIT(pProto->status.fault[0], BIT_6);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.fault[0], BIT_6);
        }
    }
    //1-8 电压异常
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventCharge);
        if((uxBit & defEventBitCONVoltOK) != defEventBitCONVoltOK)
        {
            SET_BIT(pProto->status.fault[0], BIT_7);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.fault[0], BIT_7);
        }
    }
    //[1]:3  2-4 电能表故障
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventException);
        if((uxBit & defEventBitExceptionMeter) != defEventBitExceptionMeter)
        {
            SET_BIT(pProto->status.fault[1], BIT_3);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.fault[1], BIT_3);
        }
    }
    //[1]:5 2-6 PWM切换故障
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventException);
        if((uxBit & defEventBitExceptionCPSwitch) != defEventBitExceptionCPSwitch)
        {
            SET_BIT(pProto->status.fault[1], BIT_5);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.fault[1], BIT_5);
        }
    }
    //[1]:6 2-7 温度传感器故障
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventException);
        if((uxBit & defEventBitExceptionTempSensor) != defEventBitExceptionTempSensor)
        {
            SET_BIT(pProto->status.fault[1], BIT_6);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.fault[1], BIT_6);
        }
    }
    //[1]:7 2-8 急停报警
    if(pEVSE->status.ulScramState == 1)
    {
        SET_BIT(pProto->status.fault[1], BIT_7);
    }
    else
    {
        CLEAR_BIT(pProto->status.fault[1], BIT_7);
    }
    //[2]:0 3-1 充电枪温度传感器故障
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventException);
        if((uxBit & defEventBitExceptionSocketTempSensor) != defEventBitExceptionSocketTempSensor)
        {
            SET_BIT(pProto->status.fault[2], BIT_0);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.fault[2], BIT_0);
        }
    }
    //[3]:2 4-3 接触器故障
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventException);
        if((uxBit & defEventBitExceptionRelayPaste) != defEventBitExceptionRelayPaste)
        {
            SET_BIT(pProto->status.fault[3], BIT_2);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.fault[3], BIT_2);
        }
    }

    if(memcmp(pProto->status.fault, data_old, 6) != 0)
    {
        pbuff = pProto->pCMD[ECH_CMDID_UP_FAULT]->ucRecvdOptData;
        memcpy(pbuff, pProto->status.fault, 6);
        pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_UP_FAULT, 0xffff, 0);
    }

    return ERR_NO;
}

ErrorCode_t RemoteIF_SendUpWarning(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    CON_t *pCON;
    uint32_t ulTotalCON;
    uint8_t data_old_w[6] = {0};
    uint8_t data_old_p[6] = {0};
    uint8_t *pbuff;
    EventBits_t uxBit;
    int i;

    ulTotalCON = pListCON->Total;

    memcpy(data_old_w, pProto->status.warning, 6);
    memcpy(data_old_p, pProto->status.protect, 6);

    //[0]:0 1-1 防雷器告警
    if(pEVSE->status.ulArresterState == 1)
    {
        SET_BIT(pProto->status.warning[0], BIT_0);
    }
    else
    {
        CLEAR_BIT(pProto->status.warning[0], BIT_0);
    }
    //[1]:1 2-2 一般漏电
    if(pEVSE->status.ulPEState == 1)
    {
        SET_BIT(pProto->status.warning[0], BIT_1);
    }
    else
    {
        CLEAR_BIT(pProto->status.warning[0], BIT_1);
    }
    //[1]:4 2-5 电网电压异常
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventCharge);
        if((uxBit & defEventBitCONVoltOK) != defEventBitCONVoltOK)
        {
            SET_BIT(pProto->status.warning[1], BIT_4);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.warning[1], BIT_4);
        }
    }
    //[2]:7 3-8 电网频率异常
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventCharge);
        if((uxBit & defEventBitCONFreqOK) != defEventBitCONFreqOK)
        {
            SET_BIT(pProto->status.warning[2], BIT_7);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.warning[2], BIT_7);
        }
    }
    //[3]:0 4-1 一般过载
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventCharge);
        if((uxBit & defEventBitCONCurrOK) != defEventBitCONCurrOK)
        {
            SET_BIT(pProto->status.warning[3], BIT_0);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.warning[3], BIT_0);
        }
    }

    //---保护信息
    //[1]:7 2-8 电流过高保护
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventCharge);
        if((uxBit & defEventBitCONCurrOK) != defEventBitCONCurrOK)
        {
            SET_BIT(pProto->status.protect[1], BIT_7);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.protect[1], BIT_7);
        }
    }
    //[2]:0 3-1 电压过高保护
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventCharge);
        if((uxBit & defEventBitCONVoltOK) != defEventBitCONVoltOK)
        {
            SET_BIT(pProto->status.protect[2], BIT_0);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.protect[2], BIT_0);
        }
    }
    //[2]:1 3-2 硬件漏电保护
    if(pEVSE->status.ulPEState == 1)
    {
        SET_BIT(pProto->status.protect[2], BIT_1);
    }
    else
    {
        CLEAR_BIT(pProto->status.protect[2], BIT_1);
    }


    if(memcmp(pProto->status.warning, data_old_w, 6) != 0 ||
       memcmp(pProto->status.protect, data_old_p, 6) != 0)
    {
        pbuff = pProto->pCMD[ECH_CMDID_UP_FAULT]->ucRecvdOptData;
        memcpy(pbuff, pProto->status.warning, 6);
        memcpy(&pbuff[6], pProto->status.protect, 6);
        pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_UP_WARNING, 0xffff, 0);
    }

    return ERR_NO;
}

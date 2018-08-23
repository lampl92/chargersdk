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
#include "utils.h"
#include "stringName.h"
#include "cfg_parse.h"
#include "libEcharge/ech_globals.h"
#include "libEcharge/ech_protocol_proc.h"
#include "libEcharge/ech_ftp.h"

#include "evse_debug.h"

/** @brief
 *
 * @param pucRetVal uint8_t*     1注册成功  0注册失败
 * @return ErrorCode_t
 *
 */
ErrorCode_t RemoteIF_SendLogin(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /** 调用平台注册接口 */
    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_REGISTER, 10, 1);
    /**********/

    return errcode;
}
//从队尾取，取到后删除，并传出element
ErrorCode_t RemoteRecvHandleWithCON(echProtocol_t *pProto, 
                                uint16_t usCmdID, 
                                uint8_t con_id, uint8_t con_id_pos, 
                                uint8_t *pbuff, uint32_t *pLen)
{
    ErrorCode_t errcode;
    echCMD_t *pCMD;
    echCmdElem_t *pechCmdElem;
    echProtoElem_t *pechProtoElem;
    gdsl_list_cursor_t cur; //命令队列光标
    gdsl_list_cursor_t cs; //发送队列光标
    EventBits_t bits;

    errcode = ERR_REMOTE_NODATA;
    pCMD = pProto->pCMD[usCmdID];
    if (gdsl_list_get_size(pCMD->plRecvCmd) > 0)
        printf_protolog("CMD %d[0x%02X] plRecvCmd size = %d\n", pCMD->CMDType.usRecvCmd, pCMD->CMDType.usRecvCmd, gdsl_list_get_size(pCMD->plRecvCmd));
    if (xSemaphoreTake(pCMD->xMutexCmd, 1000) == pdPASS)
    {
        cur = gdsl_list_cursor_alloc(pCMD->plRecvCmd);
        gdsl_list_cursor_move_to_head(cur);
        while ((pechCmdElem = gdsl_list_cursor_get_content(cur)) != NULL)
        {
            if (time(NULL) - pechCmdElem->timestamp > 20)//接收命令20s未处理则超时删除
            {
                gdsl_list_cursor_delete(cur);
                continue;
            }
            if (EchRemoteIDtoCONID(pechCmdElem->pbuff[con_id_pos]) == con_id)
            {
                printf_protolog("CON%d RecvCmd %02X [%d]\n", con_id, pCMD->CMDType.usRecvCmd, pCMD->CMDType.usRecvCmd);
                memcpy(pbuff, pechCmdElem->pbuff, pechCmdElem->len);
                *pLen = pechCmdElem->len;
                gdsl_list_cursor_delete(cur);
                errcode = ERR_NO;
                continue;//旧的head删除，只提取最新的（插入到tail）的协议
            }
            gdsl_list_cursor_step_forward(cur);
        }
        gdsl_list_cursor_free(cur);
        xSemaphoreGive(pCMD->xMutexCmd);
        
        if (errcode != ERR_REMOTE_NODATA)
        {
            if (xSemaphoreTake(pProto->xMutexProtoSend, 1000) == pdPASS)
            {
                cs = gdsl_list_cursor_alloc(pProto->plechSendCmd);
                gdsl_list_cursor_move_to_head(cs);
                while ((pechProtoElem = gdsl_list_cursor_get_content(cs)) != NULL)
                {
                    if (pechProtoElem->cmd_id == usCmdID && pechProtoElem->con_id == con_id)
                    {
                        gdsl_list_cursor_delete(cs);  //请求命令收到平台回复并已处理, 删除命令
                        printf_protolog("CON%d SendCmd %02X [%d] Recved\n", con_id, pechProtoElem->cmd.usSendCmd, pechProtoElem->cmd.usSendCmd);
                        continue;
                    }
                    gdsl_list_cursor_step_forward(cs);
                }
                gdsl_list_cursor_free(cs);
                xSemaphoreGive(pProto->xMutexProtoSend);
            }//if mutex
            else
            {
                printf_safe("xMutexProtoSend Timeout---> [%0X]%d!!!\n", pCMD->CMDType.usRecvCmd, pCMD->CMDType.usRecvCmd);
            }
        }
        
        bits = xEventGroupWaitBits(pCMD->xHandleEventCmd, defEventBitProtoCmdDataTimeout, pdTRUE, pdTRUE, 0);
        if ((bits & defEventBitProtoCmdDataTimeout) == defEventBitProtoCmdDataTimeout)
        {
            printf_protolog("recv event:cmd %d[0x%02X] 超时\n", pCMD->CMDType.usSendCmd, pCMD->CMDType.usSendCmd);
            errcode = ERR_REMOTE_TIMEOUT;
        }
    }//if mutex

    return errcode;
}

ErrorCode_t RemoteRecvHandle(echProtocol_t *pProto, uint16_t usCmdID, uint8_t *pbuff, uint32_t *pLen)
{
    ErrorCode_t errcode;
    echCMD_t *pCMD;
    echCmdElem_t *pechCmdElem;
    echProtoElem_t *pechProtoElem;
    gdsl_list_cursor_t cur;//命令队列光标
    gdsl_list_cursor_t cs;//发送队列光标
    EventBits_t bits;

    errcode = ERR_REMOTE_NODATA;
    pCMD = pProto->pCMD[usCmdID];
    if (gdsl_list_get_size(pCMD->plRecvCmd) > 0)
        printf_protolog("CMD %d[0x%02X] plRecvCmd size = %d\n", pCMD->CMDType.usRecvCmd, pCMD->CMDType.usRecvCmd, gdsl_list_get_size(pCMD->plRecvCmd));
    if(xSemaphoreTake(pCMD->xMutexCmd, 1000) == pdPASS)
    {
        cur = gdsl_list_cursor_alloc (pCMD->plRecvCmd);
        gdsl_list_cursor_move_to_tail (cur);//只要链表中最新接收的协议, 因此从tail开始
        while((pechCmdElem = gdsl_list_cursor_get_content (cur)) != NULL)
        {
            printf_protolog("RecvCmd %d[0x%02X]\n", pCMD->CMDType.usRecvCmd, pCMD->CMDType.usRecvCmd);
            memcpy(pbuff, pechCmdElem->pbuff, pechCmdElem->len);
            *pLen = pechCmdElem->len;
            errcode = ERR_NO;
            break;
        }
        gdsl_list_cursor_move_to_head (cur);
        while((pechCmdElem = gdsl_list_cursor_get_content(cur)) != NULL)
        {
            printf_protolog("RecvCmd %d[0x%02X] Delete\n", pCMD->CMDType.usRecvCmd, pCMD->CMDType.usRecvCmd);
            gdsl_list_cursor_delete(cur);
        }
        gdsl_list_cursor_free(cur);
        xSemaphoreGive(pCMD->xMutexCmd);
        
        if(errcode != ERR_REMOTE_NODATA)
        {
            if (xSemaphoreTake(pProto->xMutexProtoSend, 1000) == pdPASS)
            {
                cs = gdsl_list_cursor_alloc(pProto->plechSendCmd);
                gdsl_list_cursor_move_to_head(cs);
                while ((pechProtoElem = gdsl_list_cursor_get_content(cs)) != NULL)
                {
                    if (pechProtoElem->cmd_id == usCmdID)
                    {
                        gdsl_list_cursor_delete(cs);   //请求命令收到平台回复并已处理, 删除命令
                        printf_protolog("SendCmd %d[0x%02X] Recved\n", pechProtoElem->cmd.usSendCmd, pechProtoElem->cmd.usSendCmd);
                        break;
                    }
                    gdsl_list_cursor_step_forward(cs);
                }
                gdsl_list_cursor_free(cs);
                xSemaphoreGive(pProto->xMutexProtoSend);
            }//if mutex
            else
            {
                printf_safe("xMutexProtoSend Timeout---> [%0X]%d!!!\n", pCMD->CMDType.usRecvCmd, pCMD->CMDType.usRecvCmd);
            }
        }
        
        bits = xEventGroupWaitBits(pCMD->xHandleEventCmd, defEventBitProtoCmdDataTimeout, pdTRUE, pdTRUE, 0);
        if ((bits & defEventBitProtoCmdDataTimeout) == defEventBitProtoCmdDataTimeout)
        {
            printf_protolog("recv event:cmd %d[0x%02X] 超时\n", pCMD->CMDType.usSendCmd, pCMD->CMDType.usSendCmd);
            errcode = ERR_REMOTE_TIMEOUT;
        }
    }//if mutex

    return errcode;
}

ErrorCode_t RemoteIF_RecvLogin(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t errcode;

    errcode = RemoteRecvHandle(pProto, ECH_CMDID_REGISTER, pbuff, &len);

    switch (errcode)
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

    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_HEARTBEAT, 20, 1);

    return errcode;
}

ErrorCode_t RemoteIF_RecvHeart(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t errcode;

    errcode = RemoteRecvHandle(pProto, ECH_CMDID_HEARTBEAT, pbuff, &len);
    switch (errcode)
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
    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_RESET, 0, 1);

    ulOptSN = 0;
    cfg_set_uint32(pathProtoCfg, &ulOptSN, "%s", jnProtoOptSN);

    return errcode;
}
ErrorCode_t RemoteIF_RecvReset(echProtocol_t *pProto, uint32_t *pulOptSN, int *psiRetVal)
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t errcode;
    ul2uc ultmpNetSeq;

    errcode = RemoteRecvHandle(pProto, ECH_CMDID_RESET, pbuff, &len);
    switch (errcode)
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
    return errcode;
}
ErrorCode_t RemoteIF_SendStatus(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_STATUS, 0, 1);

    return errcode;
}
ErrorCode_t RemoteIF_RecvStatus(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    CON_t *pCON;
    uint8_t id;
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t errcode = ERR_NO;

    errcode = RemoteRecvHandle(pProto, ECH_CMDID_STATUS, pbuff, &len);
    switch (errcode)
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
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_REMOTE_CTRL, 0, 1);

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
extern int dummyordersn;
ErrorCode_t RemoteIF_RecvRemoteCtrl(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCONin, uint8_t *pid, uint8_t *pctrl, int *psiRetVal)// →_→
{
    CON_t *pCON;
    uint8_t id;

    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t errcode = ERR_NO;

    double dLimetFee;
    ul2uc ulTmp;
    ull2uc ullOrderSN;

    id = 0;
    errcode = RemoteRecvHandleWithCON(pProto, ECH_CMDID_REMOTE_CTRL, pCONin->info.ucCONID, 12, pbuff, &len);
    switch (errcode)
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
                pCON->order.ucCONID = id;//启动枪号
                
                //pbuff[4...11] 交易流水号
                memcpy(ullOrderSN.ucArray, &pbuff[4], 8);
                pCON->order.ullOrderSN = ntohll(ullOrderSN.ullVal);

                *pctrl = pbuff[13];

                //pbuff[14...17] 充电金额
                ulTmp.ucVal[0] = pbuff[14];
                ulTmp.ucVal[1] = pbuff[15];
                ulTmp.ucVal[2] = pbuff[16];
                ulTmp.ucVal[3] = pbuff[17];
                dLimetFee = (double)(ntohl(ulTmp.ulVal)) * 0.01;
                pCON->order.dLimitFee = dLimetFee;
#if EVSE_FUTURE
                pCON->order.ulLimitTime = 0;
                pCON->order.dLimitPower = 0;
#endif
                pCON->order.ucStartType = 5;//Remote无卡
                
                pCON->appoint.status = 1;

                xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
            }
            else if(pbuff[13] == 2)
            {
                /**在这里判断交易号是否相等 */
                memcpy(ullOrderSN.ucArray, &pbuff[4], 8);
                if(pCON->order.ullOrderSN == ntohll(ullOrderSN.ullVal) || dummyordersn)//checkordersn可以通过cli设置，防止发生异常时，APP无法停止
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
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_RTDATA, 0, 1);

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
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_CARD_RTDATA, 0, 1);

    return errcode;
}
ErrorCode_t RemoteIF_SendOrder(EVSE_t *pEVSE, echProtocol_t *pProto, OrderData_t *pOrder)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_ORDER]->ucRecvdOptData;

    pbuff[0] = pOrder->ucStartType;//4 有卡，5 无卡
    pProto->sendCommand(pProto, pEVSE, pOrder, ECH_CMDID_ORDER, 20, 3);

    return errcode;
}
ErrorCode_t RemoteIF_RecvOrder(EVSE_t *pEVSE, echProtocol_t *pProto, OrderData_t *pOrder, int *psiRetVal)
{
    CON_t *pCON;
    uint8_t id;

    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t errcode;
    ull2uc ullOrderSN;

    id = 0;
    errcode = ERR_NO;

    errcode = RemoteRecvHandleWithCON(pProto, ECH_CMDID_ORDER, pOrder->ucCONID, 9, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_REMOTE_TIMEOUT:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        //[0] 有无卡
        if (pbuff[0] != pOrder->ucStartType)
        {
            printf_safe("CON%d 启动类型不一致\n", EchRemoteIDtoCONID(pbuff[9]));
            printf_safe("-Remote Type: %d \n", pbuff[0]);
            printf_safe("-Local  Type: %d \n", pOrder->ucStartType);
            errcode = ERR_REMOTE_PARAM;
            break;
        }
        //[1...8] 交易流水号
        //[9] 充电桩接口
        id = EchRemoteIDtoCONID(pbuff[9]);
        if (id != pOrder->ucCONID)
        {
            printf_safe("订单枪号不一致\n");
            printf_safe("-Remote ID: %d \n", pbuff[9]);
            printf_safe("-Remote ID conv: %d \n", id);
            printf_safe("-Local  ID: %d \n", pOrder->ucCONID);
            errcode = ERR_REMOTE_PARAM;
            break;
        }
        pCON = CONGetHandle(id);
        if(pCON != NULL)
        {
            memcpy(ullOrderSN.ucArray, &pbuff[1], 8);
            if(pCON->order.ullOrderSN == ntohll(ullOrderSN.ullVal))
            {
                *psiRetVal = 1;
            }
            else //订单号不相等
            {
                *psiRetVal = 0;
                printf_safe("OrderSN not equal!!! \n");
                printf_safe("-Remote OrderSN: %lf \n", (double)(ntohll(ullOrderSN.ullVal)));
                printf_safe("-Local  OrderSN: %lf \n", (double)(pOrder->ullOrderSN));
                
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

ErrorCode_t RemoteIF_SendSetEnergyFee(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_HEARTBEAT, 20, 3);

    return errcode;
}

ErrorCode_t RemoteIF_RecvSetEnergyFee(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t flag_set, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t set_errcode_sharp;
    ErrorCode_t set_errcode_peak;
    ErrorCode_t set_errcode_shoulder;
    ErrorCode_t set_errcode_off_peak;
    ErrorCode_t errcode;
    ul2uc ultmpNetSeq;
    double dtmpEnergyFee;

    errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_ENERGYFEE, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
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

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1);
            break;//直接结束 返回
        }
        *psiRetVal = 1;

        //pbuff[4...7] 尖费率
        ultmpNetSeq.ucVal[0] = pbuff[4];
        ultmpNetSeq.ucVal[1] = pbuff[5];
        ultmpNetSeq.ucVal[2] = pbuff[6];
        ultmpNetSeq.ucVal[3] = pbuff[7];
        dtmpEnergyFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_sharp = cfg_set_double(pathProtoCfg, &dtmpEnergyFee, "%s", jnProtoEnergyFee_sharp);
        //pbuff[8...11] 峰费率
        ultmpNetSeq.ucVal[0] = pbuff[8];
        ultmpNetSeq.ucVal[1] = pbuff[9];
        ultmpNetSeq.ucVal[2] = pbuff[10];
        ultmpNetSeq.ucVal[3] = pbuff[11];
        dtmpEnergyFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_peak = cfg_set_double(pathProtoCfg, &dtmpEnergyFee, "%s", jnProtoEnergyFee_peak);
        //pbuff[12...15] 平费率
        ultmpNetSeq.ucVal[0] = pbuff[12];
        ultmpNetSeq.ucVal[1] = pbuff[13];
        ultmpNetSeq.ucVal[2] = pbuff[14];
        ultmpNetSeq.ucVal[3] = pbuff[15];
        dtmpEnergyFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_shoulder = cfg_set_double(pathProtoCfg, &dtmpEnergyFee, "%s", jnProtoEnergyFee_shoulder); 
        //pbuff[16...19] 谷费率
        ultmpNetSeq.ucVal[0] = pbuff[16];
        ultmpNetSeq.ucVal[1] = pbuff[17];
        ultmpNetSeq.ucVal[2] = pbuff[18];
        ultmpNetSeq.ucVal[3] = pbuff[19];
        dtmpEnergyFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_off_peak = cfg_set_double(pathProtoCfg, &dtmpEnergyFee, "%s", jnProtoEnergyFee_off_peak);

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
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0, 1);
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_FILE_RW;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1);
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
    ErrorCode_t set_errcode_sharp;
    ErrorCode_t set_errcode_peak;
    ErrorCode_t set_errcode_shoulder;
    ErrorCode_t set_errcode_off_peak;
    ErrorCode_t errcode;
    ul2uc ultmpNetSeq;
    double dtmpServFee;

    errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_SERVFEE, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
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

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1);
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
        set_errcode_sharp = cfg_set_double(pathProtoCfg, &dtmpServFee, "%s", jnProtoServFee_sharp); 
        //pbuff[8...11] 峰服务费
        ultmpNetSeq.ucVal[0] = pbuff[8];
        ultmpNetSeq.ucVal[1] = pbuff[9];
        ultmpNetSeq.ucVal[2] = pbuff[10];
        ultmpNetSeq.ucVal[3] = pbuff[11];
        dtmpServFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_peak = cfg_set_double(pathProtoCfg, &dtmpServFee, "%s", jnProtoServFee_peak); 
        //pbuff[12...15] 平服务费
        ultmpNetSeq.ucVal[0] = pbuff[12];
        ultmpNetSeq.ucVal[1] = pbuff[13];
        ultmpNetSeq.ucVal[2] = pbuff[14];
        ultmpNetSeq.ucVal[3] = pbuff[15];
        dtmpServFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_shoulder = cfg_set_double(pathProtoCfg, &dtmpServFee, "%s", jnProtoServFee_shoulder);
        //pbuff[16...19] 谷服务费
        ultmpNetSeq.ucVal[0] = pbuff[16];
        ultmpNetSeq.ucVal[1] = pbuff[17];
        ultmpNetSeq.ucVal[2] = pbuff[18];
        ultmpNetSeq.ucVal[3] = pbuff[19];
        dtmpServFee = (double)(ntohl(ultmpNetSeq.ulVal) * 0.0001);
        set_errcode_off_peak = cfg_set_double(pathProtoCfg, &dtmpServFee, "%s", jnProtoServFee_off_peak);

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
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0, 1);
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_FILE_RW;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1);
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
    ErrorCode_t set_errcode_stat;
    ErrorCode_t set_errcode_rt;
    ErrorCode_t errcode;
    uint32_t ultmpTimCyc_ms;

    errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_CYC, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        //pbuff[0...3] 操作ID
        //pbuff[4] 状态数据上报间隔
        ultmpTimCyc_ms = pbuff[4] * 1000;
        set_errcode_stat = cfg_set_uint32(pathProtoCfg, &ultmpTimCyc_ms, "%s", jnProtoStatusCyc_ms);
        //pbuff[5] 实时数据上报间隔
        ultmpTimCyc_ms = pbuff[5] * 1000;
        set_errcode_rt = cfg_set_uint32(pathProtoCfg, &ultmpTimCyc_ms, "%s", jnProtoRTDataCyc_ms);

        //pbuff[0...3] 操作ID
        if(set_errcode_stat == ERR_NO &&
           set_errcode_rt == ERR_NO)
        {
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_NO;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0, 1);
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_FILE_RW;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1);
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
    ErrorCode_t errcode;
    EchSegTime_t tmpSegTime;
    uint8_t ucOffset;
    int i;
    cJSON *jsCfgObj;

    ucOffset = 0;
    errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_TIMESEG, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        jsCfgObj = GetCfgObj(pathProtoCfg, &errcode);//为避免以下多次操作文件，选择cfgobj接口
        if (flag_set == 0 || jsCfgObj == NULL)
        {
            *psiRetVal = 1;
            errcode = ERR_NO;
            //pbuff[0...3] 操作ID
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1);
            break;//直接结束 返回
        }
        *psiRetVal = 1;
        //pbuff[0...3] 操作ID
        //pbuff[4] 尖时间段个数
        ucOffset = 4;
        //尖
        tmpSegTime.ucPeriodCont = pbuff[ucOffset++];
        cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucPeriodCont), "%s.%s", jnProtoSegTime_sharp, jnProtoSegCont);
        //pbuff[5...SegCont]
        for(i = 0; i < tmpSegTime.ucPeriodCont; i++)
        {
            tmpSegTime.ucStart[i] = pbuff[ucOffset++];
            tmpSegTime.ucEnd[i] = pbuff[ucOffset++];
            cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucStart[i]), "%s.Start%d", jnProtoSegTime_sharp, i + 1);
            cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucEnd[i]), "%s.End%d", jnProtoSegTime_sharp, i + 1);
        }
        //峰
        tmpSegTime.ucPeriodCont = pbuff[ucOffset++];
        cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucPeriodCont), "%s.%s", jnProtoSegTime_peak, jnProtoSegCont);
        for(i = 0; i < tmpSegTime.ucPeriodCont; i++)
        {
            tmpSegTime.ucStart[i] = pbuff[ucOffset++];
            tmpSegTime.ucEnd[i] = pbuff[ucOffset++];
            cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucStart[i]), "%s.Start%d", jnProtoSegTime_peak, i + 1);
            cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucEnd[i]), "%s.End%d", jnProtoSegTime_peak, i + 1);
        }
        //平
        tmpSegTime.ucPeriodCont = pbuff[ucOffset++];
        cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucPeriodCont), "%s.%s", jnProtoSegTime_shoulder, jnProtoSegCont);
        for(i = 0; i < tmpSegTime.ucPeriodCont; i++)
        {
            tmpSegTime.ucStart[i] = pbuff[ucOffset++];
            tmpSegTime.ucEnd[i] = pbuff[ucOffset++];
            cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucStart[i]), "%s.Start%d", jnProtoSegTime_shoulder, i + 1);
            cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucEnd[i]), "%s.End%d", jnProtoSegTime_shoulder, i + 1);
        }
        //谷
        tmpSegTime.ucPeriodCont = pbuff[ucOffset++];
        cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucPeriodCont), "%s.%s", jnProtoSegTime_off_peak, jnProtoSegCont);
        for(i = 0; i < tmpSegTime.ucPeriodCont; i++)
        {
            tmpSegTime.ucStart[i] = pbuff[ucOffset++];
            tmpSegTime.ucEnd[i] = pbuff[ucOffset++];
            cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucStart[i]), "%s.Start%d", jnProtoSegTime_off_peak, i + 1);
            cfgobj_set_uint8(jsCfgObj, &(tmpSegTime.ucEnd[i]), "%s.End%d", jnProtoSegTime_off_peak, i + 1);
        }
        //pbuff[0...3] 操作ID
        errcode = SetCfgObj(pathProtoCfg, jsCfgObj, 0);
        if (errcode == ERR_NO)
        {
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0, 1); 
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1); 
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
    ErrorCode_t set_errcode_key;
    ErrorCode_t set_errcode_time;
    ErrorCode_t errcode;
    char strTmpKey[16+1] = {0};
    uint32_t ultmpTim;
    ul2uc ultmpNetSeq;
    int i;
    uint8_t ucOffset;

    errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_KEY, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
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
        set_errcode_key = cfg_set_string(pathProtoCfg, strTmpKey, "%s", jnProtoNewKey);
        //pbuff[20...23] 密钥变更时间
        ultmpNetSeq.ucVal[0] = pbuff[ucOffset++];
        ultmpNetSeq.ucVal[1] = pbuff[ucOffset++];
        ultmpNetSeq.ucVal[2] = pbuff[ucOffset++];
        ultmpNetSeq.ucVal[3] = pbuff[ucOffset++];
        ultmpTim = ntohl(ultmpNetSeq.ulVal);
        set_errcode_time = cfg_set_uint32(pathProtoCfg, &ultmpTim, jnProtoNewKeyChangeTime);

        if(set_errcode_key == ERR_NO &&
           set_errcode_time == ERR_NO)
        {
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_NO;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0, 1);
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_FILE_RW;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1);
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
    ErrorCode_t set_errcode;
    ErrorCode_t errcode;
    uint8_t id_cont;
    uint8_t id;//本地枪ID,从0开始
    uint8_t len_qr;
    char qrcode[64] = {0};
    uint8_t ucOffset;
    int i,j;

    set_errcode = ERR_NO;
    ucOffset = 0;
    errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_QR, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
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

            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1);
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
            cfg_set_string(pathEVSECfg, qrcode, "%s:%d.%s", jnCONArray, id, jnQRCode);
        }

        //pbuff[0...3] 操作ID
        if(set_errcode == ERR_NO)
        {
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_NO;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0, 1);
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_REMOTE_PARAM;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1);
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
    int set_errcode;
    ErrorCode_t errcode;
    us2uc ustmpNetSeq;
    uint16_t usListCont;
    int i,j;
    uint8_t ucOffset = 0;
    char strID[16+1] = {0};
    char path[64];

    errcode = RemoteRecvHandle(pProto, usCmdID, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        if (usCmdID == ECH_CMDID_SET_BLACK)
        {
            strcpy(path, pathBlackList);
        }
        else if (usCmdID == ECH_CMDID_SET_WHITE)
        {
            strcpy(path, pathWhiteList);
        }
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
            pProto->sendCommand(pProto, pEVSE, NULL, usCmdID, 0, 1);
        }
        else
        {
            errcode = ERR_FILE_RW;
            //[0...3]操作ID
            //[4] 设置结果
            pProto->pCMD[usCmdID]->ucRecvdOptData[4] = 2;
            pProto->sendCommand(pProto, pEVSE, NULL, usCmdID, 0, 1);
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
    int set_errcode;
    ErrorCode_t errcode;
    us2uc ustmpNetSeq;
    uint16_t usListCont;
    int i,j,k;          //i:名单数  j:名单字节长度  k:名单种类
    uint8_t ucOffset = 0;
    char strID[16+1] = {0};
    char path[64];

    errcode = RemoteRecvHandle(pProto, usCmdID, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
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
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0, 1);
        }
        else
        {
            //pbuff[0...3] 操作ID
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_REMOTE_PARAM;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1);
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}
static ErrorCode_t RemoteIF_RecvReqCmdid(uint16_t usCmdID, EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal )
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t errcode;

    errcode = RemoteRecvHandle(pProto, usCmdID, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        pProto->sendCommand(pProto, pEVSE, NULL, usCmdID, 0, 1);
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}
ErrorCode_t RemoteIF_RecvReqCmdidWithCON(uint16_t usCmdID, EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, int *psiRetVal)
{
    uint8_t pbuff[1024] = { 0 };
    uint32_t len;
    ErrorCode_t errcode;

    errcode = RemoteRecvHandleWithCON(pProto, usCmdID, pCON->info.ucCONID, 4, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        pProto->sendCommand(pProto, pEVSE, pCON, usCmdID, 0, 1);
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;  
}
ErrorCode_t RemoteIF_RecvReq(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal)
{
    CON_t *pCON;
    int i;
    int res;
    ErrorCode_t errcode;

    RemoteIF_RecvReqCmdid(ECH_CMDID_REQ_ENERGYFEE, pEVSE, pProto, &res);
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
    
    for (i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        RemoteIF_RecvReqCmdidWithCON(ECH_CMDID_REQ_POWER, pEVSE, pProto, pCON, &res);
    }
    return errcode;
}

ErrorCode_t RemoteIF_SendCardStart(EVSE_t *pEVSE, echProtocol_t *pProto, RFIDDev_t *pRfid)
{
    ull2uc ullTmpNetOrderSN;
    ErrorCode_t errcode = ERR_NO;

    if (pProto->info.BnWIsListCfg(pathBlackList, pRfid->order.strCardID) == 1)
    {
        pRfid->order.ucAccountStatus = 0;
        pRfid->order.dBalance = 0;

        return ERR_BLACK_LIST;
    }
#if EVSE_USING_NET
    else if(pProto->info.BnWIsListCfg(pathWhiteList, pRfid->order.strCardID) == 1)
#else
    else if(1)
#endif
    {
        pRfid->order.ucAccountStatus = 1;
        pRfid->order.dBalance = 9999.99;

        ullTmpNetOrderSN.ullVal = htonll(time(NULL));   // 采用时间戳作为交易流水号

        //保存流水号到order
        pRfid->order.ullOrderSN = ntohll(ullTmpNetOrderSN.ullVal);

        return ERR_WHITE_LIST;
    }

    pProto->sendCommand(pProto, pEVSE, pRfid, ECH_CMDID_CARD_START, 10, 3); //注意传的参数是 pRfid
    return errcode;
}
ErrorCode_t RemoteIF_SendCardStartPwd(EVSE_t *pEVSE, echProtocol_t *pProto, RFIDDev_t *pRfid)
{
    ErrorCode_t errcode = ERR_NO;
    pProto->sendCommand(pProto, pEVSE, pRfid, ECH_CMDID_CARD_START_PWD, 10, 3); //注意传的参数是 pRfid
    return errcode;
}
ErrorCode_t RemoteIF_RecvCardStart(echProtocol_t *pProto, uint16_t usCmdID, RFIDDev_t *pRfid, uint8_t *pucVaild, int *psiRetVal)
{
    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t errcode;
    uint8_t con_id;
    char strCardID[17] = {0};
    ul2uc ultmpNetSeq;
    ull2uc ullOrderSN;
    uint8_t ucOffset;
    int i;

    errcode = RemoteRecvHandleWithCON(pProto, usCmdID, pRfid->order.ucCONID, 0, pbuff, &len);
    switch (errcode)
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
        if (strcmp(strCardID, pRfid->order.strCardID) != 0)
        {
            *psiRetVal = 0;
            break;
        }
        //pbuff[17...24] 流水号
        for(i = 0; i < 8; i++)
        {
            ullOrderSN.ucArray[i] = pbuff[ucOffset++];
        }
        if(pRfid->order.ullOrderSN != ntohll(ullOrderSN.ullVal))
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
ErrorCode_t RemoteIF_RecvCardStartRes(echProtocol_t *pProto, CON_t *pCON, int *psiRetVal)
{
    CON_t *pCONtmp;
    uint8_t id;

    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t errcode;

    id = 0;
    errcode = ERR_NO;
    errcode = RemoteRecvHandleWithCON(pProto, ECH_CMDID_CARD_START_RES, pCON->info.ucCONID, 0, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        //pbuff[0] 充电桩接口
        id = EchRemoteIDtoCONID(pbuff[0]);
        pCONtmp = CONGetHandle(id);
        if (pCONtmp != NULL)
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
ErrorCode_t RemoteIF_RecvCardStopRes(echProtocol_t *pProto, CON_t *pCON, int *psiRetVal)
{
    CON_t *pCONtmp;
    uint8_t id;

    uint8_t pbuff[1024] = {0};
    uint32_t len;
    ErrorCode_t errcode;

    id = 0;
    errcode = ERR_NO;
    errcode = RemoteRecvHandleWithCON(pProto, ECH_CMDID_CARD_STOP_RES, pCON->info.ucCONID, 0, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        //pbuff[0] 充电桩接口
        id = EchRemoteIDtoCONID(pbuff[0]);
        pCONtmp = CONGetHandle(id);
        if (pCONtmp != NULL)
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

    //[0]:0 1-1 绝缘故障
	if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_PE) == defSignalEVSE_Alarm_PE)
    {
        SET_BIT(pProto->status.fault[0], BIT_0);
    }
    else
    {
        CLEAR_BIT(pProto->status.fault[0], BIT_0);
    }
    //[0]:1 1-2 输出连接器过温故障
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
	    if (((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_Temp_Cri) == defSignalCON_Alarm_AC_A_Temp_Cri) ||
		    ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_N_Temp_Cri) == defSignalCON_Alarm_AC_N_Temp_Cri) )
	    {
            SET_BIT(pProto->status.fault[0], BIT_1);
            break;//有一个有故障就退出
	    }
        else
        {
            CLEAR_BIT(pProto->status.fault[0], BIT_1);
        }
    }
    //[0]:6 1-7 电流过大
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        uxBit = xEventGroupGetBits(pCON->status.xHandleEventCharge);
	    if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_CurrUp_Cri) == defSignalCON_Alarm_AC_A_CurrUp_Cri)
	    {
            SET_BIT(pProto->status.fault[0], BIT_6);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.fault[0], BIT_6);
        }
    }
    //[0]:7 1-8 电压异常
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
	    if (((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltUp) == defSignalCON_Alarm_AC_A_VoltUp) ||
	        ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltLow) == defSignalCON_Alarm_AC_A_VoltLow))
        {
            SET_BIT(pProto->status.fault[0], BIT_7);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.fault[0], BIT_7);
        }
    }
    //[1]:2  2-3 电能表故障
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
	    if ((pCON->status.ulSignalFault & defSignalCON_Fault_Meter) == defSignalCON_Fault_Meter)
        {
            SET_BIT(pProto->status.fault[1], BIT_2);
            break;//有一个有故障就退出
        }
        else
        {
            CLEAR_BIT(pProto->status.fault[1], BIT_2);
        }
    }
    //[1]:5 2-6 PWM切换故障
//    for(i = 0; i < ulTotalCON; i++)
//    {
//        pCON = CONGetHandle(i);
//	    if ((pCON->status.ulSignalFault & defSignalCON_Fault_CP) == defSignalCON_Fault_CP)
//        {
//            SET_BIT(pProto->status.fault[1], BIT_5);
//            break;//有一个有故障就退出
//        }
//        else
//        {
//            CLEAR_BIT(pProto->status.fault[1], BIT_5);
//        }
//    }
    //[1]:6 2-7 温度传感器故障
    for(i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
	    if (((pCON->status.ulSignalFault & defSignalCON_Fault_AC_A_Temp) == defSignalCON_Fault_AC_A_Temp) ||
	        ((pCON->status.ulSignalFault & defSignalCON_Fault_AC_N_Temp) == defSignalCON_Fault_AC_N_Temp))
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
	if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_Scram) == defSignalEVSE_Alarm_Scram)
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
	    
	    if (((pCON->status.ulSignalAlarm & defSignalCON_Alarm_SocketTemp1_Cri) == defSignalCON_Alarm_SocketTemp1_Cri) ||
			((pCON->status.ulSignalAlarm & defSignalCON_Alarm_SocketTemp2_Cri) == defSignalCON_Alarm_SocketTemp2_Cri))
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
        if ((pCON->status.ulSignalFault & defSignalCON_Fault_RelayPaste) == defSignalCON_Fault_RelayPaste)
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
        pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_UP_FAULT, 0, 1);
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
	if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_Arrester) == defSignalEVSE_Alarm_Arrester)
    {
        SET_BIT(pProto->status.warning[0], BIT_0);
    }
    else
    {
        CLEAR_BIT(pProto->status.warning[0], BIT_0);
    }
    //[1]:1 2-2 一般漏电
	if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_PE) == defSignalEVSE_Alarm_PE)
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
	    if (((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltUp) == defSignalCON_Alarm_AC_A_VoltUp) ||
			((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltLow) == defSignalCON_Alarm_AC_A_VoltLow))
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
	    if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_Freq_Cri) == defSignalCON_Alarm_AC_Freq_Cri)
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
	    if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_CurrUp_War) == defSignalCON_Alarm_AC_A_CurrUp_War)
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
	    if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_CurrUp_Cri) == defSignalCON_Alarm_AC_A_CurrUp_Cri)
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
	    if (((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltUp) == defSignalCON_Alarm_AC_A_VoltUp) ||
			((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltLow) == defSignalCON_Alarm_AC_A_VoltLow))
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
	if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_PE) == defSignalEVSE_Alarm_PE)
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
        pbuff = pProto->pCMD[ECH_CMDID_UP_WARNING]->ucRecvdOptData;
        memcpy(pbuff, pProto->status.warning, 6);
        memcpy(&pbuff[6], pProto->status.protect, 6);
        pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_UP_WARNING, 0, 1);
    }

    return ERR_NO;
}

ErrorCode_t RemoteIF_RecvSetOTA(echProtocol_t *pProto, int *psiRetVal)
{
    uint8_t pbuff[1024] = { 0 };
    uint32_t len;
    uint32_t ftp_len = 0;
    int i, j;
    uint8_t ucOffset = 0;
    ErrorCode_t errcode;
    ErrorCode_t errcode_ser, errcode_usr, errcode_pass, errcode_ver, errcode_fil;

    errcode = ERR_NO;
    errcode = RemoteRecvHandle(pProto, ECH_CMDID_SET_OTA, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        //pbuff[0...3] 操作ID
        ucOffset = 4;
        EchFtpInit(&pProto->info.ftp);
        //软件版本号
        for (i = 0; i < 10; i++)
        {
            pProto->info.ftp.strNewVersion[i] = pbuff[ucOffset++]; 
        }
        //ftp地址长度
        ftp_len = pbuff[ucOffset++];
        //ftp地址
        for (i = 0; i < ftp_len; i++)
        {
            pProto->info.ftp.strServer[i] = pbuff[ucOffset++];
        }
        //登录ftp账号
        for (i = 0; i < 8; i++)
        {
            pProto->info.ftp.strUser[i] = pbuff[ucOffset++];
        }
        //登录ftp密码
        for (i = 0; i < 8; i++)
        {
            pProto->info.ftp.strPassword[i] = pbuff[ucOffset++];
        }
        //软件包名称
        for (i = 0; i < 10; i++)
        {
            pProto->info.ftp.strNewFileName[i] = pbuff[ucOffset++];
        }
        //存储FTP数据
        errcode_ver = cfg_set_string(pathFTPCfg, pProto->info.ftp.strNewVersion, "%s", jnFtpNewVersion);
        errcode_ser = cfg_set_string(pathFTPCfg, pProto->info.ftp.strServer, "%s", jnFtpServer);
        errcode_usr = cfg_set_string(pathFTPCfg, pProto->info.ftp.strUser, "%s", jnFtpUsername);
        errcode_pass = cfg_set_string(pathFTPCfg, pProto->info.ftp.strPassword, "%s", jnFtpPassword);
        errcode_fil = cfg_set_string(pathFTPCfg, pProto->info.ftp.strNewFileName, "%s", jnFtpNewFilename);
        if (errcode_ver == ERR_NO &&
            errcode_ser == ERR_NO &&
            errcode_usr == ERR_NO &&
            errcode_pass == ERR_NO &&
            errcode_fil == ERR_NO)
        {
            *psiRetVal = 1;
            errcode = ERR_NO;
            pProto->info.ftp.GetFtpCfg((void *)&(pechProto->info.ftp), NULL);
        }
        else
        {
            *psiRetVal = 1;
            errcode = ERR_FILE_RW;
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_SendSetOTA(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t succ)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_SET_OTA]->ucRecvdOptData;
    if (succ == 1)
    {
        pbuff[4] = 1;
    }
    else if (succ == 0)
    {
        pbuff[4] = 2;
    }

    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_SET_OTA, 0, 1);

    return errcode;
}

ErrorCode_t RemoteIF_RecvReqOTA_DW(echProtocol_t *pProto, int *psiRetVal)
{
    uint8_t pbuff[1024] = { 0 };
    uint32_t len;
    uint8_t ucOffset = 0;
    int i;
    char tmpVersion[10 + 1] = {0};
    ErrorCode_t errcode;

    errcode = ERR_NO;
    errcode = RemoteRecvHandle(pProto, ECH_CMDID_REQ_OTA_DW, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        //pbuff[0...3] 操作ID
        ucOffset = 4;
        //软件版本号
        for (i = 0; i < 10; i++)
        {
            tmpVersion[i] = pbuff[ucOffset++]; 
        }
        if (strcmp(tmpVersion, pProto->info.ftp.strNewVersion) == 0)
        {
            errcode = ERR_NO;
        }
        else
        {
            errcode = ERR_REMOTE_PARAM;
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_SendReqOTA_DW(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON)
{
    uint8_t *pbuff;
    uint8_t download_status;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_REQ_OTA_DW]->ucRecvdOptData;

    pProto->info.ftp.GetFtpCfg((void *)&(pProto->info.ftp), NULL);
    pbuff[4] = pProto->info.ftp.ucDownloadStatus;

    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_REQ_OTA_DW, 0, 1);

    return errcode;
}

ErrorCode_t RemoteIF_RecvOTA_Start(echProtocol_t *pProto, int *psiRetVal)
{
    uint8_t pbuff[1024] = { 0 };
    uint32_t len;
    uint8_t ucOffset = 0;
    int i;
    char tmpVersion[10 + 1] = { 0 };
    ErrorCode_t errcode;

    errcode = ERR_NO;
    errcode = RemoteRecvHandle(pProto, ECH_CMDID_OTA_START, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        ucOffset = 0;
        //软件版本号
        for (i = 0; i < 10; i++)
        {
            tmpVersion[i] = pbuff[ucOffset++]; 
        }
        if (strcmp(tmpVersion, pProto->info.ftp.strNewVersion) == 0)
        {
            errcode = ERR_NO;
        }
        else
        {
            errcode = ERR_REMOTE_PARAM;
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_SendOTA_Start(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_OTA_START, 20, 3);

    return errcode;
}

ErrorCode_t RemoteIF_RecvOTA_Result(echProtocol_t *pProto, int *psiRetVal)
{
    uint8_t pbuff[1024] = { 0 };
    uint32_t len;
    uint8_t ucOffset = 0;
    int i;
    char tmpVersion[10 + 1] = { 0 };
    ErrorCode_t errcode;

    errcode = ERR_NO;
    errcode = RemoteRecvHandle(pProto, ECH_CMDID_OTA_RESULT, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        ucOffset = 0;
        //软件版本号
        for (i = 0; i < 10; i++)
        {
            tmpVersion[i] = pbuff[ucOffset++]; 
        }
        if (strcmp(tmpVersion, pProto->info.ftp.strNewVersion) == 0)
        {
            errcode = ERR_NO;
        }
        else
        {
            errcode = ERR_REMOTE_PARAM;
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_SendOTA_Result(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, int succ)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[ECH_CMDID_OTA_RESULT]->ucRecvdOptData;

    pbuff[20] = succ;

    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_OTA_RESULT, 20, 3);

    return errcode;
}

ErrorCode_t RemoteIF_RecvEmergencyStop(echProtocol_t *pProto, uint8_t con_id, int *psiRetVal)
{
    CON_t *pCON = NULL;
    uint8_t id = 0;
    uint8_t id_pos = 4;

    uint8_t pbuff[1024] = { 0 };
    uint32_t len = 0;
    ErrorCode_t errcode = ERR_NO;

    errcode = RemoteRecvHandleWithCON(pProto, ECH_CMDID_EMERGENCY_STOP, con_id, id_pos, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        //pbuff[0...3] 操作ID ，不处理，留在ucRecvdOptData中待回复时使用
        //pbuff[4] 充电桩接口, id_pos
        id = EchRemoteIDtoCONID(pbuff[id_pos]);
        pCON = CONGetHandle(id);
        if (pCON != NULL)
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

ErrorCode_t RemoteIF_SendEmergencyStop(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t succ)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    /*** 如下操作为破坏了程序结构，用pCMD中的缓存空间带入一些需要传递的参数*/
    /*** 2017年6月16日：现在pCMD的缓存专用于接收与传递参数，程序结构上不用担心了*/
    pbuff = pProto->pCMD[ECH_CMDID_EMERGENCY_STOP]->ucRecvdOptData;
    if (succ == 1)
    {
        pbuff[5] = 0;
    }
    else if (succ == 0)
    {
        pbuff[5] = 1;
    }
    /*********************/
    pProto->sendCommand(pProto, pEVSE, pCON, ECH_CMDID_EMERGENCY_STOP, 0, 1);

    return errcode;
}

ErrorCode_t RemoteIF_RecvSetPower(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t con_id, int *psiRetVal)
{
    CON_t *pCON = NULL;
    uint8_t id = 0;
    uint8_t id_pos = 4;
    uint8_t pbuff[1024] = { 0 };
    uint32_t len;
    ErrorCode_t set_errcode_power;
    ErrorCode_t errcode;
    ul2uc ultmpPower_kw;
    double dtmpPower_kw;

    errcode = RemoteRecvHandleWithCON(pProto, ECH_CMDID_SET_POWER, con_id, id_pos, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        *psiRetVal = 1;
        //pbuff[0...3] 操作ID
        //pbuff[4] 充电桩接口
        id = EchRemoteIDtoCONID(pbuff[id_pos]);
        pCON = CONGetHandle(id);
        if (pCON == NULL)
        {
            errcode = ERR_REMOTE_PARAM;
            break;
        }

        //pbuff[5...8] 充电桩接口功率 单位:W
        ultmpPower_kw.ucVal[0] = pbuff[5];
        ultmpPower_kw.ucVal[1] = pbuff[6];
        ultmpPower_kw.ucVal[2] = pbuff[7];
        ultmpPower_kw.ucVal[3] = pbuff[8];
        dtmpPower_kw = (double)ntohl(ultmpPower_kw.ulVal) / 1000.0;
        if (pEVSE->info.ucPhaseLine < 3)
        {
            if (dtmpPower_kw <= 7.04)//220*32
            {
                set_errcode_power = cfg_set_double(pathEVSECfg, &dtmpPower_kw, "%s:%d.%s", jnCONArray, con_id, jnRatedPower);
            }
            else
            {
                set_errcode_power = ERR_REMOTE_PARAM;
            }
        }
        else//phase = 3
        {
            if (dtmpPower_kw <= 41.58)//220*63*3
            {
                set_errcode_power = cfg_set_double(pathEVSECfg, &dtmpPower_kw, "%s:%d.%s", jnCONArray, con_id, jnRatedPower);
            }
            else
            {
                set_errcode_power = ERR_REMOTE_PARAM;
            }
        }
            
        //pbuff[0...3] 操作ID
        if(set_errcode_power == ERR_NO)
        {
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_SUCC]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_NO;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_SUCC, 0, 1);
        }
        else
        {
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[0] = pbuff[0];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[1] = pbuff[1];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[2] = pbuff[2];
            pProto->pCMD[ECH_CMDID_SET_FAIL]->ucRecvdOptData[3] = pbuff[3];
            errcode = ERR_FILE_RW;
            pProto->sendCommand(pProto, pEVSE, NULL, ECH_CMDID_SET_FAIL, 0, 1);
        }
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_RecvSetAppoint(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t con_id, int *psiRetVal)
{
    CON_t *pCON = NULL;
    uint8_t id = 0;
    uint8_t id_pos = 4;
    uint8_t pbuff[1024] = { 0 };
    uint32_t len;
    ErrorCode_t errcode;
    ul2uc ultmpTime_s;

    errcode = RemoteRecvHandleWithCON(pProto, ECH_CMDID_SET_APPOINT, con_id, id_pos, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        //pbuff[0...3] 操作ID
        //pbuff[4] 充电桩接口
        id = EchRemoteIDtoCONID(pbuff[id_pos]);
        pCON = CONGetHandle(id);
        if (pCON == NULL)
        {
            *psiRetVal = 0;
            break;
        }
        //pbuff[5] 预约操作
        //  0：未知
        //  1：无预约
        //  2：已预约
        //  3：预约失败
        if (pCON->appoint.status > 1)
        {
            *psiRetVal = 0;
            break;
        }
        pCON->appoint.status = pbuff[5];
        //pbuff[6...9] 预约时长 单位:S
        ultmpTime_s.ucVal[0] = pbuff[6];
        ultmpTime_s.ucVal[1] = pbuff[7];
        ultmpTime_s.ucVal[2] = pbuff[8];
        ultmpTime_s.ucVal[3] = pbuff[9];
        pCON->appoint.time_s = ntohl(ultmpTime_s.ulVal);
        //预约时间点
        pCON->appoint.timestamp = time(NULL);
        *psiRetVal = 1;
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_RecvReqAppoint(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t con_id, int *psiRetVal)
{
    CON_t *pCON = NULL;
    uint8_t id = 0;
    uint8_t id_pos = 4;
    uint8_t pbuff[1024] = { 0 };
    uint32_t len;
    ErrorCode_t errcode;

    errcode = RemoteRecvHandleWithCON(pProto, ECH_CMDID_REQ_APPOINT, con_id, id_pos, pbuff, &len);
    switch (errcode)
    {
    case ERR_REMOTE_NODATA:
        *psiRetVal = 0;
        break;
    case ERR_NO:
        //pbuff[0...3] 操作ID
        //pbuff[4] 充电桩接口
        id = EchRemoteIDtoCONID(pbuff[id_pos]);
        pCON = CONGetHandle(id);
        if (pCON == NULL)
        {
            *psiRetVal = 0;
            break;
        }
        *psiRetVal = 1;
        break;
    default:
        *psiRetVal = 0;
        break;
    }

    return errcode;
}

ErrorCode_t RemoteIF_SendAppoint(uint16_t usCmdID, EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, int status)
{
    uint8_t *pbuff;
    ErrorCode_t errcode;
    errcode = ERR_NO;

    pbuff = pProto->pCMD[usCmdID]->ucRecvdOptData;
    //[0...3] 操作序列号
    //[4] 充电桩接口
    //[5] 预约状态
    pbuff[5] = status;
    //[6...9] 预约剩余时间
    /*********************/
    pProto->sendCommand(pProto, pEVSE, pCON, usCmdID, 0, 1);

    return errcode;
}
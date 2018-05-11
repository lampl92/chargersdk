/**
* @file ech_protocol.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-22
*/
#include "utils.h"
#include "interface.h"
#include "libEcharge/ech_encrypt.h"
#include "libEcharge/ech_protocol_proc.h"
#include "libEcharge/ech_globals.h"

#include <string.h>
#include "stringName.h"
#include "cfg_parse.h"
#include "cJSON.h"
#include "sysinit.h"
#include "evse_debug.h"


/*---------------------------------------------------------------------------*/
/*                               获取协议配置信息                            */
/*---------------------------------------------------------------------------*/

static void cfgobj_get_period(cJSON *jsCfgObj, EchSegTime_t *pSegTime, uint8_t *strCfgObjName)
{
    int i;
    cfgobj_get_uint8(jsCfgObj, &pSegTime->ucPeriodCont, "%s.%s", strCfgObjName, jnProtoSegCont);
    for (i = 0; i < pSegTime->ucPeriodCont; i++)
    {
        cfgobj_get_uint8(jsCfgObj, &pSegTime->ucStart[i], "%s.Start%d", strCfgObjName, i + 1);
        cfgobj_get_uint8(jsCfgObj, &pSegTime->ucEnd[i], "%s.End%d", strCfgObjName, i + 1);
    }
}

/** @brief 获取protocol.cfg全部参数
 *
 * @param pvProto void*
 * @param pvCfgObj void*
 * @return ErrorCode_t
 *
 */
static ErrorCode_t GetProtoCfg(void *pvProto, void *pvCfgObj)
{
    cJSON *jsCfgObj = NULL;
    ErrorCode_t errcode = ERR_NO;
    echProtocol_t *pProto;
    int i;

    pProto = (echProtocol_t *)pvProto;

    /*json解析*/
    if (pvCfgObj == NULL)
    {
        jsCfgObj = GetCfgObj(pathProtoCfg, &errcode);
        if (jsCfgObj == NULL)
        {
            return errcode;
        }
    }
    else
    {
        jsCfgObj = (cJSON *)pvCfgObj;
    }

    cfgobj_get_string(jsCfgObj, pProto->info.strServerIP, "%s", jnProtoServerIP);
    cfgobj_get_uint16(jsCfgObj, &pProto->info.usServerPort, "%s", jnProtoServerPort);
    cfgobj_get_string(jsCfgObj, pProto->info.strUserName, "%s", jnProtoUserName);
    cfgobj_get_string(jsCfgObj, pProto->info.strUserPwd, "%s", jnProtoUserPwd);
    cfgobj_get_string(jsCfgObj, pProto->info.strKey, "%s", jnProtoKey);
    cfgobj_get_string(jsCfgObj, pProto->info.strNewKey, "%s", jnProtoNewKey);
    cfgobj_get_int32(jsCfgObj, (int32_t*)&pProto->info.tNewKeyChangeTime, "%s", jnProtoNewKeyChangeTime);
    cfgobj_get_uint32(jsCfgObj, &pProto->info.ulOptSN, "%s", jnProtoOptSN);
    cfgobj_get_uint8(jsCfgObj, &pProto->info.ucProtoVer, "%s", jnProtoProtoVer);
    cfgobj_get_uint32(jsCfgObj, &pProto->info.ulHeartBeatCyc_ms, "%s", jnProtoHeartBeatCyc_ms);
    cfgobj_get_uint32(jsCfgObj, &pProto->info.ulStatusCyc_ms, "%s", jnProtoStatusCyc_ms);
    cfgobj_get_uint32(jsCfgObj, &pProto->info.ulRTDataCyc_ms, "%s", jnProtoRTDataCyc_ms);
    cfgobj_get_uint8(jsCfgObj, &pProto->info.ucResetAct, "%s", jnProtoResetAct);
    
    cfgobj_get_double(jsCfgObj, &pProto->info.dSegEnergyFee[0], "%s", jnProtoEnergyFee_sharp);
    cfgobj_get_double(jsCfgObj, &pProto->info.dSegEnergyFee[1], "%s", jnProtoEnergyFee_peak);
    cfgobj_get_double(jsCfgObj, &pProto->info.dSegEnergyFee[2], "%s", jnProtoEnergyFee_shoulder);
    cfgobj_get_double(jsCfgObj, &pProto->info.dSegEnergyFee[3], "%s", jnProtoEnergyFee_off_peak);
    
    cfgobj_get_double(jsCfgObj, &pProto->info.dSegServFee[0], "%s", jnProtoServFee_sharp);
    cfgobj_get_double(jsCfgObj, &pProto->info.dSegServFee[1], "%s", jnProtoServFee_peak);
    cfgobj_get_double(jsCfgObj, &pProto->info.dSegServFee[2], "%s", jnProtoServFee_shoulder);
    cfgobj_get_double(jsCfgObj, &pProto->info.dSegServFee[3], "%s", jnProtoServFee_off_peak);

    /*获取尖峰平谷时间段*/
    cfgobj_get_period(jsCfgObj, &pProto->info.SegTime[0], jnProtoSegTime_sharp);
    cfgobj_get_period(jsCfgObj, &pProto->info.SegTime[1], jnProtoSegTime_peak);
    cfgobj_get_period(jsCfgObj, &pProto->info.SegTime[2], jnProtoSegTime_shoulder);
    cfgobj_get_period(jsCfgObj, &pProto->info.SegTime[3], jnProtoSegTime_off_peak);

    cJSON_Delete(jsCfgObj);
    return errcode;
}

/*---------------------------------------------------------------------------/
/                               黑白名单
/---------------------------------------------------------------------------*/
static int BnWIsListCfg(uint8_t *path, uint8_t *strID)
{
    cJSON *jsArrayObj;
    cJSON *jsArrayItem;
    ErrorCode_t errcode;
    uint16_t usTotalList;
    int i;
    int res;

    errcode = ERR_NO;

    /*json解析*/
    jsArrayObj = GetCfgObj(path, &errcode);
    if(jsArrayObj == NULL || errcode != ERR_NO)
    {
        return 0;
    }
    usTotalList = cJSON_GetArraySize(jsArrayObj);
    for(i = 0; i < usTotalList; i++)
    {
        res = 0;
        jsArrayItem = cJSON_GetArrayItem(jsArrayObj, i);
        if(strcmp(jsArrayItem->valuestring, strID) == 0)
        {
            res = 1;
            break;
        }
    }

    cJSON_Delete(jsArrayObj);

    return res;
}
static int BnWGetListSizeCfg(uint8_t *path, uint16_t *size)
{
    cJSON *jsArrayObj;
    cJSON *jsArrayItem;
    ErrorCode_t errcode;
    uint16_t usTotalList;

    errcode = ERR_NO;

    /*json解析*/
    jsArrayObj = GetCfgObj(path, &errcode);
    if(jsArrayObj == NULL || errcode != ERR_NO)
    {
        return 0;
    }
    usTotalList = cJSON_GetArraySize(jsArrayObj);
    *size = usTotalList;

    cJSON_Delete(jsArrayObj);

    return 1;
}
static int BnWGetListCfg(uint8_t *path, uint16_t idx, uint8_t *strID)
{
    cJSON *jsArrayObj;
    cJSON *jsArrayItem;
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /*json解析*/
    jsArrayObj = GetCfgObj(path, &errcode);
    if(jsArrayObj == NULL || errcode != ERR_NO)
    {
        return 0;
    }

    jsArrayItem = cJSON_GetArrayItem(jsArrayObj, idx);
    strcpy(strID, jsArrayItem->valuestring);

    cJSON_Delete(jsArrayObj);

    return 1;
}
/** @brief 设置黑白名单到文件, 会覆盖原有内容
 *
 * @param path uint8_t*
 * @param strID uint8_t*
 * @return int
 *
 */
static int BnWFlushListCfg(uint8_t *path)
{
    cJSON *jsArrayObj;
    ErrorCode_t errcode;
    int i;
    int res = 1;

    errcode = ERR_NO;

    /*json解析*/
    jsArrayObj = cJSON_CreateArray();
    if(jsArrayObj == NULL)
    {
        res = 0;
        return res;
    }

    errcode = SetCfgObj(path, jsArrayObj);
    if(errcode != ERR_NO)
    {
        res = 0;
    }
    return res;
}
/** @brief 增加黑白名单到文件, 不会覆盖
 *
 * @param path uint8_t*
 * @param strID uint8_t*
 * @return int
 *
 */
static int BnWAddListCfg(uint8_t *path, uint8_t *strID)
{
    cJSON *jsArrayObj;
    cJSON *jsArrayItem;
    ErrorCode_t errcode;
    uint16_t usTotalList;
    int i;
    int res = 1;

    errcode = ERR_NO;

    /*json解析*/
    jsArrayObj = GetCfgObj(path, &errcode);
    if(jsArrayObj == NULL || errcode != ERR_NO)
    {
        res = 0;
        return res;
    }
    usTotalList = cJSON_GetArraySize(jsArrayObj);
    for(i = 0; i < usTotalList; i++)
    {
        res = 0;
        jsArrayItem = cJSON_GetArrayItem(jsArrayObj, i);
        if(strcmp(jsArrayItem->valuestring, strID) == 0)
        {
            res = 1;
            return res; //要追加的卡号已经存在于配置中
        }
    }
    cJSON_AddItemToArray(jsArrayObj, cJSON_CreateString(strID));

    errcode = SetCfgObj(path, jsArrayObj);
    if(errcode != ERR_NO)
    {
        res = 0;
    }
    return res;
}

static int BnWDeleteListCfg(uint8_t *path, uint8_t *strID)
{
    cJSON *jsArrayObj;
    cJSON *jsArrayItem;
    ErrorCode_t errcode;
    uint16_t usTotalList;
    int i;
    int res;

    errcode = ERR_NO;

    /*json解析*/
    jsArrayObj = GetCfgObj(path, &errcode);
    if(jsArrayObj == NULL || errcode != ERR_NO)
    {
        res = 0;
        return res;
    }
    usTotalList = cJSON_GetArraySize(jsArrayObj);
    for(i = 0; i < usTotalList; i++)
    {
        res = 0;
        jsArrayItem = cJSON_GetArrayItem(jsArrayObj, i);
        if(strcmp(jsArrayItem->valuestring, strID) == 0)
        {
            res = 1;
            cJSON_DeleteItemFromArray(jsArrayObj, i);
            break;
        }
    }

    errcode = SetCfgObj(path, jsArrayObj);
    if(errcode != ERR_NO)
    {
        res = 0;
    }

    return res;
}

void testBnWList(void)
{
    uint8_t total;
    uint16_t size;
    uint8_t *strID[24] =
    {
        "0000000000000001",
        "0000000000000002",
        "0000000000000003",
        "0000000000000004",
        "0000000000000005",
        "0000000000000006",
        "0000000000000007",
        "0000000000000008",
        "0000000000000009",
        "0000000000000010",
        "0000000000000011",
        "0000000000000012",
        "0000000000000013",
        "0000000000000014",
        "0000000000000015",
        "0000000000000016",
        "0000000000000017",
        "0000000000000018",
        "0000000000000019",
        "0000000000000020",
        "0000000000000021",
        "0000000000000022",
        "0000000000000023",
        "0000000000000024"
    };
    uint8_t strIDCtx[17];
    BnWFlushListCfg(pathBlackList);
    for (int i = 0; i < 20; ++i)
    {
        /* code */
        BnWAddListCfg(pathBlackList, strID[i]);
    }
    for (int i = 0; i < 24; ++i)
    {
        /* code */
        int res;
        res = BnWIsListCfg(pathBlackList, strID[i]);
        if (res == 1)
        {
            /* code */
            printf_safe("%s 在列表中\n", strID[i]);
        }
    }

    for (int i = 0; i < 10; ++i)
    {
        /* code */
        BnWDeleteListCfg(pathBlackList, strID[i]);
    }
    for (int i = 0; i < 24; ++i)
    {
        /* code */
        int res;
        res = BnWIsListCfg(pathBlackList, strID[i]);
        if (res == 1)
        {
            /* code */
            printf_safe("%s 在列表中\n\n", strID[i]);
        }
    }

    BnWGetListSizeCfg(pathBlackList, &size);
    printf_safe("当前列表名单数量 %d\n", size);
    printf_safe("Req List is:\n");
    for(int i = 0; i < 5; i++)
    {
        BnWGetListCfg(pathBlackList, i, strIDCtx);
        printf_safe("%s\n", strIDCtx);
    }
}

/*---------------------------------------------------------------------------*/
/*                              协议解析                                     */
/*---------------------------------------------------------------------------*/
static uint16_t echVerifCheck(uint8_t ver, uint8_t atrri, uint16_t cmd, uint32_t len)
{
    return (uint16_t)(ver + atrri + cmd + len);
}
static int sendCommand(void *pPObj, void *pEObj, void *pCObj, uint16_t usCmdID, uint32_t timeout_s, uint8_t trycountmax)
{
    echProtocol_t *pProto;
    echProtoElem_t echSendCmdElem;
    uint8_t pucSendBuffer[REMOTE_SENDBUFF_MAX];
    uint32_t ulSendLength;
    int res;
    gdsl_element_t gdsl_res = NULL;

    pProto = (echProtocol_t *)pPObj;
    ulSendLength = 0;

    res = pProto->pCMD[usCmdID]->makeProc(pPObj, pEObj, pCObj, pucSendBuffer, &ulSendLength);
    if (res == 0)
    {
        return 0;
    }
    echSendCmdElem.timestamp = time(NULL);
    echSendCmdElem.timeout_s = timeout_s;
    echSendCmdElem.cmd = pProto->pCMD[usCmdID]->CMDType;
    echSendCmdElem.cmd_id = usCmdID;
    echSendCmdElem.len = ulSendLength;
    echSendCmdElem.pbuff = pucSendBuffer;
    echSendCmdElem.status = 0;//0表示未发送  1表示已发送
    echSendCmdElem.trycount = 0;
    echSendCmdElem.trycountmax = trycountmax;

    pProto->pCMD[usCmdID]->ulRecvdOptLen = 0;
    memset(pProto->pCMD[usCmdID]->ucRecvdOptData, 0, REMOTE_RECVDOPTDATA);
    if (xSemaphoreTake(pProto->xMutexProtoSend, 10000) == pdPASS)
    {
        gdsl_res = gdsl_list_insert_tail(pProto->plechSendCmd, (void *)&echSendCmdElem);
        xSemaphoreGive(pProto->xMutexProtoSend);
    }
    
    if (gdsl_res == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
static int makeStdCmd(void *pPObj,
                      void *pEObj,
                      uint16_t usCmdID,
                      uint8_t *pucMsgBodyCtx_dec,
                      uint32_t ulMsgBodyCtxLen_dec,
                      uint8_t *pucSendBuffer,
                      uint32_t *pulSendLength)
{
    echProtocol_t *pProto;
    echCMD_t *pCMD;
    EVSE_t *pE;
    uint8_t ucMsgHead[64];
    uint32_t ulMsgHeadLen;
    uint8_t ucMsgBodyCtx_enc[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_enc;
    uint16_t usCheck;
    uint8_t ucIDLength;
    int i;

    us2uc ustmpNetSeq;
    ul2uc ultmpNetSeq;

    pProto = (echProtocol_t *)pPObj;
    pCMD = pProto->pCMD[usCmdID];
    pE = (EVSE_t *)pEObj;
    ulMsgHeadLen = 0;
    ucIDLength = strlen(pE->info.strID);

    printf_protodetail("Send(dec) 0x%02X[%d]:\n", pCMD->CMDType.usSendCmd, pCMD->CMDType.usSendCmd);
    for (i = 0; i < ulMsgBodyCtxLen_dec; i++)
    {
        printf_protodetail("%02X ", pucMsgBodyCtx_dec[i]);
    }
    printf_protodetail("\n");
    ulMsgBodyCtxLen_enc = ech_aes_encrypt(pucMsgBodyCtx_dec,
                                      ulMsgBodyCtxLen_dec,
                                      pProto->info.strKey,
                                      ucMsgBodyCtx_enc);

    //协议版本
    ucMsgHead[ulMsgHeadLen++] = pProto->info.ucProtoVer;
    //消息包属性
    ucMsgHead[ulMsgHeadLen++] = 0x00;
    //命令字
    ustmpNetSeq.usVal = htons(pCMD->CMDType.usSendCmd);
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[1];
    //预留字段
    ultmpNetSeq.ulVal = 0;
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[1];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[2];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[3];
    //消息体长度
    ultmpNetSeq.ulVal = htonl(ucIDLength + ulMsgBodyCtxLen_enc);
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[1];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[2];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[3];
    //消息校验码
    ustmpNetSeq.usVal = htons(echVerifCheck(pProto->info.ucProtoVer,
                                            0,
                                            pCMD->CMDType.usSendCmd,
                                            ucIDLength + ulMsgBodyCtxLen_enc));
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[1];

    for(i = 0; i < ulMsgHeadLen; i++)
    {
        pucSendBuffer[i] = ucMsgHead[i];
    }
    for(i = 0; i < ucIDLength; i++)  //此处借用ulMsgHeadLen表示pucSendBuffer位置，运行之后ulMsgHeadLen表示的就是原ulMsgHeadLen + 桩号长度
    {
        pucSendBuffer[ulMsgHeadLen++] = pE->info.strID[i];
    }
    for(i = 0; i < ulMsgBodyCtxLen_enc; i++)
    {
        pucSendBuffer[ulMsgHeadLen + i] = ucMsgBodyCtx_enc[i];
    }

    *pulSendLength = ulMsgHeadLen + ulMsgBodyCtxLen_enc; //此处ulMsgHeadLen已经包含了桩号长度

    return 1;
}
static int makeCmdRegBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint32_t ulMsgBodyCtxLen_dec;
    int i;
    ul2uc ultmpNetSeq;

    pProto = (echProtocol_t *)pPObj;
    ulMsgBodyCtxLen_dec = 0;

    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    for(i = 0; i < strlen(pProto->info.strUserName); i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.strUserName[i];
    }
    for(i = 0; i < strlen(pProto->info.strUserPwd); i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.strUserPwd[i];
    }
    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec;

    return 1;
}
static int makeCmdReg(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdRegBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REGISTER, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);

    return 1;
}

static int makeCmdHeartBodyCtx(uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    uint32_t ulMsgBodyCtxLen_dec;

    ul2uc ultmpNetSeq;

    ulMsgBodyCtxLen_dec = 0;

    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 1;
}
static int makeCmdHeart(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdHeartBodyCtx(ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_HEARTBEAT, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;
}

static int makeCmdResetBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_RESET]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    for(i = 0; i < 4; i++)
    {
        //[0...3] 操作ID
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[i];
    }
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[4]; //成功标志

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 1;
}
static int makeCmdReset(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdResetBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_RESET, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;
}

static int makeCmdStatusBodyCtx(void *pEObj, void *pCObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    EVSE_t *pEVSE;
    CON_t *pCON;
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    uint8_t errcode;
    EventBits_t uxBits;

    pCON = (CON_t *)pCObj;
    pEVSE = (EVSE_t *)pEObj;

    ulMsgBodyCtxLen_dec = 0;
    errcode = 0;

    //充电桩接口   0：默认 1：A 2：B
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.ucCONID + 1;
    //预约状态 1：无预约  2:有预约
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
    //车位状态 1：空闲   2：占用   3：未知
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 3;
    //接口连接状态  1 空闲， 2,车连接 3 未知
    if ((pCON->status.ulSignalState & defSignalCON_State_Plug) == defSignalCON_State_Plug)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 2;
    }
    else
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
    }
    //接口工作状态 1：充电 2:待机 3：故障 4：充电结束 5：未知
    if ((pCON->status.ulSignalState & defSignalCON_State_Working) == defSignalCON_State_Working)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
    }
    else if ((pCON->status.ulSignalState & defSignalCON_State_Standby) == defSignalCON_State_Standby)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 2;
    }
    else if ((pCON->status.ulSignalState & defSignalCON_State_Fault) == defSignalCON_State_Fault)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 3;
    }
    else if ((pCON->status.ulSignalState & defSignalCON_State_Stopping) == defSignalCON_State_Stopping)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 4;
    }
    else
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 5;
    }
    //输出电压xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingVoltage * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //A B C 相电压
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->line[defLineA].status.dVolt * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->line[defLineB].status.dVolt * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->line[defLineC].status.dVolt * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //输出电流xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingCurrent * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //A B C 相电流
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->line[defLineA].status.dCurr * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->line[defLineB].status.dCurr * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->line[defLineC].status.dCurr * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //继电器状态 1开，2关
    if(pCON->status.ucRelayLState == SWITCH_ON &&
            pCON->status.ucRelayNState == SWITCH_ON)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
    }
    else if(pCON->status.ucRelayLState == SWITCH_OFF &&
            pCON->status.ucRelayNState == SWITCH_OFF)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 2;
    }
    //系统风机工作状态 1：开机 2：关机
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //桩实时温度 xx.xxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dACLTemp * 1000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //有功功率 无功功率 xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingPower * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //电能表有功电能 xx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingEnergy * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //电表无功电能 xx.xx
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //SOC 1~100
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //故障码
    if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_Scram) == defSignalEVSE_Alarm_Scram)
    {
        errcode |= 1 << 0; //Bit0 急停故障
    }
    if ((pCON->status.ulSignalFault & defSignalCON_Fault_Meter) == defSignalCON_Fault_Meter)
    {
        errcode |= 1 << 1; //Bit1 电表故障
    }
    if ((pCON->status.ulSignalFault & defSignalCON_Fault_RelayPaste) == defSignalCON_Fault_RelayPaste)
    {
        errcode |= 1 << 2; //Bit2 接触器故障
    }
    if ((pEVSE->status.ulSignalFault & defSignalEVSE_Fault_RFID) == defSignalEVSE_Fault_RFID)
    {
        errcode |= 1 << 3; //Bit3 读卡器故障
    }
    if((pEVSE->status.ulSignalAlarm & defSignalGroupEVSE_Alarm_Temp_Cri) != 0 || 
       (pCON->status.ulSignalAlarm & defSignalGroupCON_Alarm_Temp_Cri) != 0)
    {
        errcode |= 1 << 4; //Bit4 内部过温故障
    }
    //errcode |= 1 << 5 //Bit5 连接器故障
    if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_PE) == defSignalEVSE_Alarm_PE)
    {
        errcode |= 1 << 6; //Bit5 绝缘故障
    }
    if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_PowerOff) == defSignalEVSE_Alarm_PowerOff)
    {
        errcode |= 1 << 7;  //Bit7 其他(在此定义为停电故障)
    }
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = errcode;
    //充电桩当前时间
    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 1;
}
static int makeCmdStatus(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdStatusBodyCtx(pEObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_STATUS, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}

static int makeCmdRemoteCtrlBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_REMOTE_CTRL]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    for(i = 0; i < 14; i++)
    {
        //[0...3] 操作ID
        //[4...11] 交易流水号
        //[12] 充电桩接口
        //[13] 充电桩操作
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[i]; //不变
    }
    //[14] 启停结果
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[14];//在发送命令之前赋值的
    //[15] 失败原因
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[15];//在发送命令之前赋值的
    //[16] SOC
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 1;
}
static int makeCmdRemoteCtrl(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdRemoteCtrlBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REMOTE_CTRL, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdRTDataBodyCtx(void *pPObj, void *pCObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    CON_t *pCON;
    uint8_t *pbuff;
    uint8_t ucOrderSN[8] = {0};
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    us2uc ustmpNetSeq;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pCON = (CON_t *)pCObj;
    pbuff = pProto->pCMD[ECH_CMDID_RTDATA]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0...7] 交易流水号
    StrToHex(pCON->order.strOrderSN, ucOrderSN, strlen(pCON->order.strOrderSN));
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    //[8] 桩接口
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.ucCONID + 1;
    //[9...12] 当前充电总电量 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalEnergy * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[13...16] 尖电量 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dSegTotalEnergy[0] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[17...20] 峰电量
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dSegTotalEnergy[1] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[21...24] 平电量
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dSegTotalEnergy[3] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[25...28] 谷电量
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dSegTotalEnergy[3] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[29...32] 当前充电金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalEnergyFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[32...36] 当前服务费金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[37,38] 当前充电时间
    ustmpNetSeq.usVal = htons(  time(NULL) - pCON->order.tStartTime  );
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[39] 充电桩状态
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[39];//注意修改这里也要修改interface_remote.c对应位置
    //[40] 停止原因
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[40];
    //[41] 当前SOC
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[42,43] 剩余充电时间
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[44...47] 输出电压 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingVoltage * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[48...51] 输出电流 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingCurrent * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[52...55] 电池组最低温度
    //[56...59] 电池组最高温度
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }
    //[60...63] 当前时间戳
    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[64...80] 车辆VIN号
    for(i = 0; i < 17; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 1;
}
static int makeCmdRTData(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdRTDataBodyCtx(pPObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_RTDATA, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}

static int makeCmdCardRTDataBodyCtx(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    EVSE_t *pEVSE;
    CON_t *pCON;
    uint8_t *pbuff;
    uint8_t ucOrderSN[8] = {0};
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    us2uc ustmpNetSeq;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pEVSE = (EVSE_t *)pEObj;
    pCON = (CON_t *)pCObj;
    pbuff = pProto->pCMD[ECH_CMDID_CARD_RTDATA]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0] 充电桩接口
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = EchCONIDtoRemoteID(pCON->order.ucCONID, pEVSE->info.ucTotalCON);
    //[1...8] 交易流水号
    StrToHex(pCON->order.strOrderSN, ucOrderSN, strlen(pCON->order.strOrderSN));
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    //[9...24] 卡号
    for(i = 0; i < 16; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->order.strCardID[i];
    }
    //[25...28] 当前充电总电量 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalEnergy * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[29...32] 尖电量 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dSegTotalEnergy[0] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[33...36] 峰电量
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dSegTotalEnergy[1] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[37...40] 平电量
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dSegTotalEnergy[2] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[41...44] 谷电量
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dSegTotalEnergy[3] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[45...48] 当前充电金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalEnergyFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[49...52] 当前服务费金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[53,54] 当前充电时间
    ustmpNetSeq.usVal = htons(  time(NULL) - pCON->order.tStartTime  );
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[55] 充电桩状态
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[55];//注意修改这里也要修改interface_remote.c对应位置
    //[56] 停止原因
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[56];
    //[57] 当前SOC
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[58,59] 剩余充电时间
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[60...63] 输出电压 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingVoltage * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[64...67] 输出电流 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingCurrent * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[68...71] 电池组最低温度
    //[72...75] 电池组最高温度
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }
    //[76...79] 当前时间戳
    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[80...96] 车辆VIN号
    for(i = 0; i < 17; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 1;
}
static int makeCmdCardRTData(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdCardRTDataBodyCtx(pPObj, pEObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_CARD_RTDATA, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdOrderBodyCtx(void *pPObj, void *pCObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    OrderData_t *pOrder;
    uint8_t *pbuff;
    uint8_t ucOrderSN[8] = {0};
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    us2uc ustmpNetSeq;
    uint8_t reason;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pOrder = (OrderData_t *)pCObj;
    pbuff = pProto->pCMD[ECH_CMDID_ORDER]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0] 有卡 04 无卡05
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    //[1...8] 交易流水号
    StrToHex(pOrder->strOrderSN, ucOrderSN, strlen(pOrder->strOrderSN));
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    //[9] 充电桩接口
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pOrder->ucCONID + 1;
    //[10...25] 卡号
    if(pbuff[0] == 4)
    {
        for(i = 0; i < 16; i++)
        {
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pOrder->strCardID[i];
        }
    }
    else if(pbuff[0] == 5)
    {
        for(i = 0; i < 16; i++)
        {
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
        }
    }
    //[26...29] 充电前总电能示值 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dStartEnergy * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[30...33] 充电后电能总示值 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)((pOrder->dStartEnergy + pOrder->dTotalEnergy) * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[34...37] 本次充电电费总金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dTotalEnergyFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[38...41] 本次充电服务费总金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dTotalServFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[42...45] 尖电价       xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegEnergyFee[0] * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[46...49] 尖服务费单价 xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegServFee[0] * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[50...53] 尖电量       xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalEnergy[0] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[54...57] 尖充电金额   xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalEnergyFee[0] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[58...61] 尖服务费金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalServFee[0] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[62,63] 尖充电时长 xx
    ustmpNetSeq.usVal = htons((uint16_t)(pOrder->ulSegTotalTime[0]));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[64...85]峰
    //[64...67] 峰电价       xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegEnergyFee[1] * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[68...71] 峰服务费单价 xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegServFee[1] * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[72...75] 峰电量       xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalEnergy[1] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[76...79] 峰充电金额   xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalEnergyFee[1] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[80...83] 峰服务费金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalServFee[1] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[84,85] 峰充电时长 xx
    ustmpNetSeq.usVal = htons((uint16_t)(pOrder->ulSegTotalTime[1]));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[86...107]平
    //[86...89] 平电价       xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegEnergyFee[2] * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[90...93] 平服务费单价 xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegServFee[2] * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[94...97] 平电量       xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalEnergy[2] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[98...101] 平充电金额   xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalEnergyFee[2] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[102...105] 平服务费金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalServFee[2] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[106,107] 平充电时长 xx
    ustmpNetSeq.usVal = htons((uint16_t)(pOrder->ulSegTotalTime[2]));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[108...129]谷
    //[108...111] 谷电价       xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegEnergyFee[3] * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[112...115] 谷服务费单价 xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegServFee[3] * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[116...119] 谷电量       xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalEnergy[3] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[120...123] 谷充电金额   xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalEnergyFee[3] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[124...127] 谷服务费金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pOrder->dSegTotalServFee[3] * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[128,129] 谷充电时长 xx
    ustmpNetSeq.usVal = htons((uint16_t)(pOrder->ulSegTotalTime[3]));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[130...133] 充电开始时间
    ultmpNetSeq.ulVal = htonl(pOrder->tStartTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[134,135] 充电持续时间
    ustmpNetSeq.usVal = htons(pOrder->tStopTime - pOrder->tStartTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[136] 停止充电原因
    switch(pOrder->ucStopType)
    {
    case defOrderStopType_RFID:
    case defOrderStopType_Remote:
        reason = 1;//手动停止
        break;
    case defOrderStopType_Full:
        reason = 3;//充满停止
        break;
    case defOrderStopType_Fee:
        reason = 4;//达到充电金额
        break;
    case defOrderStopType_Scram:
    case defOrderStopType_Offline:
    case defOrderStopType_Poweroff:
    case defOrderStopType_OverCurr:
    case defOrderStopType_Knock:
        reason = 5;//异常停止
        break;
    default:
        reason = 6;//其他原因停止
        break;
    }
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = reason;
    //[137] 当前soc
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[138] 状态
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[139...142] 充电结束时间
    ultmpNetSeq.ulVal = htonl(pOrder->tStopTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 1;    
}
static int makeCmdOrder(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdOrderBodyCtx(pPObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_ORDER, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdSetResBodyCtx(void *pPObj, uint16_t usCmdID, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[usCmdID]->ucRecvdOptData;  // -------注意修改ID

    //[0...3] 操作ID
    pucMsgBodyCtx_dec[0] = pbuff[0];
    pucMsgBodyCtx_dec[1] = pbuff[1];
    pucMsgBodyCtx_dec[2] = pbuff[2];
    pucMsgBodyCtx_dec[3] = pbuff[3];

    *pulMsgBodyCtxLen_dec = 4; //不要忘记赋值
   
    return 1;    
}
static int makeCmdSetSucc(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdSetResBodyCtx(pPObj, ECH_CMDID_SET_SUCC, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_SET_SUCC, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdSetFail(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdSetResBodyCtx(pPObj, ECH_CMDID_SET_FAIL, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_SET_FAIL, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdReqFeeBodyCtx(void *pPObj, uint16_t usCmdID, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[usCmdID]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] 操作ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4...7]充电桩当前时间
    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    switch(usCmdID)
    {
    case ECH_CMDID_REQ_ENERGYFEE:
        //[8...11] 尖
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegEnergyFee[0] * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[12...15] 峰
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegEnergyFee[1] * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[16...19] 平
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegEnergyFee[2] * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[20...23] 谷
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegEnergyFee[3] * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        break;
    case ECH_CMDID_REQ_SERVFEE:
        //[8...11] 尖
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegServFee[0] * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[12...15] 峰
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegServFee[1] * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[16...19] 平
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegServFee[2] * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[20...23] 谷
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dSegServFee[3] * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        break;
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
    return 1;    
}
static int makeCmdReqEnergyFee(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdReqFeeBodyCtx(pPObj, ECH_CMDID_REQ_ENERGYFEE, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_ENERGYFEE, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdReqServFee(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdReqFeeBodyCtx(pPObj, ECH_CMDID_REQ_SERVFEE, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_SERVFEE, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdReqCycBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;


    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_REQ_CYC]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] 操作ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4] 状态上报间隔
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = (uint8_t)(pProto->info.ulStatusCyc_ms / 1000);
    //[5] 实时数据上报间隔
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = (uint8_t)(pProto->info.ulRTDataCyc_ms / 1000);

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
    return 1;    
}
static int makeCmdReqCyc(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdReqCycBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_CYC, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}

static int makeCmdReqTimeSegBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_REQ_TIMESEG]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] 操作ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //尖时间段个数
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[0].ucPeriodCont;
    //尖时起止时间段
    for(i = 0; i < pProto->info.SegTime[0].ucPeriodCont; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[0].ucStart[i];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[0].ucEnd[i];
    }
    //峰时间段个数
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[1].ucPeriodCont;
    //峰时起止时间段
    for(i = 0; i < pProto->info.SegTime[1].ucPeriodCont; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[1].ucStart[i];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[1].ucEnd[i];
    }

    //平时间段个数
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[2].ucPeriodCont;
    //平时起止时间段
    for(i = 0; i < pProto->info.SegTime[2].ucPeriodCont; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[2].ucStart[i];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[2].ucEnd[i];
    }

    //谷时间段个数
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[3].ucPeriodCont;
    //谷时起止时间段
    for(i = 0; i < pProto->info.SegTime[3].ucPeriodCont; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[3].ucStart[i];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime[3].ucEnd[i];
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
    return 1;    
}
static int makeCmdReqTimeSeg(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdReqTimeSegBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_TIMESEG, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}


static int makeCmdReqKeyBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_REQ_KEY]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] 操作ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4...19] 密钥
    for(i = 0; i < 16; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.strKey[i];
    }
    //[20...23] 密钥变更时间
    ultmpNetSeq.ulVal = htonl(pProto->info.tNewKeyChangeTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 1;    
}
static int makeCmdReqKey(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdReqKeyBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_KEY, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}


static int makeCmdReqSoftVerBodyCtx(void *pPObj, void *pEObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    EVSE_t *pEVSE;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    int i;

    pEVSE = (EVSE_t *)pEObj;
    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_REQ_SOFTVER]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] 操作ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4...13] 软件版本号
    for(i = 0; i < 10; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = xSysconf.strVersion[i]; //eg. 3.9.3135.17 in version.h
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
    return 1;    
}
static int makeCmdReqSoftVer(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdReqSoftVerBodyCtx(pPObj, pEObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_SOFTVER, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdReqQRBodyCtx(void *pPObj, void *pEObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    EVSE_t *pEVSE;
    CON_t *pCON;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    int con_id, remote_id, i_qr;
    uint8_t total;
    uint32_t qr_len;

    pEVSE = (EVSE_t *)pEObj;
    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_REQ_QR]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] 操作ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4] 枪口个数
    total = pEVSE->info.ucTotalCON;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = total;

    for(con_id = 0; con_id < total; con_id++)
    {
        pCON = CONGetHandle(con_id);
        //[5] 充电桩接口
        remote_id = EchCONIDtoRemoteID(con_id, total);
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = remote_id;
        //[6] 二维码长度
        qr_len = strlen(pCON->info.strQRCode);
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = qr_len;
        //[7...]二维码
        for(i_qr = 0; i_qr < qr_len; i_qr++)
        {
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.strQRCode[i_qr];
        }
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
    return 1;    
}
static int makeCmdReqQR(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdReqQRBodyCtx(pPObj, pEObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_QR, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}

static int makeCmdSetBnWResBodyCtx(void *pPObj, uint16_t usCmdID, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[usCmdID]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] 操作ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4] 设置结果
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[4];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
    return 1;    
}

static int makeCmdSetBlackRes(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdSetBnWResBodyCtx(pPObj, ECH_CMDID_SET_BLACK, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_SET_BLACK, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdSetWhiteRes(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdSetBnWResBodyCtx(pPObj, ECH_CMDID_SET_WHITE, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_SET_WHITE, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}

static int makeCmdReqBnWBodyCtx(void *pPObj, uint16_t usCmdID, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    us2uc ustmpNetSeq;
    uint16_t usListCont;
    int i,j;
    uint8_t ucOffset = 0;
    char strID[16+1] = {0};
    char path[64];

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[usCmdID]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;
    if(usCmdID == ECH_CMDID_REQ_BLACK)
    {
        strcpy(path, pathBlackList);
    }
    else if(usCmdID == ECH_CMDID_REQ_WHITE)
    {
        strcpy(path, pathWhiteList);
    }

    //[0...3] 操作ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4, 5] 名单个数
    ucOffset = 4;
    pProto->info.BnWGetListSizeCfg(path, &usListCont);
    ustmpNetSeq.usVal = htons(usListCont);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[6...]名单列表
    for(i = 0; i < usListCont; i++)
    {
        pProto->info.BnWGetListCfg(path, i, strID);
        for(j = 0; j < 16; j++)
        {
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = strID[j];
        }
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
    return 1;    
}
static int makeCmdReqBlack(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdReqBnWBodyCtx(pPObj, ECH_CMDID_REQ_BLACK, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_BLACK, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdReqWhite(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdReqBnWBodyCtx(pPObj, ECH_CMDID_REQ_WHITE, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_WHITE, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdCardStartBodyCtx(void *pEObj, void *pCObj, uint16_t usCmdID, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    EVSE_t *pEVSE;
    RFIDDev_t *pRfid;
    uint8_t ucOrderSN[8] = {0};
    uint8_t strOrderSN[17] = {0};
    uint32_t ulMsgBodyCtxLen_dec;
    uint8_t remote_id;
    ul2uc ultmpNetSeq;
    int i;

    pEVSE = (EVSE_t *)pEObj;
    pRfid = (RFIDDev_t *)pCObj; //注意这里传过来的设备是刷卡板, 而不是枪, 我们要处理刷卡板里面的数据
    ulMsgBodyCtxLen_dec = 0;
    //[0] 充电桩接口
    remote_id = EchCONIDtoRemoteID(pRfid->order.ucCONID, pEVSE->info.ucTotalCON);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = remote_id;
    //[1...16] 刷卡启动的卡号
    for(i = 0;  i < 16; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pRfid->order.strCardID[i];
    }
    //[17...24] 有卡充电流水号
    ultmpNetSeq.ulVal = htonl(time(NULL)); // 采用时间戳作为交易流水号
    while (ultmpNetSeq.ulVal == 0)
	{
		printf_safe("有卡充电时， time null == 0！！！！！！\n");
	}
    ucOrderSN[0] = 0;
    ucOrderSN[1] = 0;
    ucOrderSN[2] = 0;
    ucOrderSN[3] = 0;
    ucOrderSN[4] = ultmpNetSeq.ucVal[0];
    ucOrderSN[5] = ultmpNetSeq.ucVal[1];
    ucOrderSN[6] = ultmpNetSeq.ucVal[2];
    ucOrderSN[7] = ultmpNetSeq.ucVal[3];
    for (i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    if (usCmdID == ECH_CMDID_CARD_START_PWD)
    {
      //[25...30]密码
        for (i = 0; i < 6; i++)
        {
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pRfid->status.strPwd[i];
        }  
    }
    
    //保存流水号到order
    HexToStr(ucOrderSN, strOrderSN, 8);
    strcpy(pRfid->order.strOrderSN, strOrderSN);

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
   
    return 1;    
}
static int makeCmdCardStart(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdCardStartBodyCtx(pEObj, pCObj, ECH_CMDID_CARD_START, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_CARD_START, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdCardStartPwd(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdCardStartBodyCtx(pEObj, pCObj, ECH_CMDID_CARD_START_PWD, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_CARD_START_PWD, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdCardStartResBodyCtx(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    CON_t *pCON;
    EVSE_t *pEVSE;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    uint8_t ucOrderSN[8] = {0};
    ul2uc ultmpNetSeq;
    int i;
    EventBits_t uxBit;

    pProto = (echProtocol_t *)pPObj;
    pCON = (CON_t *)pCObj;
    pEVSE = (EVSE_t *)pEObj;
    pbuff = pProto->pCMD[ECH_CMDID_CARD_START_RES]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0] 充电桩接口
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = EchCONIDtoRemoteID(pCON->order.ucCONID, pEVSE->info.ucTotalCON);
    //[1...16] 卡号
    for(i = 0; i < 16; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->order.strCardID[i];
    }
    //[17] 卡号状态
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->order.ucCardStatus;
    //[18] 启动结果
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[18];
    //[19...26] 有卡充电流水号
    StrToHex(pCON->order.strOrderSN, ucOrderSN, 16);
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    //[27...30] 当前电表读数
    uxBit = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                defEventBitOrderMakeOK,
                                pdFALSE, pdTRUE, 10000);
    if ((uxBit & defEventBitOrderMakeOK) == defEventBitOrderMakeOK)
    {
        ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dStartEnergy * 100));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    }
    else 
    {
        return 0;
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 1;
}
static int makeCmdCardStartRes(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;
    int res;

    // -------注意修改ID
    res = makeCmdCardStartResBodyCtx(pPObj, pEObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    if (res != 1)
    {
        return 0;
    }
    res = makeStdCmd(pPObj, pEObj, ECH_CMDID_CARD_START_RES, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return res;
}
static int makeCmdCardStopResBodyCtx(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    CON_t *pCON;
    EVSE_t *pEVSE;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    uint8_t ucOrderSN[8] = {0};
    int i;
    EventBits_t uxBits;

    pProto = (echProtocol_t *)pPObj;
    pCON = (CON_t *)pCObj;
    pEVSE = (EVSE_t *)pEObj;

    pbuff = pProto->pCMD[ECH_CMDID_CARD_STOP_RES]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;

    //[0] 充电桩接口
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = EchCONIDtoRemoteID(pCON->order.ucCONID, pEVSE->info.ucTotalCON);
    //[1...16] 启动卡号
    for(i = 0; i < 16; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->order.strCardID[i];
    }
    //[17] 卡号状态
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->order.ucCardStatus;
    //[18...25] 有卡充电流水号
    StrToHex(pCON->order.strOrderSN, ucOrderSN, 16);
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    //[26] 停止充电原因
    uxBits = xEventGroupGetBits(pCON->status.xHandleEventOrder);
    if((uxBits & defEventBitOrderMakeFinish) == defEventBitOrderMakeFinish)
    {
        switch(pCON->order.ucStopType)
        {
        case defOrderStopType_RFID:
        case defOrderStopType_Remote:
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
            break;
        case defOrderStopType_Full:
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 3;
            break;
        case defOrderStopType_Fee:
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 4;
            break;
        case defOrderStopType_Scram:
        case defOrderStopType_Offline:
        case defOrderStopType_Poweroff:
        case defOrderStopType_OverCurr:
        case defOrderStopType_Knock:
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 5;
            break;
        default:
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 6;
            break;
        }
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 1;
}
static int makeCmdCardStopRes(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdCardStopResBodyCtx(pPObj, pEObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_CARD_STOP_RES, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}

static int makeCmdUpFaultBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    int i;

    pProto = (echProtocol_t *)pPObj;

    pbuff = pProto->pCMD[ECH_CMDID_UP_FAULT]->ucRecvdOptData;  // -------注意修改ID

    //[0...5] 故障码
    for(i = 0; i < 6; i++)
    {
        pucMsgBodyCtx_dec[i] = pbuff[i];
    }

    *pulMsgBodyCtxLen_dec = 6; //不要忘记赋值

    return 1;
}
static int makeCmdUpFault(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdUpFaultBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_UP_FAULT, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}

static int makeCmdUpWarningBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    int i;
    EventBits_t uxBits;

    pProto = (echProtocol_t *)pPObj;

    pbuff = pProto->pCMD[ECH_CMDID_UP_WARNING]->ucRecvdOptData;  // -------注意修改ID

    //[0...5] 告警
    //[6...11] 保护
    for(i = 0; i < 12; i++)
    {
        pucMsgBodyCtx_dec[i] = pbuff[i];
    }

    *pulMsgBodyCtxLen_dec = 12; //不要忘记赋值

    return 1;
}
static int makeCmdUpWarning(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdUpFaultBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_UP_WARNING, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;    
}
static int makeCmdSetOTABodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;

    pProto = (echProtocol_t *)pPObj;

    pbuff = pProto->pCMD[ECH_CMDID_SET_OTA]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;
    //[0...3] 操作ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4] 结果
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[4];
    
    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
    
    return 1;
}
static int makeCmdSetOTA(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdSetOTABodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_SET_OTA, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;   
}
static int makeCmdReqOTA_DWBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;

    pProto = (echProtocol_t *)pPObj;

    pbuff = pProto->pCMD[ECH_CMDID_REQ_OTA_DW]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;
    //[0...3] 操作ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4] 结果
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[4];
    
    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
    
    return 1;
}
static int makeCmdReqOTA_DW(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdReqOTA_DWBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_OTA_DW, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;   
}
static int makeCmdOTA_StartBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint32_t ulMsgBodyCtxLen_dec;
    int i;
    
    pProto = (echProtocol_t *)pPObj;

    ulMsgBodyCtxLen_dec = 0;
    //[0...9] 软件版本号
    for (i = 0; i < 10; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.ftp.strNewVersion[i];
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
    
    return 1;
}
static int makeCmdOTA_Start(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdOTA_StartBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_OTA_START, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;
}
static int makeCmdOTA_ResultBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    int i;
    char file_id[3] = { 0};
    uint8_t succ = 2;
    
    pProto = (echProtocol_t *)pPObj;

    pbuff = pProto->pCMD[ECH_CMDID_OTA_RESULT]->ucRecvdOptData;  // -------注意修改ID
    ulMsgBodyCtxLen_dec = 0;
    //[0...9] 软件版本号
    for (i = 0; i < 10; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.ftp.strNewVersion[i];
    }
    //[10...19] 当前软件版本号
    for (i = 0; i < 10; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = xSysconf.strVersion[i];
    }
    //[20] 升级结果
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[20];
    //[21] 失败原因
    if (pbuff[20] == 1)//成功
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;//0. 升级成功
    }
    else if (pbuff[20] == 2)//失败
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 3;//3. 其他错误
    }
    
    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值
    
    return 1;
}
static int makeCmdOTA_Result(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    // -------注意修改ID
    makeCmdOTA_ResultBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_OTA_RESULT, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
    return 1;
}
static uint16_t GetCmdIDViaRecvCmd(echProtocol_t *pProto, uint16_t usRecvCmd)
{
    uint32_t id;
    for(id = 0; id < ECH_CMD_MAX; id++)
    {
        if(pProto->pCMD[id]->CMDType.usRecvCmd == usRecvCmd)
        {
            return id;
        }
    }
    return ECH_CMD_MAX;
}
static uint32_t GetRecvTOViaRecvCmd(echProtocol_t *pProto, uint16_t usRecvCmd)
{
    uint32_t id;
    for(id = 0; id < ECH_CMD_MAX; id++)
    {
        if(pProto->pCMD[id]->CMDType.usRecvCmd == usRecvCmd)
        {
            return pProto->pCMD[id]->ulRecvTimeout_s;
        }
    }
    return 1;
}
#define ECH_ERR_OK      1
#define ECH_ERR_VER     -1
#define ECH_ERR_CHECK   -2
#define ECH_ERR_ID      -3
#define ECH_ERR_CMDID   -4
#define ECH_ERR_LEN     -5

static int recvResponse(void *pPObj,
                        void *pEObj,
                        uint8_t *pbuff,
                        uint32_t ulRecvdLen,
                        uint8_t trycountmax)
{
    echProtocol_t *pProto;
    EVSE_t *pE;
    echProtoElem_t echRecvCmdElem;
    us2uc ustmpNetSeq;
    ul2uc ultmpNetSeq;
    uint32_t ulMsgBodyLen_enc;
    uint16_t usChecksum;
    uint8_t EVSEID[8];
    uint32_t ulOffset;
    int i;
    uint16_t cmd_id;

    pProto = (echProtocol_t *)pPObj;
    pE = (EVSE_t *)pEObj;
    ulOffset = 0;
    while(pbuff[ulOffset] != pProto->info.ucProtoVer)
    {
        ulOffset++;
        if(ulOffset == ulRecvdLen)
        {
            return ECH_ERR_VER;
        }
    }

    //pbuff[1] 属性

    //pbuff[2,3] 命令字
    ustmpNetSeq.ucVal[0] = pbuff[ulOffset + 2];
    ustmpNetSeq.ucVal[1] = pbuff[ulOffset + 3];
    echRecvCmdElem.cmd.usRecvCmd = ntohs(ustmpNetSeq.usVal);

    //pbuff[4...7] 预留字段

    //pbuff[8...11] 消息体长度
    ultmpNetSeq.ucVal[0] = pbuff[ulOffset + 8];
    ultmpNetSeq.ucVal[1] = pbuff[ulOffset + 9];
    ultmpNetSeq.ucVal[2] = pbuff[ulOffset + 10];
    ultmpNetSeq.ucVal[3] = pbuff[ulOffset + 11];
    ulMsgBodyLen_enc = ntohl(ultmpNetSeq.ulVal);
    if (ulRecvdLen < ulMsgBodyLen_enc + 14)
    {
        return ECH_ERR_LEN;
    }
    //pbuff[12,13] Checksum
    ustmpNetSeq.ucVal[0] = pbuff[ulOffset + 12];
    ustmpNetSeq.ucVal[1] = pbuff[ulOffset + 13];
    usChecksum = ntohs(ustmpNetSeq.usVal);
    if(usChecksum != echVerifCheck(pProto->info.ucProtoVer, 0, echRecvCmdElem.cmd.usRecvCmd, ulMsgBodyLen_enc))
    {
        return ECH_ERR_CHECK;
    }
//    StrToHex(&pbuff[ulOffset + 14], EVSEID, 16);
    if(memcmp(&pbuff[ulOffset + 14], pE->info.strID, 16) != 0 )
    {
        return ECH_ERR_ID;
    }
    echRecvCmdElem.timestamp = time(NULL);
    cmd_id = GetCmdIDViaRecvCmd(pProto, echRecvCmdElem.cmd.usRecvCmd);
    if(cmd_id == ECH_CMD_MAX)
    {
        return ECH_ERR_CMDID;
    }
    echRecvCmdElem.cmd_id = cmd_id;
    echRecvCmdElem.timeout_s = GetRecvTOViaRecvCmd(pProto, echRecvCmdElem.cmd.usRecvCmd);
    echRecvCmdElem.len = ulMsgBodyLen_enc + 14;
    echRecvCmdElem.pbuff = pbuff;
    echRecvCmdElem.status = 0;
    echRecvCmdElem.trycount = 0;
    echRecvCmdElem.trycountmax = trycountmax;

    if (xSemaphoreTake(pProto->xMutexProtoRecv, 10000) == pdPASS)
    {
        gdsl_list_insert_tail(pProto->plechRecvCmd, (void *)&echRecvCmdElem);
        xSemaphoreGive(pProto->xMutexProtoRecv);
    }
    else
    {
        return ECH_ERR_OK;
    }

    if(ulRecvdLen - ulOffset > echRecvCmdElem.len)
    {
        recvResponse(pPObj, pEObj, &pbuff[ulOffset + echRecvCmdElem.len], ulRecvdLen - ulOffset - echRecvCmdElem.len, 3);
    }
    return ECH_ERR_OK;
}

/** @brief
 *
 * @param pPObj void*
 * @param usCmdID uint16_t
 * @param pbuff uint8_t*
 * @param ulRecvLen uint32_t
 * @return int
 *
 */
static int analyStdRes(void *pPObj, uint16_t usCmdID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    echProtocol_t *pProto;
    uint8_t *pMsgBodyCtx_enc;
    uint8_t *pMsgBodyCtx_dec;
    uint32_t ulMsgBodyCtxLen_enc;
    ul2uc ultmpNetSeq;
    time_t timestamp;

    pProto = (echProtocol_t *)pPObj;
    pMsgBodyCtx_enc = pbuff + 30;         //取出加密部分buff
    ulMsgBodyCtxLen_enc = ulRecvLen - 30; //加密部分长度
    if (ulMsgBodyCtxLen_enc == 0)
    {
        pProto->pCMD[usCmdID]->ulRecvdOptLen = 0;
    }
    else
    {
        pMsgBodyCtx_dec = (uint8_t *)malloc(ulMsgBodyCtxLen_enc * sizeof(uint8_t));

        ech_aes_decrypt(pMsgBodyCtx_enc, ulMsgBodyCtxLen_enc, pProto->info.strKey, pMsgBodyCtx_dec);

        memcpy(pProto->pCMD[usCmdID]->ucRecvdOptData, pMsgBodyCtx_dec, ulMsgBodyCtxLen_enc);
        pProto->pCMD[usCmdID]->ulRecvdOptLen = ulMsgBodyCtxLen_enc;

        free(pMsgBodyCtx_dec);
    }
    
    return 1;
}

/*
写分析函数的方法
1. 等待Mutex
2. 初始化lRecvElem
3. 将lRecvElem插入队尾
4. 释放Mutex
*/

static int analyCmdCommon(void *pPObj, uint16_t usCmdID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    echProtocol_t *pProto;
    echCMD_t *pCMD;
    echCmdElem_t lRecvElem;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pCMD = pProto->pCMD[usCmdID];

    if(xSemaphoreTake(pCMD->xMutexCmd, 10000) == pdPASS)
    {
        analyStdRes(pPObj, usCmdID, pbuff, ulRecvLen);
        if (pCMD->ulRecvdOptLen == 0)
        {
            
        }
        else
        {
            printf_protodetail("\e[34;43mRecv:\e[0m %02X [%d]\n", pCMD->CMDType.usRecvCmd, pCMD->CMDType.usRecvCmd);
            for (i = 0; i < pCMD->ulRecvdOptLen; i++)
            {
                printf_protodetail("%02X ", pCMD->ucRecvdOptData[i]);
            }
            printf_protodetail("\n");
            lRecvElem.UID = 0;
            lRecvElem.timestamp = time(NULL);
            lRecvElem.len = pCMD->ulRecvdOptLen;
            lRecvElem.pbuff = pCMD->ucRecvdOptData;
            lRecvElem.status = 0;
            gdsl_list_insert_tail(pCMD->plRecvCmd, (void *)&lRecvElem);
        }

        xSemaphoreGive(pCMD->xMutexCmd);
    }//if mutex

    return 1;
}

static int analyCmdHeart(void *pPObj, uint16_t usCmdID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    echProtocol_t *pProto;
    echCMD_t *pCMD;
    uint8_t *pMsgBodyCtx_dec;
    ul2uc ultmpNetSeq;
    time_t timestamp;
    echCmdElem_t lRecvElem;

    pProto = (echProtocol_t *)pPObj;
    pCMD = pProto->pCMD[usCmdID];
    if(xSemaphoreTake(pCMD->xMutexCmd, 10000) == pdTRUE)
    {
        analyStdRes(pPObj, usCmdID, pbuff, ulRecvLen);
        pMsgBodyCtx_dec = pCMD->ucRecvdOptData;

        ultmpNetSeq.ucVal[0] = pMsgBodyCtx_dec[0];
        ultmpNetSeq.ucVal[1] = pMsgBodyCtx_dec[1];
        ultmpNetSeq.ucVal[2] = pMsgBodyCtx_dec[2];
        ultmpNetSeq.ucVal[3] = pMsgBodyCtx_dec[3];
        timestamp = (time_t)ntohl(ultmpNetSeq.ulVal);
        if (dePrintTime == 1)
        {
            printf_safe("server: ");
            printTime(timestamp);
            printf_safe("\n");
            printf_safe("local:  ");
            printTime(time(NULL));
            printf_safe("\n");
        }
        if(utils_abs(timestamp - time(NULL)) > 10)//大于10s进行校时
        {
            time(&timestamp);
        }
        lRecvElem.UID = 0;
        lRecvElem.timestamp = time(NULL);
        lRecvElem.len = pCMD->ulRecvdOptLen;
        lRecvElem.pbuff = pCMD->ucRecvdOptData;
        lRecvElem.status = 0;
        gdsl_list_insert_tail(pCMD->plRecvCmd, (void *)&lRecvElem);

        xSemaphoreGive(pCMD->xMutexCmd);
    }//if mutex

    return 1;
}
static gdsl_element_t echCmdListAlloc(gdsl_element_t e)
{
    echCmdElem_t *copyCmdElem;
    copyCmdElem = (echCmdElem_t *)malloc(sizeof(echCmdElem_t));
    if(copyCmdElem != NULL)
    {
        memcpy(copyCmdElem, e, sizeof(echCmdElem_t));
    }
    else
    {
        return NULL;
    }
    copyCmdElem->pbuff = (uint8_t *)malloc(copyCmdElem->len * sizeof(uint8_t));
    if(copyCmdElem->pbuff != NULL)
    {
        memcpy(copyCmdElem->pbuff, ((echCmdElem_t *)e)->pbuff, copyCmdElem->len);
    }
    else
    {
        return NULL;
    }

    return (gdsl_element_t)copyCmdElem;
}
static void echCmdListFree (gdsl_element_t e)
{
    free(((echCmdElem_t *)e)->pbuff);
    free (e);
    ((echCmdElem_t *)e)->pbuff = NULL;
    e = NULL;
}
/** @brief 复制待插入的元素到新申请的空间
 *
 * @param pechCmd void*
 * @return gdsl_element_t
 *
 */
static gdsl_element_t echProtoListAlloc(gdsl_element_t e)
{
    echProtoElem_t *copyProtoElem;

    copyProtoElem = (echProtoElem_t *)malloc(sizeof(echProtoElem_t));
    if(copyProtoElem != NULL)
    {
        memcpy(copyProtoElem, e, sizeof(echProtoElem_t));
    }
    else
    {
        return NULL;
    }
    copyProtoElem->pbuff = (uint8_t *)malloc(copyProtoElem->len * sizeof(uint8_t));
    if(copyProtoElem->pbuff != NULL)
    {
        memcpy(copyProtoElem->pbuff, ((echProtoElem_t *)e)->pbuff, copyProtoElem->len);
    }
    else
    {
        return NULL;
    }

    return (gdsl_element_t)copyProtoElem;
}

static void echProtoListFree (gdsl_element_t e)
{
    free(((echProtoElem_t *)e)->pbuff);
    free (e);
    ((echProtoElem_t *)e)->pbuff = NULL;
    e = NULL;
}

static void deleteProto(void *pPObj)
{
    int i;
    echProtocol_t *pProto;

    pProto = (echProtocol_t *)pPObj;
    for(i = 0; i < ECH_CMD_MAX; i++)
    {
        if(pProto->pCMD[i] != NULL)
        {
            vEventGroupDelete(pProto->pCMD[i]->xHandleEventCmd);
            vSemaphoreDelete(pProto->pCMD[i]->xMutexCmd);
            gdsl_list_free(pProto->pCMD[i]->plRecvCmd);
            free(pProto->pCMD[i]);
            pProto->pCMD[i] = NULL;
        }
    }
    gdsl_list_free(pProto->plechSendCmd);
    gdsl_list_free(pProto->plechRecvCmd);
    vSemaphoreDelete(pProto->xMutexProtoRecv);
    vSemaphoreDelete(pProto->xMutexProtoSend);
    free(pProto);
    pProto = NULL;
}
static echCMD_t *EchCMDCreate(uint16_t usSendCmd,
                              uint16_t usRecvCmd,
                              uint32_t ulRecvTimeout_s,
                              pECH_MAKE_PROC makeProc,
                              pECH_ANALY_PROC analyProc)
{
    echCMD_t *pECHCMD = (echCMD_t *)malloc(sizeof(echCMD_t));
    if(pECHCMD == NULL)
    {
        return NULL;
    }
    pECHCMD->CMDType.usSendCmd = usSendCmd;
    pECHCMD->CMDType.usRecvCmd = usRecvCmd;

    pECHCMD->ulRecvTimeout_s = ulRecvTimeout_s;

    pECHCMD->xHandleEventCmd = xEventGroupCreate();
    pECHCMD->xMutexCmd = xSemaphoreCreateMutex();

    pECHCMD->plRecvCmd = gdsl_list_alloc ("CMDRecvLis", echCmdListAlloc, echCmdListFree);
    pECHCMD->makeProc = makeProc;
    pECHCMD->analyProc = analyProc;

    return pECHCMD;
}

echProtocol_t *EchProtocolCreate(void)
{
    int i,j;
    echProtocol_t *pProto = (echProtocol_t *)malloc(sizeof(echProtocol_t));
    if(pProto == NULL)
    {
        return NULL;
    }
    memset(pProto, 0, sizeof(echProtocol_t));

    EchFtpInit(&pProto->info.ftp);
    
    pProto->info.GetProtoCfg = GetProtoCfg;

    pProto->info.BnWIsListCfg = BnWIsListCfg;
    pProto->info.BnWGetListCfg = BnWGetListCfg;
    pProto->info.BnWGetListSizeCfg = BnWGetListSizeCfg;
    pProto->info.BnWAddListCfg = BnWAddListCfg;
    pProto->info.BnWDeleteListCfg = BnWDeleteListCfg;
    pProto->info.BnWFlushListCfg = BnWFlushListCfg;

    /* @todo (rgw#1#): 接收命令超时参数现在已经不用了, 随便设置, 调试完成后剔除 */
    //注册                                 (桩命令, 平台命令, 接收的命令处理超时, 发送命令制作, 接收分析)
    pProto->pCMD[ECH_CMDID_REGISTER]       = EchCMDCreate(1,   2,   0,  makeCmdReg,          analyCmdCommon);
    pProto->pCMD[ECH_CMDID_HEARTBEAT]      = EchCMDCreate(3,   4,   0,  makeCmdHeart,        analyCmdHeart);
    pProto->pCMD[ECH_CMDID_RESET]          = EchCMDCreate(6,   5,   30, makeCmdReset,        analyCmdCommon);
    pProto->pCMD[ECH_CMDID_STATUS]         = EchCMDCreate(41,  42,  30, makeCmdStatus,       analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REMOTE_CTRL]    = EchCMDCreate(44,  43,  30, makeCmdRemoteCtrl,   analyCmdCommon);
    pProto->pCMD[ECH_CMDID_RTDATA]         = EchCMDCreate(45,  0,   0,  makeCmdRTData,       NULL);
    pProto->pCMD[ECH_CMDID_ORDER]          = EchCMDCreate(46,  47,  30, makeCmdOrder,        analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_SUCC]       = EchCMDCreate(7,   0,   0,  makeCmdSetSucc,      NULL);
    pProto->pCMD[ECH_CMDID_SET_FAIL]       = EchCMDCreate(8,   0,   0,  makeCmdSetFail,      NULL);
    pProto->pCMD[ECH_CMDID_SET_ENERGYFEE]   = EchCMDCreate(0,   11,  30, NULL,                analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_SERVFEE]    = EchCMDCreate(0,   12,  30, NULL,                analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_CYC]        = EchCMDCreate(0,   13,  30, NULL,                analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_TIMESEG]    = EchCMDCreate(0,   14,  30, NULL,                analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_KEY]        = EchCMDCreate(0,   15,  30, NULL,                analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_ENERGYFEE]   = EchCMDCreate(22,  21,  30, makeCmdReqEnergyFee,  analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_SERVFEE]    = EchCMDCreate(24,  23,  30, makeCmdReqServFee,   analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_CYC]        = EchCMDCreate(26,  25,  30, makeCmdReqCyc,       analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_TIMESEG]    = EchCMDCreate(28,  27,  30, makeCmdReqTimeSeg,   analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_KEY]        = EchCMDCreate(30,  29,  30, makeCmdReqKey,       analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_SOFTVER]    = EchCMDCreate(34,  33,  30, makeCmdReqSoftVer,   analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_QR]         = EchCMDCreate(0,   35,  30, NULL,                analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_QR]         = EchCMDCreate(37,  36,  30, makeCmdReqQR,        analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_BLACK]      = EchCMDCreate(98,  97,  30, makeCmdSetBlackRes,  analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_WHITE]      = EchCMDCreate(100, 99,  30, makeCmdSetWhiteRes,  analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_BLACK]      = EchCMDCreate(102, 101, 30, makeCmdReqBlack,     analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_WHITE]      = EchCMDCreate(104, 103, 30, makeCmdReqWhite,     analyCmdCommon);
    pProto->pCMD[ECH_CMDID_ADD_BNW]        = EchCMDCreate(0,   105, 30, NULL,                analyCmdCommon);
    pProto->pCMD[ECH_CMDID_DEL_BNW]        = EchCMDCreate(0,   106, 30, NULL,                analyCmdCommon);
    pProto->pCMD[ECH_CMDID_CARD_START]     = EchCMDCreate(90,  91,  30, makeCmdCardStart,    analyCmdCommon);
    pProto->pCMD[ECH_CMDID_CARD_START_PWD] = EchCMDCreate(89,  91,  30, makeCmdCardStartPwd, analyCmdCommon);
    pProto->pCMD[ECH_CMDID_CARD_START_RES] = EchCMDCreate(92,  93,  30, makeCmdCardStartRes, analyCmdCommon);
    pProto->pCMD[ECH_CMDID_CARD_STOP_RES]  = EchCMDCreate(95,  96,  30, makeCmdCardStopRes,  analyCmdCommon);
    pProto->pCMD[ECH_CMDID_CARD_RTDATA]    = EchCMDCreate(94,  0,   30, makeCmdCardRTData,   NULL);
    pProto->pCMD[ECH_CMDID_UP_FAULT]       = EchCMDCreate(70,  71,  30, makeCmdUpFault,      analyCmdCommon);
    pProto->pCMD[ECH_CMDID_UP_WARNING]     = EchCMDCreate(72,  73,  30, makeCmdUpWarning,    analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_OTA]        = EchCMDCreate(111, 110, 30, makeCmdSetOTA,       analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_OTA_DW]     = EchCMDCreate(113, 112, 30, makeCmdReqOTA_DW,    analyCmdCommon);
    pProto->pCMD[ECH_CMDID_OTA_START]      = EchCMDCreate(114, 115, 30, makeCmdOTA_Start,    analyCmdCommon);
    pProto->pCMD[ECH_CMDID_OTA_RESULT]     = EchCMDCreate(116, 117, 30, makeCmdOTA_Result,   analyCmdCommon);

    //end of 注册                                       (桩命令, 平台命令, 接收的命令处理超时, 发送命令制作, 接收分析)

    pProto->recvResponse = recvResponse;
    pProto->sendCommand = sendCommand;
    pProto->deleteProtocol = deleteProto;

    pProto->plechRecvCmd = gdsl_list_alloc ("ProtoRecvLis", echProtoListAlloc, echProtoListFree);
    pProto->plechSendCmd = gdsl_list_alloc ("ProtoSendLis", echProtoListAlloc, echProtoListFree);
    
    pProto->xMutexProtoRecv = xSemaphoreCreateMutex();
    pProto->xMutexProtoSend = xSemaphoreCreateMutex();

    return pProto;
}



//encode/decode protocol requests
//encode/decode protocol responses

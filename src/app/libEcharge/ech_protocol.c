/**
* @file ech_protocol.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-22
*/
#include "utils.h"
#include "interface.h"
#include "enc_dec.h"
#include "libEcharge/ech_protocol_proc.h"

#include <string.h>
#include "stringName.h"
#include "cfg_parse.h"
#include "cJSON.h"

/*---------------------------------------------------------------------------/
/                               获取协议配置信息
/---------------------------------------------------------------------------*/

#if 0
static ErrorCode_t GetProtoInfo(uint16_t *pProtoInfoU16, uint8_t *jnName, void *pvCfgObj)
{
    uint16_t tmpShort;
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pProtoCfgObj;

    errcode = ERR_NO;
    tmpShort = 0;

    pProtoCfgObj = (cJSON *)pvCfgObj;

    //  解析Int
    jsItem = cJSON_GetObjectItem(pProtoCfgObj, jnName);
    if(jsItem == NULL)
    {
        errcode = ERR_FILE_PARSE;
        goto err_return;
    }

    tmpShort = (uint16_t)(jsItem->valueint);

#ifdef DEBUG_CFG_PARSE_PROTO
    printf_safe("%s\t = %d\n", jnName, tmpShort);
#endif

    /*********************/
    *pProtoInfoU16 = tmpShort;

err_return:
    return errcode;
}

static ErrorCode_t GetProtoInfoStr(uint8_t *protoInfoStr, const uint8_t *jnName, void *pvCfgObj)
{
    uint8_t tmpStrLength;
    uint8_t *ptmpStr;
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pProtoCfgObj;

    errcode = ERR_NO;
    ptmpStr = NULL;

    pProtoCfgObj = (cJSON *)pvCfgObj;

    //  解析string
    jsItem = cJSON_GetObjectItem(pProtoCfgObj, jnName);
    if(jsItem == NULL)
    {
        errcode = ERR_FILE_PARSE;
        goto err_return;
    }
    ptmpStr = utils_strdup(jsItem->valuestring); //strdup之后出现错误都要先free
    if(ptmpStr == NULL)
    {
        errcode = ERR_MEMORY;
        goto err_return;
    }

#ifdef DEBUG_CFG_PARSE_PROTO
    printf_safe("%s\t = %s\n", jnName, ptmpStr);
#endif

    /*********************/
    strcpy(protoInfoStr, ptmpStr);

err_free:
    free(ptmpStr);
err_return:
    return errcode;
}
#endif
/** @brief 获取ProtoCfg中参数的值
 *
 * @param pvProtoInfoItem void* 传入要获取的参数的指针
 * @param type uint8_t 要获取参数的类型
 * @param pvCfgObj void* 
 * @param jnItemName uint8_t* 参数名称
 * @return ErrorCode_t
 *
 */       
static ErrorCode_t GetProtoCfgItem(void *pvProtoInfoItem, uint8_t type, void *pvCfgObj, uint8_t *jnItemName)
{
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pProtoCfgObj;
    errcode = ERR_NO;

    pProtoCfgObj = (cJSON *)pvCfgObj;

    //  解析
    jsItem = cJSON_GetObjectItem(pProtoCfgObj, jnItemName);
    if(jsItem == NULL)
    {
        errcode = ERR_FILE_PARSE;
        goto err_return;
    }
    switch(type)
    {
    case ParamTypeU8:
        *((uint8_t *)pvProtoInfoItem) = (uint8_t)(jsItem->valueint);
        break;
    case ParamTypeU16:
        *((uint16_t *)pvProtoInfoItem) = (uint16_t)(jsItem->valueint);
        break;
    case ParamTypeU32:
        *((uint32_t *)pvProtoInfoItem) = (uint32_t)(jsItem->valueint);
        break;
    case ParamTypeDouble:
        *((double *)pvProtoInfoItem) = (double)(jsItem->valuedouble);
        break;
    case ParamTypeString:
        strcpy((uint8_t *)pvProtoInfoItem, jsItem->valuestring);
        break;
    case ParamTypeObj:
        *(uint32_t *)pvProtoInfoItem = (uint32_t)jsItem; //很不喜欢这种投机取巧，应为这个参数增加一个二级指针
        break;
    default:
        break;
    }

#ifdef DEBUG_CFG_PARSE_PROTO
    switch(type)
    {
    case ParamTypeU8:
        printf_safe("%s\t = %d\n", jnItemName, *((uint8_t *)pvProtoInfoItem));
        break;
    case ParamTypeU16:
        printf_safe("%s\t = %d\n", jnItemName, *((uint16_t *)pvProtoInfoItem));
        break;
    case ParamTypeU32:
        printf_safe("%s\t = %d\n", jnItemName, *((uint32_t *)pvProtoInfoItem));
        break;
    case ParamTypeDouble:
        printf_safe("%s\t = %.2lf\n", jnItemName, *((double *)pvProtoInfoItem));
        break;
    case ParamTypeString:
        printf_safe("%s\t = %s\n", jnItemName, (uint8_t *)pvProtoInfoItem);
        break;
    case ParamTypeObj:
        printf_safe("%s: %x\n", jnItemName, jsItem);
        break;
    default:
        break;
    }
#endif

    /*********************/

err_return:
    return errcode;
}
/** @brief 获取Proto中Obj的子参数
 *
 * @param jsProtoObj cJSON*     Obj的父Obj
 * @param pSegTime EchSegTime_t* Obj对应的时间段结构体
 * @param jnNameObj uint8_t*    Obj的名称
 * @return ErrorCode_t
 *
 */      
static ErrorCode_t GetProtoCfgObj(cJSON *jsProtoObj, EchSegTime_t *pSegTime, uint8_t *jnNameObj)
{
    uint32_t ItemAddr;
    cJSON *jsItem;
    int i;
    uint8_t strName[16] = {0};
    ErrorCode_t errcode;

    errcode = ERR_NO;

    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)&ItemAddr,
                                          ParamTypeObj,
                                          jsProtoObj,
                                          jnNameObj),
                ERR_LEVEL_WARNING,
                "GetProtoCfgObj_Item");
    jsItem = (cJSON *)ItemAddr;
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pSegTime->ucSegCont)),
                                          ParamTypeU8,
                                          jsItem,
                                          jnProtoSegCont),
                ERR_LEVEL_WARNING,
                "GetProtoCfgObj_SegCont()");
    for(i = 0; i < pSegTime->ucSegCont; i++)
        //for(i = 0; i < 5; i++)
    {
        sprintf(strName, "Start%d", i + 1);
        THROW_ERROR(defDevID_File,
                    errcode = GetProtoCfgItem((void *)(&(pSegTime->ucStart[i])),
                                              ParamTypeU8,
                                              jsItem,
                                              strName),
                    ERR_LEVEL_WARNING,
                    "GetProtoCfgObj_Start()");
        sprintf(strName, "End%d", i + 1);
        THROW_ERROR(defDevID_File,
                    errcode = GetProtoCfgItem((void *)(&(pSegTime->ucEnd[i])),
                                              ParamTypeU8,
                                              jsItem,
                                              strName),
                    ERR_LEVEL_WARNING,
                    "GetProtoCfgObj_End()");
    }
    return errcode;
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
    cJSON *jsProtoObj;
    ErrorCode_t errcode;
    echProtocol_t *pProto;

    errcode = ERR_NO;
    pProto = (echProtocol_t *)pvProto;

    /*json解析*/
    jsProtoObj = GetCfgObj(pathProtoCfg, &errcode);
    if(jsProtoObj == NULL || errcode != ERR_NO)
    {
        return errcode;
    }

    THROW_ERROR(defDevID_File, errcode = GetProtoCfgItem((void *)(pProto->info.strServerIP),
                                         ParamTypeString,
                                         jsProtoObj,
                                         jnProtoServerIP),
                ERR_LEVEL_WARNING,
                "GetServerInfo()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.usServerPort)),
                                          ParamTypeU16,
                                          jsProtoObj,
                                          jnProtoServerPort),
                ERR_LEVEL_WARNING,
                "GetServerPort()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(pProto->info.strUserName),
                                          ParamTypeString,
                                          jsProtoObj,
                                          jnProtoUserName),
                ERR_LEVEL_WARNING,
                "GetUserName()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(pProto->info.strUserPwd),
                                          ParamTypeString,
                                          jsProtoObj,
                                          jnProtoUserPwd),
                ERR_LEVEL_WARNING,
                "GetUserPwd()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(pProto->info.strKey),
                                          ParamTypeString,
                                          jsProtoObj,
                                          jnProtoKey),
                ERR_LEVEL_WARNING,
                "GetKey()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(pProto->info.strNewKey),
                                          ParamTypeString,
                                          jsProtoObj,
                                          jnProtoNewKey),
                ERR_LEVEL_WARNING,
                "GetNewKey()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.tNewKeyChangeTime)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoNewKeyChangeTime),
                ERR_LEVEL_WARNING,
                "GetNewKeyChangeTime()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ucProtoVer)),
                                          ParamTypeU8,
                                          jsProtoObj,
                                          jnProtoProtoVer),
                ERR_LEVEL_WARNING,
                "GetProtoVer()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulHeartBeatCyc_ms)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoHeartBeatCyc_ms),
                ERR_LEVEL_WARNING,
                "GetHeartBeatCyc()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulStatusCyc_ms)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoStatusCyc_ms),
                ERR_LEVEL_WARNING,
                "GetStatusCyc()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulRTDataCyc_ms)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoRTDataCyc_ms),
                ERR_LEVEL_WARNING,
                "GetRTDataCyc()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ucResetAct)),
                                          ParamTypeU8,
                                          jsProtoObj,
                                          jnProtoResetAct),
                ERR_LEVEL_WARNING,
                "GetResetAct()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulPowerFee_sharp)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoPowerFee_sharp),
                ERR_LEVEL_WARNING,
                "GetPowerFee_sharp()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulPowerFee_peak)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoPowerFee_peak),
                ERR_LEVEL_WARNING,
                "GetPowerFee_peak()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulPowerFee_shoulder)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoPowerFee_shoulder),
                ERR_LEVEL_WARNING,
                "GetPowerFee_shoulder()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulPowerFee_off_peak)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoPowerFee_off_peak),
                ERR_LEVEL_WARNING,
                "GetPowerFee_off_peak()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulServiceFee_sharp)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoServiceFee_sharp),
                ERR_LEVEL_WARNING,
                "GetServiceFee_sharp()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulServiceFee_peak)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoServiceFee_peak),
                ERR_LEVEL_WARNING,
                "GetServiceFee_peak()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulServiceFee_shoulder)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoServiceFee_shoulder),
                ERR_LEVEL_WARNING,
                "GetServiceFee_shoulder()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulServiceFee_off_peak)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoServiceFee_off_peak),
                ERR_LEVEL_WARNING,
                "GetServiceFee_off_peak()");
    /*获取尖峰平谷时间段*/
    GetProtoCfgObj(jsProtoObj, &(pProto->info.SegTime_sharp),    jnProtoSegTime_sharp);
    GetProtoCfgObj(jsProtoObj, &(pProto->info.SegTime_peak),     jnProtoSegTime_peak);
    GetProtoCfgObj(jsProtoObj, &(pProto->info.SegTime_shoulder), jnProtoSegTime_shoulder);
    GetProtoCfgObj(jsProtoObj, &(pProto->info.SegTime_off_peak), jnProtoSegTime_off_peak);

#ifdef DEBUG_CFG_PARSE
    printf_safe("********************************\n");
#endif
    cJSON_Delete(jsProtoObj);
    return errcode;
}

/** @brief 设置参数
 *
 * @param jnItemString uint8_t*     要设置参数的名称 或 要设置参数所在的Obj名称
 * @param ObjType uint8_t           要设置参数的类型 或 Obj类型
 * @param jnSubItemString uint8_t*  假如要设置参数在另一个Obj中，则传入这个Obj中该参数的名称
 * @param SubType uint8_t           要设置参数的类型
 * @param pvCfgParam void*          要设置的参数
 * @return ErrorCode_t
 *
 */        
static ErrorCode_t SetProtoCfg(uint8_t *jnItemString, uint8_t ObjType, uint8_t *jnSubItemString, uint8_t SubType, void *pvCfgParam)
{
    cJSON *jsProtoCfgObj;
    cJSON *jsItem;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    jsProtoCfgObj = GetCfgObj(pathProtoCfg, &errcode);
    if(jsProtoCfgObj == NULL)
    {
        return errcode;
    }
    jsItem = jsProtoCfgObj->child;
    do
    {
        if(strcmp(jsItem->string, jnItemString) == 0)
        {
            switch(ObjType)
            {
            case ParamTypeU8:
                cJSON_ReplaceItemInObject(jsProtoCfgObj, jnItemString, cJSON_CreateNumber(*((uint8_t *)pvCfgParam)));
                break;
            case ParamTypeU16:
                cJSON_ReplaceItemInObject(jsProtoCfgObj, jnItemString, cJSON_CreateNumber(*((uint16_t *)pvCfgParam)));
                break;
            case ParamTypeU32:
                cJSON_ReplaceItemInObject(jsProtoCfgObj, jnItemString, cJSON_CreateNumber(*((uint32_t *)pvCfgParam)));
                break;
            case ParamTypeDouble:
                cJSON_ReplaceItemInObject(jsProtoCfgObj, jnItemString, cJSON_CreateNumber(*((double *)pvCfgParam)));
                break;
            case ParamTypeString:
                cJSON_ReplaceItemInObject(jsProtoCfgObj, jnItemString, cJSON_CreateString((uint8_t *)pvCfgParam));
                break;
            case ParamTypeObj:
                //subtype在这里没有使用，因为目前只有uint8一种类型
                cJSON_ReplaceItemInObject(jsItem, jnSubItemString, cJSON_CreateNumber(*((uint8_t *)pvCfgParam)));
                break;
            default:
                break;
            }
            break;//退出while循环
        }
        else
        {
            jsItem = jsItem->next;
        }
    }
    while(jsItem != NULL);
    errcode = SetCfgObj(pathProtoCfg, jsProtoCfgObj);

    return errcode;
}

/** @brief 测试参数设置函数
 *
 * @return void
 *
 */   
void testSetProtoCfg()
{
    uint8_t ucParam;
    uint32_t ulParam;
    ucParam = 4;
    ulParam = 232;
    SetProtoCfg(jnProtoSegTime_sharp, ParamTypeObj, jnProtoSegCont, ParamTypeU8, (void *)&ucParam);
    SetProtoCfg(jnProtoSegTime_sharp, ParamTypeObj, "Start1", ParamTypeU8, (void *)&ucParam);
    SetProtoCfg(jnProtoNewKeyChangeTime, ParamTypeU32, NULL, 0, (void *)&ulParam);
}

/*---------------------------------------------------------------------------/
/                               协议解析
/---------------------------------------------------------------------------*/
static uint16_t echVerifCheck(uint8_t ver, uint8_t atrri, uint16_t cmd, uint32_t len)
{
    return (uint16_t)(ver + atrri + cmd + len);
}
static int sendCommand(void *pPObj, void *pEObj, void *pCObj, uint16_t usSendID, uint32_t timeout, uint8_t trycountmax)
{
    echProtocol_t *pProto;
    echProtoElem_t echSendCmdElem;
    uint8_t pucSendBuffer[REMOTE_SENDBUFF_MAX];
    uint32_t ulSendLength;

    pProto = (echProtocol_t *)pPObj;
    ulSendLength = 0;

    pProto->pCMD[usSendID]->makeProc(pPObj, pEObj, pCObj, pucSendBuffer, &ulSendLength);

    echSendCmdElem.timestamp = time(NULL);
    echSendCmdElem.timeout_s = timeout;
    echSendCmdElem.cmd = pProto->pCMD[usSendID]->CMDType;
    echSendCmdElem.cmd_id = usSendID;
    echSendCmdElem.len = ulSendLength;
    echSendCmdElem.pbuff = pucSendBuffer;
    echSendCmdElem.status = 0;//0表示未发送  1表示已发送
    echSendCmdElem.trycount = 0;
    echSendCmdElem.trycountmax = trycountmax;

    pProto->pCMD[usSendID]->ulRecvdOptLen = 0;
    memset(pProto->pCMD[usSendID]->ucRecvdOptData, 0, REMOTE_RECVDOPTDATA);
    gdsl_list_insert_tail(pProto->plechSendCmd, (void *)&echSendCmdElem);
}
static int makeStdCmd(void *pPObj,
                      void *pEObj,
                      uint16_t usSendID,
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
    uint8_t i;

    us2uc ustmpNetSeq;
    ul2uc ultmpNetSeq;

    pProto = (echProtocol_t *)pPObj;
    pCMD = pProto->pCMD[usSendID];
    pE = (EVSE_t *)pEObj;
    ulMsgHeadLen = 0;

    ulMsgBodyCtxLen_enc = aes_encrypt(pucMsgBodyCtx_dec,
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
    ultmpNetSeq.ulVal = htonl(pE->info.ucIDLength + ulMsgBodyCtxLen_enc);
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[1];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[2];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[3];
    //消息校验码
    ustmpNetSeq.usVal = htons(echVerifCheck(pProto->info.ucProtoVer,
                                            0,
                                            pCMD->CMDType.usSendCmd,
                                            pE->info.ucIDLength + ulMsgBodyCtxLen_enc));
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[1];

    for(i = 0; i < ulMsgHeadLen; i++)
    {
        pucSendBuffer[i] = ucMsgHead[i];
    }
    for(i = 0; i < pE->info.ucIDLength; i++)  //此处借用ulMsgHeadLen表示pucSendBuffer位置，运行之后ulMsgHeadLen表示的就是原ulMsgHeadLen + 桩号长度
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
    uint8_t i;
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

    return 0;
}
static int makeCmdHeart(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdHeartBodyCtx(ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_HEARTBEAT, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
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
    if(pCON->status.xPlugState == UNPLUG)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
    }
    else if(pCON->status.xPlugState == PLUG)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 2;
    }
    else
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 3;
    }
    //接口工作状态 1：充电 2:待机 3：故障 4：充电结束 5：未知
    switch(pCON->state)
    {
    case STATE_CON_CHARGING:
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
        break;
    case STATE_CON_IDLE:
    case STATE_CON_PLUGED:
    case STATE_CON_PRECONTRACT:
    case STATE_CON_PRECONTRACT_LOSEPLUG:
    case STATE_CON_STARTCHARGE: //在这个状态还没开始充电
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 2;
        break;
    case STATE_CON_ERROR:
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 3;
        break;
    case STATE_CON_STOPCHARGE:
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 4;
        break;
    default:
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 5;
        break;
    }
    //输出电压xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingVoltage * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //A B C 相电压
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //输出电流xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingCurrent * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //A B C 相电流
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
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
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //电能表有功电能 xx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingPower * 100));
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
    if(pEVSE->status.ulScramState == 1)
    {
        errcode |= 1 << 0; //Bit0 急停故障
    }
    uxBits = xEventGroupGetBits(pCON->status.xHandleEventException);
    if((uxBits & defEventBitExceptionMeter) == defEventBitExceptionMeter)
    {
        errcode |= 1 << 1; //Bit1 电表故障
    }
    if((uxBits & defEventBitExceptionRelayPaste) == defEventBitExceptionRelayPaste)
    {
        errcode |= 1 << 2; //Bit2 接触器故障
    }
    if((uxBits & defEventBitExceptionRFID) == defEventBitExceptionRFID)
    {
        errcode |= 1 << 3; //Bit3 读卡器故障
    }
    uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
    if((uxBits & defEventBitCONACTempOK) != defEventBitCONACTempOK)
    {
        errcode |= 1 << 4; //Bit4 内部过温故障
    }
    //errcode |= 1 << 5 //Bit5 连接器故障
    if(pEVSE->status.ulPEState == 1)
    {
        errcode |= 1 << 6; //Bit5 绝缘故障
    }
    if(pEVSE->status.ulPowerOffState == 1)
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

    return 0;
}
static int makeCmdStatus(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdStatusBodyCtx(pEObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_STATUS, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}

static int makeCmdRemoteCtrlBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_REMOTE_CTRL]->ucRecvdOptData;
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

    return 0;
}
static int makeCmdRemoteCtrl(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdRemoteCtrlBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REMOTE_CTRL, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}
static int makeCmdRTDataBodyCtx(void *pPObj, void *pCObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    CON_t *pCON;
    uint8_t *pbuff;
    uint8_t ucOrderSN[8];
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    us2uc ustmpNetSeq;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pCON = (CON_t *)pCObj;
    pbuff = pProto->pCMD[ECH_CMDID_RTDATA]->ucRecvdOptData;
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
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[13...16] 尖电量 xxx.xx
    //[17...20] 峰电量
    //[21...24] 平电量
    //[25...28] 谷电量
    for(i = 0; i < 16; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }
    //[29...32] 当前充电金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPowerFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[32...36] 当前服务费金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServiceFee * 100));
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
    //[44...47] 输出电压 xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingVoltage * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[48...51] 输出电流 xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingCurrent * 10));
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

    return 0;
}
static int makeCmdRTData(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdRTDataBodyCtx(pPObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_RTDATA, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}
static int makeCmdOrderBodyCtx(void *pPObj, void *pCObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    CON_t *pCON;
    uint8_t *pbuff;
    uint8_t ucOrderSN[8];
    uint8_t strCardID[17];
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    us2uc ustmpNetSeq;
    uint8_t reason;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pCON = (CON_t *)pCObj;
    pbuff = pProto->pCMD[ECH_CMDID_ORDER]->ucRecvdOptData;
    ulMsgBodyCtxLen_dec = 0;

    //[0] 有卡 04 无卡05
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    //[1...8] 交易流水号
    StrToHex(pCON->order.strOrderSN, ucOrderSN, strlen(pCON->order.strOrderSN));
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    //[9] 充电桩接口
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.ucCONID + 1;
    //[10...25] 卡号
    if(pbuff[0] == 4)
    {
        HexToStr(pCON->order.ucCardID, strCardID, 8);
        for(i = 0; i < 16; i++)
        {
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = strCardID[i];
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
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dStartPower * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[30...33] 充电后电能总示值 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)((pCON->order.dStartPower + pCON->order.dTotalPower) * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[34...37] 本次充电电费总金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPowerFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[38...41] 本次充电服务费总金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServiceFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[42...45] 尖电价       xx.xxxx
    //[46...49] 尖服务费单价 xx.xxxx
    //[50...53] 尖电量       xxx.xx
    //[54...57] 尖充电金额   xxx.xx
    //[58...61] 尖服务费金额 xxx.xx
    //[62,63] 尖充电时长 xx
    //[64...85]峰
    //[86...107]平
    //[108...129]谷
    for(i = 0; i < 22 * 4; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }
    //[130...133] 充电开始时间
    ultmpNetSeq.ulVal = htonl(pCON->order.tStartTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[134,135] 充电持续时间
    ustmpNetSeq.usVal = htons(pCON->order.tStopTime - pCON->order.tStartTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[136] 停止充电原因
    switch(pCON->order.ucStopType)
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
    case defOrderStopType_NetLost:
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
    ultmpNetSeq.ulVal = htonl(pCON->order.tStopTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 0;
}
static int makeCmdOrder(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdOrderBodyCtx(pPObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_ORDER, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}
#define ECH_ERR_VER     -1
#define ECH_ERR_CHECK   -2
#define ECH_ERR_ID      -3

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
//    if(pbuff[ulOffset] != pProto->info.ucProtoVer)
//    {
//        return ECH_ERR_VER;
//    }

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
    switch(echRecvCmdElem.cmd.usRecvCmd)
    {
    case 2://主机回复的命令，不需要timeout 单位s。
        echRecvCmdElem.cmd_id = ECH_CMDID_REGISTER;
        echRecvCmdElem.timeout_s =  0;
        break;
    case 4:
        echRecvCmdElem.cmd_id = ECH_CMDID_HEARTBEAT;
        echRecvCmdElem.timeout_s =  0;
        break;
    case 42:
        echRecvCmdElem.cmd_id = ECH_CMDID_STATUS;
        echRecvCmdElem.timeout_s =  30;
        break;
    case 43:
        echRecvCmdElem.cmd_id = ECH_CMDID_REMOTE_CTRL;
        echRecvCmdElem.timeout_s =  30;
        break;
    case 47:
        echRecvCmdElem.cmd_id = ECH_CMDID_ORDER;
        echRecvCmdElem.timeout_s =  30;
        break;
    default:
        break;
    }
    echRecvCmdElem.len = ulMsgBodyLen_enc + 14;
    echRecvCmdElem.pbuff = pbuff;
    echRecvCmdElem.status = 0;
    echRecvCmdElem.trycount = 0;
    echRecvCmdElem.trycountmax = trycountmax;

    gdsl_list_insert_tail(pProto->plechRecvCmd, (void *)&echRecvCmdElem);

    if(ulRecvdLen - ulOffset > echRecvCmdElem.len)
    {
        recvResponse(pPObj, pEObj, &pbuff[ulOffset + echRecvCmdElem.len], ulRecvdLen - ulOffset - echRecvCmdElem.len, 3);
    }
    return 1;
}

/** @brief
 *
 * @param pPObj void*
 * @param usSendID uint16_t （这里可能会产生理解问题）SendID表示收到的回复对应的SendID
 * @param pbuff uint8_t*
 * @param ulRecvLen uint32_t
 * @return int
 *
 */
static int analyStdRes(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
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
    pMsgBodyCtx_dec = (uint8_t *)malloc(ulMsgBodyCtxLen_enc * sizeof(uint8_t));

    aes_decrypt(pMsgBodyCtx_enc, pProto->info.strKey, pMsgBodyCtx_dec, ulMsgBodyCtxLen_enc);

    memmove(pProto->pCMD[usSendID]->ucRecvdOptData, pMsgBodyCtx_dec, ulMsgBodyCtxLen_enc);
    pProto->pCMD[usSendID]->ulRecvdOptLen = ulMsgBodyCtxLen_enc;

    free(pMsgBodyCtx_dec);

    return 1;
}

/*
写分析函数的方法
1. 等待Mutex
2. 初始化lRecvElem
3. 将lRecvElem插入队尾
4. 释放Mutex
*/

static int analyCmdCommon(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    echProtocol_t *pProto;
    echCMD_t *pCMD;
    echCmdElem_t lRecvElem;

    pProto = (echProtocol_t *)pPObj;
    pCMD = pProto->pCMD[usSendID];

    if(xSemaphoreTake(pCMD->xMutexCmd, 10000) == pdTRUE)
    {
        analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);

        lRecvElem.UID = 0;
        lRecvElem.timestamp = time(NULL);
        lRecvElem.len = pCMD->ulRecvdOptLen;
        lRecvElem.pbuff = pCMD->ucRecvdOptData;
        lRecvElem.status = 0;
        gdsl_list_insert_tail(pCMD->plRecvCmd, (void *)&lRecvElem);

        xSemaphoreGive(pCMD->xMutexCmd);
    }

    return 1;
}
#if 0
static int analyCmdReg(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    echProtocol_t *pProto;
    echCMD_t *pCMD;
    echCmdElem_t lRecvElem;

    pProto = (echProtocol_t *)pPObj;
    pCMD = pProto->pCMD[usSendID];

    if(xSemaphoreTake(pCMD->xMutexCmd, 10000) == pdTRUE)
    {
        analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);

        lRecvElem.UID = 0;
        lRecvElem.timestamp = time(NULL);
        lRecvElem.len = pCMD->ulRecvdOptLen;
        lRecvElem.pbuff = pCMD->ucRecvdOptData;
        lRecvElem.status = 0;
        gdsl_list_insert_tail(pCMD->plRecvCmd, (void *)&lRecvElem);

        xSemaphoreGive(pCMD->xMutexCmd);
    }

    return 1;
}
#endif
static int analyCmdHeart(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    echProtocol_t *pProto;
    echCMD_t *pCMD;
    uint8_t *pMsgBodyCtx_dec;
    ul2uc ultmpNetSeq;
    time_t timestamp;
    echCmdElem_t lRecvElem;

    pProto = (echProtocol_t *)pPObj;
    pCMD = pProto->pCMD[usSendID];
    if(xSemaphoreTake(pCMD->xMutexCmd, 10000) == pdTRUE)
    {
        analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);
        pMsgBodyCtx_dec = pCMD->ucRecvdOptData;

        ultmpNetSeq.ucVal[0] = pMsgBodyCtx_dec[0];
        ultmpNetSeq.ucVal[1] = pMsgBodyCtx_dec[1];
        ultmpNetSeq.ucVal[2] = pMsgBodyCtx_dec[2];
        ultmpNetSeq.ucVal[3] = pMsgBodyCtx_dec[3];
        timestamp = (time_t)ntohl(ultmpNetSeq.ulVal);
        if(utils_abs(timestamp - time(NULL)) > 5)//大于5s进行校时
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
    }

    return 1;
}
#if 0
static int analyCmdStatus(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);

    return 1;
}
static int analyCmdRemoteCtrl(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);

    return 1;
}
static int analyCmdRTData(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);
    return 1;
}
static int analyCmdOrder(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);
    return 1;
}
#endif
static gdsl_element_t echCmdListAlloc(gdsl_element_t e)
{
    echCmdElem_t *copyCmdElem;
    copyCmdElem = (echCmdElem_t *)malloc(sizeof(echCmdElem_t));
    if(copyCmdElem != NULL)
    {
        memmove(copyCmdElem, e, sizeof(echCmdElem_t));
    }
    else
    {
        return NULL;
    }
    copyCmdElem->pbuff = (uint8_t *)malloc(copyCmdElem->len * sizeof(uint8_t));
    if(copyCmdElem->pbuff != NULL)
    {
        memmove(copyCmdElem->pbuff, ((echCmdElem_t *)e)->pbuff, copyCmdElem->len);
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
        memmove(copyProtoElem, e, sizeof(echProtoElem_t));
    }
    else
    {
        return NULL;
    }
    copyProtoElem->pbuff = (uint8_t *)malloc(copyProtoElem->len * sizeof(uint8_t));
    if(copyProtoElem->pbuff != NULL)
    {
        memmove(copyProtoElem->pbuff, ((echProtoElem_t *)e)->pbuff, copyProtoElem->len);
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
    free(pProto);
    pProto = NULL;
}
static echCMD_t *EchCMDCreate(uint16_t usSendCmd, uint16_t usRecvCmd, pECH_MAKE_PROC makeProc, pECH_ANALY_PROC analyProc)
{
    echCMD_t *pECHCMD = (echCMD_t *)malloc(sizeof(echCMD_t));
    if(pECHCMD == NULL)
    {
        return NULL;
    }
    pECHCMD->CMDType.usSendCmd = usSendCmd;
    pECHCMD->CMDType.usRecvCmd = usRecvCmd;
    pECHCMD->xHandleEventCmd = xEventGroupCreate();
    pECHCMD->xMutexCmd = xSemaphoreCreateMutex();

    pECHCMD->plRecvCmd = gdsl_list_alloc ("CMDRecvLis", echCmdListAlloc, echCmdListFree);
    pECHCMD->makeProc = makeProc;
    pECHCMD->analyProc = analyProc;

    return pECHCMD;
}

echProtocol_t *EchProtocolCreate(void)
{
    int i;
    echProtocol_t *pProto = (echProtocol_t *)malloc(sizeof(echProtocol_t));
    if(pProto == NULL)
    {
        return NULL;
    }
    strcpy(pProto->info.strServerIP, "123.56.113.124");//"124.207.112.70");//
    pProto->info.usServerPort      = 6677;//8051;//
    strcpy(pProto->info.strUserName, "esaasusr");
    strcpy(pProto->info.strUserPwd, "esaaspasswrd");
    strcpy(pProto->info.strKey, "0123456789abcdeg");
//   strcpy(pProto->info.strKey, "1234567890abcde2");
    memset(pProto->info.strNewKey, 0, 17);
    pProto->info.tNewKeyChangeTime = 0;
    pProto->info.ucProtoVer        = 0x68;
    pProto->info.ulHeartBeatCyc_ms = 15000;
    pProto->info.ucResetAct        = 0;

    pProto->info.ulPowerFee_sharp    = 0; //尖峰费率 系数0.0001
    pProto->info.ulPowerFee_peak     = 0; //峰
    pProto->info.ulPowerFee_shoulder = 0; //平
    pProto->info.ulPowerFee_off_peak = 0; //谷

    pProto->info.ulServiceFee_sharp    = 0; //系数0.0001
    pProto->info.ulServiceFee_peak     = 0;
    pProto->info.ulServiceFee_shoulder = 0;
    pProto->info.ulServiceFee_off_peak = 0;

    pProto->info.ulStatusCyc_ms = 20000; //状态数据上报间隔
    pProto->info.ulRTDataCyc_ms = 10000; //实时数据上报间隔  10s

    pProto->info.GetProtoCfg = GetProtoCfg;
    pProto->info.SetProtoCfg = SetProtoCfg;


    for(i = 0; i < ECH_CMD_MAX; i++)
    {
        pProto->pCMD[i] = NULL;
    }
    //桩命令, 平台命令
    pProto->pCMD[ECH_CMDID_REGISTER]    = EchCMDCreate(1,  2,  makeCmdReg,        analyCmdCommon);
    pProto->pCMD[ECH_CMDID_HEARTBEAT]   = EchCMDCreate(3,  4,  makeCmdHeart,      analyCmdHeart);
    pProto->pCMD[ECH_CMDID_STATUS]      = EchCMDCreate(41, 42, makeCmdStatus,     analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REMOTE_CTRL] = EchCMDCreate(44, 43, makeCmdRemoteCtrl, analyCmdCommon);
    pProto->pCMD[ECH_CMDID_RTDATA]      = EchCMDCreate(45, 0,  makeCmdRTData,     analyCmdCommon);
    pProto->pCMD[ECH_CMDID_ORDER]       = EchCMDCreate(46, 47, makeCmdOrder,      analyCmdCommon);

    pProto->recvResponse = recvResponse;
    pProto->sendCommand = sendCommand;
    pProto->deleteProtocol = deleteProto;

    pProto->plechRecvCmd = gdsl_list_alloc ("ProtoRecvLis", echProtoListAlloc, echProtoListFree);
    pProto->plechSendCmd = gdsl_list_alloc ("ProtoSendLis", echProtoListAlloc, echProtoListFree);

    return pProto;
}



//encode/decode protocol requests
//encode/decode protocol responses

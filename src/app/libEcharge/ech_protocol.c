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
#include "libEcharge/ech_globals.h"

#include <string.h>
#include "stringName.h"
#include "cfg_parse.h"
#include "cJSON.h"

/*---------------------------------------------------------------------------/
/                               ��ȡЭ��������Ϣ
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

    //  ����Int
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

    //  ����string
    jsItem = cJSON_GetObjectItem(pProtoCfgObj, jnName);
    if(jsItem == NULL)
    {
        errcode = ERR_FILE_PARSE;
        goto err_return;
    }
    ptmpStr = utils_strdup(jsItem->valuestring); //strdup֮����ִ���Ҫ��free
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
/** @brief ��ȡProtoCfg�в�����ֵ
 *
 * @param pvProtoInfoItem void* ����Ҫ��ȡ�Ĳ�����ָ��
 * @param type uint8_t Ҫ��ȡ����������
 * @param pvCfgObj void*
 * @param jnItemName uint8_t* ��������
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

    //  ����
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
        *(uint32_t *)pvProtoInfoItem = (uint32_t)jsItem; //�ܲ�ϲ������Ͷ��ȡ�ɣ�ӦΪ�����������һ������ָ��
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
/** @brief ��ȡProto��Obj���Ӳ���
 *
 * @param jsProtoObj cJSON*     Obj�ĸ�Obj
 * @param pSegTime EchSegTime_t* Obj��Ӧ��ʱ��νṹ��
 * @param jnNameObj uint8_t*    Obj������
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
/** @brief ��ȡprotocol.cfgȫ������
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

    /*json����*/
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
                errcode = GetProtoCfgItem((void *)(&(pProto->info.ulOptSN)),
                                          ParamTypeU32,
                                          jsProtoObj,
                                          jnProtoOptSN),
                ERR_LEVEL_WARNING,
                "GetOptSN()");
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
                errcode = GetProtoCfgItem((void *)(&(pProto->info.dPowerFee_sharp)),
                                          ParamTypeDouble,
                                          jsProtoObj,
                                          jnProtoPowerFee_sharp),
                ERR_LEVEL_WARNING,
                "GetPowerFee_sharp()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.dPowerFee_peak)),
                                          ParamTypeDouble,
                                          jsProtoObj,
                                          jnProtoPowerFee_peak),
                ERR_LEVEL_WARNING,
                "GetPowerFee_peak()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.dPowerFee_shoulder)),
                                          ParamTypeDouble,
                                          jsProtoObj,
                                          jnProtoPowerFee_shoulder),
                ERR_LEVEL_WARNING,
                "GetPowerFee_shoulder()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.dPowerFee_off_peak)),
                                          ParamTypeDouble,
                                          jsProtoObj,
                                          jnProtoPowerFee_off_peak),
                ERR_LEVEL_WARNING,
                "GetPowerFee_off_peak()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.dServFee_sharp)),
                                          ParamTypeDouble,
                                          jsProtoObj,
                                          jnProtoServFee_sharp),
                ERR_LEVEL_WARNING,
                "GetServiceFee_sharp()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.dServFee_peak)),
                                          ParamTypeDouble,
                                          jsProtoObj,
                                          jnProtoServFee_peak),
                ERR_LEVEL_WARNING,
                "GetServiceFee_peak()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.dServFee_shoulder)),
                                          ParamTypeDouble,
                                          jsProtoObj,
                                          jnProtoServFee_shoulder),
                ERR_LEVEL_WARNING,
                "GetServiceFee_shoulder()");
    THROW_ERROR(defDevID_File,
                errcode = GetProtoCfgItem((void *)(&(pProto->info.dServFee_off_peak)),
                                          ParamTypeDouble,
                                          jsProtoObj,
                                          jnProtoServFee_off_peak),
                ERR_LEVEL_WARNING,
                "GetServiceFee_off_peak()");
    /*��ȡ���ƽ��ʱ���*/
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

/** @brief ���ò���
 *
 * @param jnItemString uint8_t*     Ҫ���ò��������� �� Ҫ���ò������ڵ�Obj����
 * @param ObjType uint8_t           Ҫ���ò��������� �� Obj����
 * @param jnSubItemString uint8_t*  ����Ҫ���ò�������һ��Obj�У��������Obj�иò���������
 * @param SubType uint8_t           Ҫ���ò���������
 * @param pvCfgParam void*          Ҫ���õĲ���
 * @return ErrorCode_t
 *
 */
static ErrorCode_t SetProtoCfg(const uint8_t *jnItemString, uint8_t ObjType, const uint8_t *jnSubItemString, uint8_t SubType, void *pvCfgParam)
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
                //subtype������û��ʹ�ã���ΪĿǰֻ��uint8һ������
                cJSON_ReplaceItemInObject(jsItem, jnSubItemString, cJSON_CreateNumber(*((uint8_t *)pvCfgParam)));
                break;
            default:
                break;
            }
            break;//�˳�whileѭ��
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

/** @brief ���Բ������ú���
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
/                               Э�����
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
    echSendCmdElem.status = 0;//0��ʾδ����  1��ʾ�ѷ���
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

    //Э��汾
    ucMsgHead[ulMsgHeadLen++] = pProto->info.ucProtoVer;
    //��Ϣ������
    ucMsgHead[ulMsgHeadLen++] = 0x00;
    //������
    ustmpNetSeq.usVal = htons(pCMD->CMDType.usSendCmd);
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[1];
    //Ԥ���ֶ�
    ultmpNetSeq.ulVal = 0;
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[1];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[2];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[3];
    //��Ϣ�峤��
    ultmpNetSeq.ulVal = htonl(pE->info.ucIDLength + ulMsgBodyCtxLen_enc);
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[1];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[2];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[3];
    //��ϢУ����
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
    for(i = 0; i < pE->info.ucIDLength; i++)  //�˴�����ulMsgHeadLen��ʾpucSendBufferλ�ã�����֮��ulMsgHeadLen��ʾ�ľ���ԭulMsgHeadLen + ׮�ų���
    {
        pucSendBuffer[ulMsgHeadLen++] = pE->info.strID[i];
    }
    for(i = 0; i < ulMsgBodyCtxLen_enc; i++)
    {
        pucSendBuffer[ulMsgHeadLen + i] = ucMsgBodyCtx_enc[i];
    }

    *pulSendLength = ulMsgHeadLen + ulMsgBodyCtxLen_enc; //�˴�ulMsgHeadLen�Ѿ�������׮�ų���

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

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

    return 0;
}
static int makeCmdHeart(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdHeartBodyCtx(ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_HEARTBEAT, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}

static int makeCmdResetBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_RESET]->ucRecvdOptData;
    ulMsgBodyCtxLen_dec = 0;

    for(i = 0; i < 4; i++)
    {
        //[0...3] ����ID
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[i];
    }
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[4]; //�ɹ���־

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

    return 0;
}
static int makeCmdReset(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdResetBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_RESET, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
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

    //���׮�ӿ�   0��Ĭ�� 1��A 2��B
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.ucCONID + 1;
    //ԤԼ״̬ 1����ԤԼ  2:��ԤԼ
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
    //��λ״̬ 1������   2��ռ��   3��δ֪
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 3;
    //�ӿ�����״̬  1 ���У� 2,������ 3 δ֪
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
    //�ӿڹ���״̬ 1����� 2:���� 3������ 4�������� 5��δ֪
    switch(pCON->state)
    {
    case STATE_CON_CHARGING:
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
        break;
    case STATE_CON_IDLE:
    case STATE_CON_PLUGED:
    case STATE_CON_PRECONTRACT:
    case STATE_CON_PRECONTRACT_LOSEPLUG:
    case STATE_CON_STARTCHARGE: //�����״̬��û��ʼ���
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
    //�����ѹxxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingVoltage * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //A B C ���ѹ
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
    //�������xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingCurrent * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //A B C �����
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
    //�̵���״̬ 1����2��
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
    //ϵͳ�������״̬ 1������ 2���ػ�
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //׮ʵʱ�¶� xx.xxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dACLTemp * 1000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //�й����� �޹����� xx.xxxx
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //���ܱ��й����� xx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingPower * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //����޹����� xx.xx
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //SOC 1~100
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //������
    if(pEVSE->status.ulScramState == 1)
    {
        errcode |= 1 << 0; //Bit0 ��ͣ����
    }
    uxBits = xEventGroupGetBits(pCON->status.xHandleEventException);
    if((uxBits & defEventBitExceptionMeter) == defEventBitExceptionMeter)
    {
        errcode |= 1 << 1; //Bit1 ������
    }
    if((uxBits & defEventBitExceptionRelayPaste) == defEventBitExceptionRelayPaste)
    {
        errcode |= 1 << 2; //Bit2 �Ӵ�������
    }
    if((uxBits & defEventBitExceptionRFID) == defEventBitExceptionRFID)
    {
        errcode |= 1 << 3; //Bit3 ����������
    }
    uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
    if((uxBits & defEventBitCONACTempOK) != defEventBitCONACTempOK)
    {
        errcode |= 1 << 4; //Bit4 �ڲ����¹���
    }
    //errcode |= 1 << 5 //Bit5 ����������
    if(pEVSE->status.ulPEState == 1)
    {
        errcode |= 1 << 6; //Bit5 ��Ե����
    }
    if(pEVSE->status.ulPowerOffState == 1)
    {
        errcode |= 1 << 7;  //Bit7 ����(�ڴ˶���Ϊͣ�����)
    }
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = errcode;
    //���׮��ǰʱ��
    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

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
        //[0...3] ����ID
        //[4...11] ������ˮ��
        //[12] ���׮�ӿ�
        //[13] ���׮����
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[i]; //����
    }
    //[14] ��ͣ���
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[14];//�ڷ�������֮ǰ��ֵ��
    //[15] ʧ��ԭ��
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[15];//�ڷ�������֮ǰ��ֵ��
    //[16] SOC
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

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

    //[0...7] ������ˮ��
    StrToHex(pCON->order.strOrderSN, ucOrderSN, strlen(pCON->order.strOrderSN));
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    //[8] ׮�ӿ�
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.ucCONID + 1;
    //[9...12] ��ǰ����ܵ��� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[13...16] ����� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower_sharp * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[17...20] �����
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower_peak * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[21...24] ƽ����
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower_shoulder * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[25...28] �ȵ���
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower_off_peak * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[29...32] ��ǰ����� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPowerFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[32...36] ��ǰ����ѽ�� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[37,38] ��ǰ���ʱ��
    ustmpNetSeq.usVal = htons(  time(NULL) - pCON->order.tStartTime  );
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[39] ���׮״̬
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[39];//ע���޸�����ҲҪ�޸�interface_remote.c��Ӧλ��
    //[40] ֹͣԭ��
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[40];
    //[41] ��ǰSOC
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[42,43] ʣ����ʱ��
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[44...47] �����ѹ xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingVoltage * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[48...51] ������� xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingCurrent * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[52...55] ���������¶�
    //[56...59] ���������¶�
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }
    //[60...63] ��ǰʱ���
    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[64...80] ����VIN��
    for(i = 0; i < 17; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

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

    //[0] �п� 04 �޿�05
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    //[1...8] ������ˮ��
    StrToHex(pCON->order.strOrderSN, ucOrderSN, strlen(pCON->order.strOrderSN));
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    //[9] ���׮�ӿ�
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.ucCONID + 1;
    //[10...25] ����
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
    //[26...29] ���ǰ�ܵ���ʾֵ xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dStartPower * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[30...33] ���������ʾֵ xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)((pCON->order.dStartPower + pCON->order.dTotalPower) * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[34...37] ���γ�����ܽ�� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPowerFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[38...41] ���γ�������ܽ�� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[42...45] ����       xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dPowerFee_sharp * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[46...49] �����ѵ��� xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dServFee_sharp * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[50...53] �����       xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower_sharp * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[54...57] ������   xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPowerFee_sharp * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[58...61] �����ѽ�� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServFee_sharp * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[62,63] ����ʱ�� xx
    ustmpNetSeq.usVal = htons((uint16_t)(pCON->order.ulTotalTime_sharp));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[64...85]��
    //[64...67] ����       xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dPowerFee_peak * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[68...71] �����ѵ��� xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dServFee_peak * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[72...75] �����       xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower_peak * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[76...79] ������   xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPowerFee_peak * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[80...83] �����ѽ�� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServFee_peak * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[84,85] ����ʱ�� xx
    ustmpNetSeq.usVal = htons((uint16_t)(pCON->order.ulTotalTime_peak));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[86...107]ƽ
    //[86...89] ƽ���       xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dPowerFee_shoulder * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[90...93] ƽ����ѵ��� xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dServFee_shoulder * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[94...97] ƽ����       xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower_shoulder * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[98...101] ƽ�����   xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPowerFee_shoulder * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[102...105] ƽ����ѽ�� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServFee_shoulder * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[106,107] ƽ���ʱ�� xx
    ustmpNetSeq.usVal = htons((uint16_t)(pCON->order.ulTotalTime_shoulder));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[108...129]��
    //[108...111] �ȵ��       xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dPowerFee_off_peak * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[112...115] �ȷ���ѵ��� xx.xxxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dServFee_off_peak * 10000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[116...119] �ȵ���       xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower_off_peak * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[120...123] �ȳ����   xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPowerFee_off_peak * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[124...127] �ȷ���ѽ�� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServFee_off_peak * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[128,129] �ȳ��ʱ�� xx
    ustmpNetSeq.usVal = htons((uint16_t)(pCON->order.ulTotalTime_off_peak));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[130...133] ��翪ʼʱ��
    ultmpNetSeq.ulVal = htonl(pCON->order.tStartTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[134,135] ������ʱ��
    ustmpNetSeq.usVal = htons(pCON->order.tStopTime - pCON->order.tStartTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[136] ֹͣ���ԭ��
    switch(pCON->order.ucStopType)
    {
    case defOrderStopType_RFID:
    case defOrderStopType_Remote:
        reason = 1;//�ֶ�ֹͣ
        break;
    case defOrderStopType_Full:
        reason = 3;//����ֹͣ
        break;
    case defOrderStopType_Fee:
        reason = 4;//�ﵽ�����
        break;
    case defOrderStopType_Scram:
    case defOrderStopType_NetLost:
    case defOrderStopType_Poweroff:
    case defOrderStopType_OverCurr:
    case defOrderStopType_Knock:
        reason = 5;//�쳣ֹͣ
        break;
    default:
        reason = 6;//����ԭ��ֹͣ
        break;
    }
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = reason;
    //[137] ��ǰsoc
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[138] ״̬
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[139...142] ������ʱ��
    ultmpNetSeq.ulVal = htonl(pCON->order.tStopTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

    return 0;
}
static int makeCmdOrder(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdOrderBodyCtx(pPObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_ORDER, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}
static int makeCmdSetResBodyCtx(void *pPObj, uint16_t usSendID, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[usSendID]->ucRecvdOptData;

    //[0...3] ����ID
    pucMsgBodyCtx_dec[0] = pbuff[0];
    pucMsgBodyCtx_dec[1] = pbuff[1];
    pucMsgBodyCtx_dec[2] = pbuff[2];
    pucMsgBodyCtx_dec[3] = pbuff[3];

    *pulMsgBodyCtxLen_dec = 4; //��Ҫ���Ǹ�ֵ

    return 0;
}
static int makeCmdSetSucc(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdSetResBodyCtx(pPObj, ECH_CMDID_SET_SUCC, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_SET_SUCC, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}
static int makeCmdSetFail(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdSetResBodyCtx(pPObj, ECH_CMDID_SET_FAIL, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_SET_FAIL, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}
static int makeCmdReqFeeBodyCtx(void *pPObj, uint16_t usSendID, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[usSendID]->ucRecvdOptData;
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] ����ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4...7]���׮��ǰʱ��
    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    switch(usSendID)
    {
    case ECH_CMDID_REQ_POWERFEE:
        //[8...11] ��
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dPowerFee_sharp * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[12...15] ��
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dPowerFee_peak * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[16...19] ƽ
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dPowerFee_shoulder * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[20...23] ��
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dPowerFee_off_peak * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        break;
    case ECH_CMDID_REQ_SERVFEE:
        //[8...11] ��
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dServFee_sharp * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[12...15] ��
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dServFee_peak * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[16...19] ƽ
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dServFee_shoulder * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        //[20...23] ��
        ultmpNetSeq.ulVal = htonl((uint32_t)(pProto->info.dServFee_off_peak * 10000));
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
        break;
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ
}
static int makeCmdReqPowerFee(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdReqFeeBodyCtx(pPObj, ECH_CMDID_REQ_POWERFEE, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_POWERFEE, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}
static int makeCmdReqServFee(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdReqFeeBodyCtx(pPObj, ECH_CMDID_REQ_SERVFEE, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_SERVFEE, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}
static int makeCmdReqCycBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;


    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_REQ_CYC]->ucRecvdOptData;
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] ����ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4] ״̬�ϱ����
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = (uint8_t)(pProto->info.ulStatusCyc_ms / 1000);
    //[5] ʵʱ�����ϱ����
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = (uint8_t)(pProto->info.ulRTDataCyc_ms / 1000);

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ
}
static int makeCmdReqCyc(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdReqCycBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_CYC, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}

static int makeCmdReqTimeSegBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_REQ_TIMESEG]->ucRecvdOptData;
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] ����ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //��ʱ��θ���
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_sharp.ucSegCont;
    //��ʱ��ֹʱ���
    for(i = 0; i < pProto->info.SegTime_sharp.ucSegCont; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_sharp.ucStart[i];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_sharp.ucEnd[i];
    }
    //��ʱ��θ���
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_peak.ucSegCont;
    //��ʱ��ֹʱ���
    for(i = 0; i < pProto->info.SegTime_peak.ucSegCont; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_peak.ucStart[i];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_peak.ucEnd[i];
    }

    //ƽʱ��θ���
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_shoulder.ucSegCont;
    //ƽʱ��ֹʱ���
    for(i = 0; i < pProto->info.SegTime_shoulder.ucSegCont; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_shoulder.ucStart[i];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_shoulder.ucEnd[i];
    }

    //��ʱ��θ���
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_off_peak.ucSegCont;
    //��ʱ��ֹʱ���
    for(i = 0; i < pProto->info.SegTime_off_peak.ucSegCont; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_off_peak.ucStart[i];
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.SegTime_off_peak.ucEnd[i];
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ
}
static int makeCmdReqTimeSeg(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdReqTimeSegBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_TIMESEG, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}


static int makeCmdReqKeyBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_REQ_KEY]->ucRecvdOptData;
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] ����ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4...19] ��Կ
    for(i = 0; i < 16; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.strKey[i];
    }
    //[20...23] ��Կ���ʱ��
    ultmpNetSeq.ulVal = htonl(pProto->info.tNewKeyChangeTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

}
static int makeCmdReqKey(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdReqKeyBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_KEY, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
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
    pbuff = pProto->pCMD[ECH_CMDID_REQ_SOFTVER]->ucRecvdOptData;
    ulMsgBodyCtxLen_dec = 0;

    //[0...3] ����ID
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[3];
    //[4...13] ����汾��
    for(i = 0; i < defEchProtoSoftVerLen; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pEVSE->info.strSoftVer[i]; //3.9.3135
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ
}
static int makeCmdReqSoftVer(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdReqSoftVerBodyCtx(pPObj, pEObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REQ_SOFTVER, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
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
    return 0;
}
#define ECH_ERR_OK      1
#define ECH_ERR_VER     -1
#define ECH_ERR_CHECK   -2
#define ECH_ERR_ID      -3
#define ECH_ERR_CMDID   -4

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

    //pbuff[1] ����

    //pbuff[2,3] ������
    ustmpNetSeq.ucVal[0] = pbuff[ulOffset + 2];
    ustmpNetSeq.ucVal[1] = pbuff[ulOffset + 3];
    echRecvCmdElem.cmd.usRecvCmd = ntohs(ustmpNetSeq.usVal);

    //pbuff[4...7] Ԥ���ֶ�

    //pbuff[8...11] ��Ϣ�峤��
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

    gdsl_list_insert_tail(pProto->plechRecvCmd, (void *)&echRecvCmdElem);

    if(ulRecvdLen - ulOffset > echRecvCmdElem.len)
    {
        recvResponse(pPObj, pEObj, &pbuff[ulOffset + echRecvCmdElem.len], ulRecvdLen - ulOffset - echRecvCmdElem.len, 3);
    }
    return ECH_ERR_OK;
}

/** @brief
 *
 * @param pPObj void*
 * @param usSendID uint16_t ��������ܻ����������⣩SendID��ʾ�յ��Ļظ���Ӧ��SendID
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
    pMsgBodyCtx_enc = pbuff + 30;         //ȡ�����ܲ���buff
    ulMsgBodyCtxLen_enc = ulRecvLen - 30; //���ܲ��ֳ���
    pMsgBodyCtx_dec = (uint8_t *)malloc(ulMsgBodyCtxLen_enc * sizeof(uint8_t));

    aes_decrypt(pMsgBodyCtx_enc, pProto->info.strKey, pMsgBodyCtx_dec, ulMsgBodyCtxLen_enc);

    memmove(pProto->pCMD[usSendID]->ucRecvdOptData, pMsgBodyCtx_dec, ulMsgBodyCtxLen_enc);
    pProto->pCMD[usSendID]->ulRecvdOptLen = ulMsgBodyCtxLen_enc;

    free(pMsgBodyCtx_dec);

    return 1;
}

/*
д���������ķ���
1. �ȴ�Mutex
2. ��ʼ��lRecvElem
3. ��lRecvElem�����β
4. �ͷ�Mutex
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
        if(utils_abs(timestamp - time(NULL)) > 5)//����5s����Уʱ
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
/** @brief ���ƴ������Ԫ�ص�������Ŀռ�
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
    pProto->info.ulOptSN           = 0;
    pProto->info.ucProtoVer        = 0x68;
    pProto->info.ulHeartBeatCyc_ms = 15000;
    pProto->info.ucResetAct        = 0;

    pProto->info.dPowerFee_sharp    = 0; //������
    pProto->info.dPowerFee_peak     = 0; //��
    pProto->info.dPowerFee_shoulder = 0; //ƽ
    pProto->info.dPowerFee_off_peak = 0; //��

    pProto->info.dServFee_sharp    = 0;
    pProto->info.dServFee_peak     = 0;
    pProto->info.dServFee_shoulder = 0;
    pProto->info.dServFee_off_peak = 0;

    pProto->info.ulStatusCyc_ms = 20000; //״̬�����ϱ����
    pProto->info.ulRTDataCyc_ms = 10000; //ʵʱ�����ϱ����  10s

    pProto->info.GetProtoCfg = GetProtoCfg;
    pProto->info.SetProtoCfg = SetProtoCfg;


    for(i = 0; i < ECH_CMD_MAX; i++)
    {
        pProto->pCMD[i] = NULL;
    }

    /* @todo (rgw#1#): �������ʱ���������Ѿ�������, �������, ������ɺ��޳� */
    //ע��                                 (׮����, ƽ̨����, ���յ������ʱ, ������������, ���շ���)
    pProto->pCMD[ECH_CMDID_REGISTER]     = EchCMDCreate(1,  2,  0,  makeCmdReg,        analyCmdCommon);
    pProto->pCMD[ECH_CMDID_HEARTBEAT]    = EchCMDCreate(3,  4,  0,  makeCmdHeart,      analyCmdHeart);
    pProto->pCMD[ECH_CMDID_RESET]        = EchCMDCreate(6,  5,  30, makeCmdReset,      analyCmdCommon);
    pProto->pCMD[ECH_CMDID_STATUS]       = EchCMDCreate(41, 42, 30, makeCmdStatus,     analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REMOTE_CTRL]  = EchCMDCreate(44, 43, 30, makeCmdRemoteCtrl, analyCmdCommon);
    pProto->pCMD[ECH_CMDID_RTDATA]       = EchCMDCreate(45, 0,  0,  makeCmdRTData,     NULL);
    pProto->pCMD[ECH_CMDID_ORDER]        = EchCMDCreate(46, 47, 30, makeCmdOrder,      analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_SUCC]     = EchCMDCreate(7,  0,  0,  makeCmdSetSucc,    NULL);
    pProto->pCMD[ECH_CMDID_SET_FAIL]     = EchCMDCreate(8,  0,  0,  makeCmdSetFail,    NULL);
    pProto->pCMD[ECH_CMDID_SET_POWERFEE] = EchCMDCreate(0,  11, 30, NULL,              analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_SERVFEE]  = EchCMDCreate(0,  12, 30, NULL,              analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_CYC]      = EchCMDCreate(0,  13, 30, NULL,              analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_TIMESEG]  = EchCMDCreate(0,  14, 30, NULL,              analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_KEY]      = EchCMDCreate(0,  15, 30, NULL,              analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_POWERFEE] = EchCMDCreate(22, 21, 30, makeCmdReqPowerFee, analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_SERVFEE]  = EchCMDCreate(24, 23, 30, makeCmdReqServFee, analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_CYC]      = EchCMDCreate(26, 25, 30, makeCmdReqCyc,     analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_TIMESEG]  = EchCMDCreate(28, 27, 30, makeCmdReqTimeSeg, analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_KEY]      = EchCMDCreate(30, 29, 30, makeCmdReqKey,     analyCmdCommon);
    pProto->pCMD[ECH_CMDID_REQ_SOFTVER]  = EchCMDCreate(34, 33, 30, makeCmdReqSoftVer, analyCmdCommon);
    pProto->pCMD[ECH_CMDID_SET_QR]       = EchCMDCreate(0,  35, 30, NULL,              analyCmdCommon);
    //end of ע��

    pProto->recvResponse = recvResponse;
    pProto->sendCommand = sendCommand;
    pProto->deleteProtocol = deleteProto;

    pProto->plechRecvCmd = gdsl_list_alloc ("ProtoRecvLis", echProtoListAlloc, echProtoListFree);
    pProto->plechSendCmd = gdsl_list_alloc ("ProtoSendLis", echProtoListAlloc, echProtoListFree);

    return pProto;
}



//encode/decode protocol requests
//encode/decode protocol responses

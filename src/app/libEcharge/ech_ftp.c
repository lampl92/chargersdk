#include "ech_ftp.h"
#include "cJSON.h"
#include "stringName.h"
#include "cfg_parse.h"
#include <string.h>

static ErrorCode_t GetFTPCfgItem(void *pvCfgObj, uint8_t *jnItemName, void *pvCfgItem, uint8_t type)
{
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pCfgObj;
    errcode = ERR_NO;

    pCfgObj = (cJSON *)pvCfgObj;

    //  解析
    jsItem = cJSON_GetObjectItem(pCfgObj, jnItemName);
    if (jsItem == NULL)
    {
        errcode = ERR_FILE_PARSE;
        goto err_return;
    }
    switch (type)
    {
    case ParamTypeU8:
        *((uint8_t *)pvCfgItem) = (uint8_t)(jsItem->valueint);
        break;
    case ParamTypeU16:
        *((uint16_t *)pvCfgItem) = (uint16_t)(jsItem->valueint);
        break;
    case ParamTypeU32:
        *((uint32_t *)pvCfgItem) = (uint32_t)(jsItem->valuedouble);
        break;
    case ParamTypeDouble:
        *((double *)pvCfgItem) = (double)(jsItem->valuedouble);
        break;
    case ParamTypeString:
        strcpy((uint8_t *)pvCfgItem, jsItem->valuestring);
        break;
    default:
        break;
    }

#ifdef DEBUG_CFG_PARSE_PROTO
    switch (type)
    {
    case ParamTypeU8:
        printf_safe("%s\t = %d\n", jnItemName, *((uint8_t *)pvCfgItem));
        break;
    case ParamTypeU16:
        printf_safe("%s\t = %d\n", jnItemName, *((uint16_t *)pvCfgItem));
        break;
    case ParamTypeU32:
        printf_safe("%s\t = %d\n", jnItemName, *((uint32_t *)pvCfgItem));
        break;
    case ParamTypeDouble:
        printf_safe("%s\t = %.2lf\n", jnItemName, *((double *)pvCfgItem));
        break;
    case ParamTypeString:
        printf_safe("%s\t = %s\n", jnItemName, (uint8_t *)pvCfgItem);
        break;
    default:
        break;
    }
#endif

    /*********************/

err_return:
    return errcode;
}

ErrorCode_t GetFTPCfg(void *pvFtp, void *pvCfgObj)
{
    cJSON *jsFTPObj;
    ErrorCode_t errcode;
    EchFtpCfg_t *pFtp;

    errcode = ERR_NO;
    pFtp = (EchFtpCfg_t *)pvFtp;

    /*json解析*/
    jsFTPObj = GetCfgObj(pathFTPCfg, &errcode);
    if (jsFTPObj == NULL || errcode != ERR_NO)
    {
        return errcode;
    }

    THROW_ERROR(defDevID_File,
                errcode = GetFTPCfgItem(jsFTPObj,
                                        jnFtpServer,                                        
                                        (void *)(pFtp->strServer),                                
                                        ParamTypeString),
                ERR_LEVEL_WARNING,
                "GetFTPServer()");
    THROW_ERROR(defDevID_File,
                errcode = GetFTPCfgItem(jsFTPObj,
                                        jnFtpPort,                                        
                                        (void *)(&(pFtp->usPort)),                                
                                        ParamTypeU16),
                ERR_LEVEL_WARNING,
                "GetFTPPort()");
    THROW_ERROR(defDevID_File,
                errcode = GetFTPCfgItem(jsFTPObj,
                                        jnFtpUsername,                                        
                                        (void *)(pFtp->strUser),                                
                                        ParamTypeString),
                ERR_LEVEL_WARNING,
                "GetFTPUser()");
    THROW_ERROR(defDevID_File,
                errcode = GetFTPCfgItem(jsFTPObj,
                                        jnFtpPassword,                                        
                                        (void *)(pFtp->strPassword),                                
                                        ParamTypeString),
                ERR_LEVEL_WARNING,
                "GetFTPPass()");
    THROW_ERROR(defDevID_File,
                errcode = GetFTPCfgItem(jsFTPObj,
                                        jnFtpNewVersion,                                        
                                        (void *)(pFtp->strNewVersion),                                
                                        ParamTypeString),
                ERR_LEVEL_WARNING,
                "GetFTPNewVersion()");
    THROW_ERROR(defDevID_File,
                errcode = GetFTPCfgItem(jsFTPObj,
                                        jnFtpNewFilename,                                        
                                        (void *)(pFtp->strNewFileName),                                
                                        ParamTypeString),
                ERR_LEVEL_WARNING,
                "GetFTPNewFileName()");
    THROW_ERROR(defDevID_File,
                errcode = GetFTPCfgItem(jsFTPObj,
                                        jnFtpDownloadStart,                                        
                                        (void *)(&(pFtp->ucDownloadStart)),                                
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                "GetFTPDWStart()");
    THROW_ERROR(defDevID_File,
                errcode = GetFTPCfgItem(jsFTPObj,
                                        jnFtpDownloadStatus,                                        
                                        (void *)(&(pFtp->ucDownloadStatus)),                                
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                "GetFTPDWStatus()");

    cJSON_Delete(jsFTPObj);
    return errcode;
}



ErrorCode_t SetFTPCfg(uint8_t *jnItemString, void *pvCfgParam, uint8_t type)
{
    cJSON *jsFTPCfgObj;
    cJSON *jsItem;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    jsFTPCfgObj = GetCfgObj(pathFTPCfg, &errcode);
    if (jsFTPCfgObj == NULL)
    {
        return errcode;
    }
    jsItem = jsFTPCfgObj->child;
    do
    {
        if (strcmp(jsItem->string, jnItemString) == 0)
        {
            switch (type)
            {
            case ParamTypeU8:
                cJSON_ReplaceItemInObject(jsFTPCfgObj, jnItemString, cJSON_CreateNumber(*((uint8_t *)pvCfgParam)));
                break;
            case ParamTypeU16:
                cJSON_ReplaceItemInObject(jsFTPCfgObj, jnItemString, cJSON_CreateNumber(*((uint16_t *)pvCfgParam)));
                break;
            case ParamTypeU32:
                cJSON_ReplaceItemInObject(jsFTPCfgObj, jnItemString, cJSON_CreateNumber(*((uint32_t *)pvCfgParam)));
                break;
            case ParamTypeDouble:
                cJSON_ReplaceItemInObject(jsFTPCfgObj, jnItemString, cJSON_CreateNumber(*((double *)pvCfgParam)));
                break;
            case ParamTypeString:
                cJSON_ReplaceItemInObject(jsFTPCfgObj, jnItemString, cJSON_CreateString((uint8_t *)pvCfgParam));
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
    } while (jsItem != NULL);
    errcode = SetCfgObj(pathFTPCfg, jsFTPCfgObj);

    return errcode;
}

void EchFtpInit(EchFtpCfg_t *ftp)
{
    memset(ftp, '\0', sizeof(EchFtpCfg_t));
    ftp->SetFtpCfg = SetFTPCfg;
    ftp->GetFtpCfg = GetFTPCfg;
}
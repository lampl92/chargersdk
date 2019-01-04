#include "ech_ftp.h"
#include "cJSON.h"
#include "stringName.h"
#include "cfg_parse.h"
#include <string.h>

static ErrorCode_t GetFTPCfgItem(void *pvCfgObj, char *jnItemName, void *pvCfgItem, uint8_t type)
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
        strcpy((char *)pvCfgItem, jsItem->valuestring);
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
    cJSON *jsCfgObj;
    ErrorCode_t errcode;
    EchFtpCfg_t *pFtp;

    errcode = ERR_NO;
    pFtp = (EchFtpCfg_t *)pvFtp;

    /*json解析*/
    if (pvCfgObj == NULL)
    {
        jsCfgObj = GetCfgObj(pathFTPCfg, &errcode);
        if (jsCfgObj == NULL)
        {
            return errcode;
        }
    }
    else
    {
        jsCfgObj = (cJSON *)pvCfgObj;
    }

    cfgobj_get_string(jsCfgObj, pFtp->strServer, "%s", jnFtpServer);
    cfgobj_get_uint16(jsCfgObj, &pFtp->usPort, "%s", jnFtpPort);
    cfgobj_get_string(jsCfgObj, pFtp->strUser, "%s", jnFtpUsername);
    cfgobj_get_string(jsCfgObj, pFtp->strPassword, "%s", jnFtpPassword);
    cfgobj_get_string(jsCfgObj, pFtp->strNewVersion, "%s", jnFtpNewVersion);
    cfgobj_get_string(jsCfgObj, pFtp->strNewFileName, "%s", jnFtpNewFilename);
    cfgobj_get_uint8(jsCfgObj, &pFtp->ucDownloadStart, "%s", jnFtpDownloadStart);
    cfgobj_get_uint8(jsCfgObj, &pFtp->ucDownloadStatus, "%s", jnFtpDownloadStatus);

    cJSON_Delete(jsCfgObj);
    return errcode;
}

void EchFtpInit(EchFtpCfg_t *ftp)
{
    memset(ftp, '\0', sizeof(EchFtpCfg_t));
    ftp->GetFtpCfg = GetFTPCfg;
}
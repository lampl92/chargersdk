#include "cfg_sys.h"
#include "cJSON.h"
#include "stringName.h"
#include "cfg_parse.h"
#include <string.h>

static ErrorCode_t GetSysCfgItem(void *pvCfgObj, char *jnItemName, void *pvCfgItem, uint8_t type)
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
    case ParamTypeS32:
        *((int32_t *)pvCfgItem) = (int32_t)(jsItem->valueint);
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

static ErrorCode_t GetSysCfg(void *pvSysconf, void *pvCfgObj)
{
    cJSON *jsSysObj;
    ErrorCode_t errcode;
    Sysconf_t *pSysconf;

    errcode = ERR_NO;
    pSysconf = (Sysconf_t *)pvSysconf;

    /*json解析*/
    jsSysObj = GetCfgObj(pathSysCfg, &errcode);
    if (jsSysObj == NULL || errcode != ERR_NO)
    {
        return errcode;
    }

    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysChargersdk_bin,                  
                                        (void *)&(pSysconf->xUpFlag.chargesdk_bin),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSysChargersdk_bin);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysChargersdk_bin_crc32,                  
                                        (void *)&(pSysconf->xUpFlag.chargesdk_bin_crc32),
                                        ParamTypeU32),
                ERR_LEVEL_WARNING,
                jnSysChargersdk_bin_crc32);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysEvse_cfg,                  
                                        (void *)&(pSysconf->xUpFlag.evse_cfg),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSysEvse_cfg);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysProtocol_cfg,                  
                                        (void *)&(pSysconf->xUpFlag.protocol_cfg),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSysProtocol_cfg);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysSys_cfg,                  
                                        (void *)&(pSysconf->xUpFlag.sys_cfg),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSysSys_cfg);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysBlacklist_cfg,                  
                                        (void *)&(pSysconf->xUpFlag.blacklist_cfg),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSysBlacklist_cfg);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysWhitelist_cfg,                  
                                        (void *)&(pSysconf->xUpFlag.whitelist_cfg),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSysWhitelist_cfg);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysDispSleepTime,                  
                                        (void *)(&(pSysconf->ulDispSleepTime_s)),
                                        ParamTypeU32),
                ERR_LEVEL_WARNING,
                jnSysDispSleepTime);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSystouchtype,                  
                                        (void *)(&(pSysconf->xCalibrate.touchtype)),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSystouchtype);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysis_calibrate,                  
                                        (void *)(&(pSysconf->xCalibrate.is_calibrate)),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSysis_calibrate);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysxfac,                  
                                        (void *)(&(pSysconf->xCalibrate.xfac)),
                                        ParamTypeDouble),
                ERR_LEVEL_WARNING,
                jnSysxfac);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysyfac,                  
                                        (void *)(&(pSysconf->xCalibrate.yfac)),
                                        ParamTypeDouble),
                ERR_LEVEL_WARNING,
                jnSysyfac);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysxoff,                  
                                        (void *)(&(pSysconf->xCalibrate.xoff)),
                                        ParamTypeU16),
                ERR_LEVEL_WARNING,
                jnSysxoff);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysyoff,                  
                                        (void *)(&(pSysconf->xCalibrate.yoff)),
                                        ParamTypeU16),
                ERR_LEVEL_WARNING,
                jnSysyoff);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysVersion,                  
                                        (void *)(pSysconf->strVersion),
                                        ParamTypeString),
                ERR_LEVEL_WARNING,
                jnSysVersion);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysUSE_GPRSModem,                  
                                        (void *)(&(pSysconf->xModule.use_gprs)),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSysUSE_GPRSModem);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysUSE_Meter,                  
                                        (void *)(&(pSysconf->xModule.use_meter)),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSysUSE_Meter);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysUSE_RFID,                  
                                        (void *)(&(pSysconf->xModule.use_rfid)),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSysUSE_RFID);
    THROW_ERROR(defDevID_File,
                errcode = GetSysCfgItem(jsSysObj,
                                        jnSysUSE_GUI,                  
                                        (void *)(&(pSysconf->xModule.use_gui)),
                                        ParamTypeU8),
                ERR_LEVEL_WARNING,
                jnSysUSE_GUI);
    
    cJSON_Delete(jsSysObj);
    return errcode;
}

static ErrorCode_t SetSysCfg(char *jnItemString, void *pvCfgParam, uint8_t type)
{
    cJSON *jsSysCfgObj;
    cJSON *jsItem;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    jsSysCfgObj = GetCfgObj(pathSysCfg, &errcode);
    if (jsSysCfgObj == NULL)
    {
        return errcode;
    }
    jsItem = jsSysCfgObj->child;
    do
    {
        if (strcmp(jsItem->string, jnItemString) == 0)
        {
            switch (type)
            {
            case ParamTypeU8:
                cJSON_ReplaceItemInObject(jsSysCfgObj, jnItemString, cJSON_CreateNumber(*((uint8_t *)pvCfgParam)));
                break;
            case ParamTypeU16:
                cJSON_ReplaceItemInObject(jsSysCfgObj, jnItemString, cJSON_CreateNumber(*((uint16_t *)pvCfgParam)));
                break;
            case ParamTypeU32:
                cJSON_ReplaceItemInObject(jsSysCfgObj, jnItemString, cJSON_CreateNumber(*((uint32_t *)pvCfgParam)));
                break;            
            case ParamTypeS32:
                cJSON_ReplaceItemInObject(jsSysCfgObj, jnItemString, cJSON_CreateNumber(*((int32_t *)pvCfgParam)));
                break;
            case ParamTypeDouble:
                cJSON_ReplaceItemInObject(jsSysCfgObj, jnItemString, cJSON_CreateNumber(*((double *)pvCfgParam)));
                break;
            case ParamTypeString:
                cJSON_ReplaceItemInObject(jsSysCfgObj, jnItemString, cJSON_CreateString((uint8_t *)pvCfgParam));
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
    errcode = SetCfgObj(pathSysCfg, jsSysCfgObj);

    return errcode;
}

void SysCfgInit(Sysconf_t *sysconf)
{
    memset(sysconf, '\0', sizeof(Sysconf_t));
    sysconf->SetSysCfg = SetSysCfg;
    sysconf->GetSysCfg = GetSysCfg;
}
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
    cJSON *jsCfgObj;
    ErrorCode_t errcode;
    Sysconf_t *pSysconf;

    errcode = ERR_NO;
    pSysconf = (Sysconf_t *)pvSysconf;

    /*json解析*/
    if (pvCfgObj == NULL)
    {
        jsCfgObj = GetCfgObj(pathSysCfg, &errcode);
        if (jsCfgObj == NULL)
        {
            return errcode;
        }
    }
    else
    {
        jsCfgObj = (cJSON *)pvCfgObj;
    }
    
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xUpFlag.chargesdk_bin, "%s", jnSysChargersdk_bin);
    cfgobj_get_uint32(jsCfgObj, &pSysconf->xUpFlag.chargesdk_bin_crc32, "%s", jnSysChargersdk_bin_crc32);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xUpFlag.evse_cfg, "%s", jnSysEvse_cfg);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xUpFlag.protocol_cfg, "%s", jnSysProtocol_cfg);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xUpFlag.sys_cfg, "%s", jnSysSys_cfg);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xUpFlag.blacklist_cfg, "%s", jnSysBlacklist_cfg);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xUpFlag.whitelist_cfg, "%s", jnSysWhitelist_cfg);
    cfgobj_get_uint32(jsCfgObj, &pSysconf->ulDispSleepTime_s, "%s", jnSysDispSleepTime);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xCalibrate.touchtype, "%s", jnSystouchtype);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xCalibrate.touchtype, "%s", jnSystouchtype);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xCalibrate.is_calibrate, "%s", jnSysis_calibrate);
    cfgobj_get_double(jsCfgObj, &pSysconf->xCalibrate.xfac, "%s", jnSysxfac);
    cfgobj_get_double(jsCfgObj, &pSysconf->xCalibrate.yfac, "%s", jnSysyfac);
    cfgobj_get_uint16(jsCfgObj, &pSysconf->xCalibrate.xoff, "%s", jnSysxoff);
    cfgobj_get_uint16(jsCfgObj, &pSysconf->xCalibrate.yoff, "%s", jnSysyoff);
    extern uint8_t passwd[];
    cfgobj_get_string(jsCfgObj, passwd, "%s", jnSysVersion);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xModule.use_gprs, "%s", jnSysUSE_GPRSModem);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xModule.use_meter, "%s", jnSysUSE_Meter);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xModule.use_rfid, "%s", jnSysUSE_RFID);
    cfgobj_get_uint8(jsCfgObj, &pSysconf->xModule.use_gui, "%s", jnSysUSE_GUI);
    
    cJSON_Delete(jsCfgObj);
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
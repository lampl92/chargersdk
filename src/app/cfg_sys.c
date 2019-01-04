#include "cfg_sys.h"
#include "cJSON.h"
#include "stringName.h"
#include "cfg_parse.h"
#include "evse_version.h"
#include "file_op.h"
#include <string.h>

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
    sprintf(pSysconf->strVersion, "%s.%s.%s", EVSE_MAJOR, EVSE_MINOR, EVSE_PATCH);
    get_crc32_tmp_file(pathBootldrCrc32Tmp, pSysconf->strBootldrCrc32);
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
    errcode = SetCfgObj(pathSysCfg, jsSysCfgObj, 0);

    return errcode;
}

void SysCfgInit(Sysconf_t *sysconf)
{
    memset(sysconf, '\0', sizeof(Sysconf_t));
    sysconf->SetSysCfg = SetSysCfg;
    sysconf->GetSysCfg = GetSysCfg;
}
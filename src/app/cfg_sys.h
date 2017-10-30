#ifndef SYS_CFG_H
#define SYS_CFG_H

#include "stm32f4xx.h"
#include "errorcode.h"

typedef struct
{
    uint8_t touchtype;
    uint8_t is_calibrate;
    double xfac;
    double yfac;
    uint16_t xoff;
    uint16_t yoff;
} Calibrate_t;


//0   无需升级
//1   待升级
//2   升级成功
//3   升级失败
typedef struct 
{
    uint8_t chargesdk_bin;
    uint8_t evse_cfg;
    uint8_t protocol_cfg;
    uint8_t sys_cfg;
    uint8_t blacklist_cfg;
    uint8_t whitelist_cfg;
}UpgradeFlag_t;
typedef struct 
{
    uint8_t use_gprs; // 0:无网络 2:2G 3:3G
    uint8_t use_meter; //1:内部计量  2:外部计量
    uint8_t use_rfid; // 1:使用rfid
    uint8_t use_gui; //1:使用GUI
}ModuleSelect_t;

typedef struct
{
    char strVersion[64 + 1];
    uint32_t ulDispSleepTime_s;
    Calibrate_t xCalibrate;
    UpgradeFlag_t xUpFlag;
    ModuleSelect_t xModule;
    ErrorCode_t(*GetSysCfg)(void *pvSys, void *pvCfgObj);
    ErrorCode_t(*SetSysCfg)(uint8_t *jnItemString, void *pvCfgParam, uint8_t type);
    
} Sysconf_t;

void SysCfgInit(Sysconf_t *sysconf);

#endif

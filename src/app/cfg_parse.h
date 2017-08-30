/**
* @file D:\Documents\Projects\chargersdk\src\app\cfg_parse.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-19
*/
#ifndef  __CFG_PARSE_H
#define  __CFG_PARSE_H

#include "cJSON.h"

#define ParamTypeU8         1
#define ParamTypeU16        2
#define ParamTypeU32        3
#define ParamTypeDouble     4
#define ParamTypeString     5
#define ParamTypeList       6
#define ParamTypeObj        7

ErrorCode_t SetCfgObj(uint8_t *path, cJSON *jsCfgObj);
cJSON *GetCfgObj(uint8_t *path, ErrorCode_t *perrcode);

#endif

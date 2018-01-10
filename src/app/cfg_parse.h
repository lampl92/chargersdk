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
#include "errorcode.h"

#define ParamTypeU8         1
#define ParamTypeU16        2
#define ParamTypeU32        3
#define ParamTypeDouble     4
#define ParamTypeString     5
#define ParamTypeList       6
#define ParamTypeObj        7
#define ParamTypeS32        8

ErrorCode_t cfg_get_uint8(cJSON *pCfgObj, uint8_t *retval, char *fmt, ...);
ErrorCode_t cfg_get_uint16(cJSON *pCfgObj, uint16_t *retval, char *fmt, ...);
ErrorCode_t cfg_get_uint32(cJSON *pCfgObj, uint32_t *retval, char *fmt, ...);
ErrorCode_t cfg_get_int32(cJSON *pCfgObj, int32_t *retval, char *fmt, ...);
ErrorCode_t cfg_get_double(cJSON *pCfgObj, double *retval, char *fmt, ...);
ErrorCode_t cfg_get_string(cJSON *pCfgObj, char *retval, char *fmt, ...);

ErrorCode_t SetCfgObj(char *path, cJSON *jsCfgObj);
cJSON *GetCfgObj(char *path, ErrorCode_t *perrcode);

#endif

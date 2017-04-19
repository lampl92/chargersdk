/**
* @file D:\Documents\Projects\chargersdk\src\app\cfg_parse.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-19
*/
#ifndef  __CFG_PARSE_H
#define  __CFG_PARSE_H

ErrorCode_t SetCfgObj(uint8_t *path, cJSON *jsCfgObj);
cJSON *GetCfgObj(uint8_t *path, ErrorCode_t *perrcode);

#endif

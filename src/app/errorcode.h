/**
* @file errorcode.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#ifndef  __ERRORCODE_H
#define  __ERRORCODE_H

#define THROW_ERROR(errcode)   if(errcode != ERR_NO) {ThrowErrorCode(errcode);}

typedef enum _ErrorCode
{
    ERR_NO,
    ERR_SCRAM,
    ERR_OTHER
} ErrorCode_t;

extern const uint8_t *strErrorCode[];

void ThrowErrorCode(ErrorCode_t errcode);
#endif

#ifndef ECH_FTP_H
#define ECH_FTP_H

#include "stm32f4xx.h"
#include "errorcode.h"

typedef struct
{
    char strServer[64 + 1];
    char strUser[64 + 1];
    char strPassword[64 + 1];
    char strNewFileName[64 + 1];
    char strNewVersion[64 + 1];
    uint16_t usPort;
    uint8_t ucDownloadStart;
    uint8_t ucDownloadStatus;
    ErrorCode_t(*GetFtpCfg)(void *pvFtp, void *pvCfgObj);
    ErrorCode_t(*SetFtpCfg)(uint8_t *jnItemString, void *pvCfgParam, uint8_t type);
    
}EchFtpCfg_t;

void EchFtpInit(EchFtpCfg_t *ftp);

#endif
#ifndef _INTERFACE_FTPCLIENT_H_
#define _INTERFACE_FTPCLIENT_H_

#include  "stm32f4xx.h"

struct _ftp_ctx;

typedef int(*ftp_cb)(struct _ftp_ctx *ftp_ctx, char *data, uint32_t len);

struct _ftp_ctx
{
    char ftp_server[64];
    uint16_t port;
    char fpath[128];
    char fname[128];
    char user[64];
    char pass[64];
    ftp_cb on_bg;
    ftp_cb on_data;
    ftp_cb on_end;
};

int ftp_download_file(EchFtpCfg_t *pechFtp, net_device_t *net_dev);

#endif
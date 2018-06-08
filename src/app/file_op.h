#ifndef FILE_OP_H
#define FILE_OP_H

#include "libEcharge/ech_ftp.h"

typedef struct _flist_t
{
    char strFilename[64];
    uint32_t size_byte;
    char strCrc32[9];
    char strLocalpath[256];
    char strFtpdir[256];
    uint8_t ucStatus;
}flist_t;

uint8_t set_tmp_file(char *path, char *flg);
uint8_t get_tmp_file(char *path, char *flg);
ErrorCode_t parse_flist(char *path, EchFtpCfg_t *ftp, flist_t *flist);
int get_bmp_check_tmp(void);

#endif

#include "stm32f4xx.h"
#include "yaffsfs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libEcharge/ech_ftp.h"
#include "cJSON.h"
#include "cfg_parse.h"
#include "stringName.h"
#include "errorcode.h"
#include "file_op.h"

ErrorCode_t parse_flist(char *path, EchFtpCfg_t *ftp, flist_t *flist)
{
    cJSON *jsRoot = NULL;
    ErrorCode_t errcode = ERR_NO;
    int i = 0;
    uint8_t status = 0;
    
    jsRoot = GetCfgObj(pathDownloadList, &errcode);
    if (jsRoot == NULL)
    {
        return errcode;
    }
    cfgobj_get_string(jsRoot, ftp->strServer, "ftp.server");
    cfgobj_get_uint16(jsRoot, &ftp->usPort, "ftp.port");
    cfgobj_get_string(jsRoot, ftp->strUser, "ftp.user");
    cfgobj_get_string(jsRoot, ftp->strPassword, "ftp.pass");
    
   // while(1)
    for(i = 0;;i++)
    {
        //i++;
        errcode = cfgobj_get_string(jsRoot, flist->strFilename, "flist:%d.filename", i);
        if (errcode != ERR_NO)
        {
            cJSON_Delete(jsRoot);
            return errcode;
        }
        cfgobj_get_uint32(jsRoot, &flist->size_byte, "flist:%d.size_byte", i);
        ftp->fsize = flist->size_byte;
        cfgobj_get_string(jsRoot, flist->strCrc32, "flist:%d.crc32", i);
        cfgobj_get_string(jsRoot, flist->strLocalpath, "flist:%d.localpath", i);
        cfgobj_get_string(jsRoot, flist->strFtpdir, "flist:%d.ftpdir", i);
        cfgobj_get_uint8(jsRoot, &flist->ucStatus, "flist:%d.status", i);
        if (flist->ucStatus == 1)
        {
            strcpy(ftp->strNewVersion, flist->strFtpdir);
            strcpy(ftp->strNewFileName, flist->strFilename);
            status = 0;
            cfgobj_set_uint8(jsRoot, &status, "flist:%d.status", i);
            break;
        }
        else
        {
            continue;
        }
    }
    errcode = SetCfgObj(pathDownloadList, jsRoot);
    return errcode;
}

uint8_t set_tmp_file(char *path, char *flg)
{
    int fd;
    int bw;
    fd = yaffs_open(path, O_CREAT | O_TRUNC | O_RDWR, S_IWRITE | S_IREAD);
    if (fd < 0)
    {
        ThrowFSCode(yaffs_get_error(), path, "set_upgrade_tmp()-open");
        return 0;
    }
    taskENTER_CRITICAL();
    bw = yaffs_write(fd, flg, 1);
    taskEXIT_CRITICAL();
    if (1 != bw)
    {
        ThrowFSCode(yaffs_get_error(), path, "set_upgrade_tmp()-write");
        yaffs_close(fd);
        return 0;
    }
    yaffs_close(fd);
    return 1;
}

uint8_t get_tmp_file(char *path, char *flg)
{
    int fd;
    int rb;
    fd = yaffs_open(path, O_RDWR, S_IWRITE | S_IREAD);
    if (fd < 0)
    {
        //ThrowFSCode(yaffs_get_error(), path, "get_upgrade_tmp()-open");
        return 0;
    }
    taskENTER_CRITICAL();
    rb = yaffs_read(fd, flg, 1);
    taskEXIT_CRITICAL();
    if (1 != rb)
    {
        //ThrowFSCode(yaffs_get_error(), path, "set_upgrade_tmp()-read");
        yaffs_close(fd);
        return 0;
    }
    yaffs_close(fd);
    return 1;
}


int get_bmp_check_tmp(void)
{
    char ch[1];
    char flg;
    if (get_tmp_file(pathBmpCheckTmp, ch) == 1)
    {
        flg = atoi(ch);
        if (flg == 3)//有文件并且设置过3
        {
            return flg;
        }
        else
        {
            return flg;
        }
    }
    else//无文件
    {
        return -1;
    }
}
/**
* @file cfg_parse.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-18
*/
#include "includes.h"
#include "stringName.h"
#include "factorycfg.h"
#include "interface.h"
#include "cJSON.h"
#include "yaffsfs.h"
#include "sysinit.h"

static const char *select_ctx_from_path(char *path)
{
    if (strcmp(path, pathOrder) == 0)
    {
        return strOrderCfg;
    }
    if (strcmp(path, pathOrderTmp) == 0)
    {
        return strOrderCfg;
    }
    if (strcmp(path, pathEVSELog) == 0)
    {
        return strLogCfg;
    }
    if (strcmp(path, pathEVSECfg) == 0)
    {
        return strEVSECfg;
    }
    if (strcmp(path, pathSysCfg) == 0)
    {
        return strSysCfg;
    }
    if (strcmp(path, pathFTPCfg) == 0)
    {
        return strFtpCfg;
    }
    if (strcmp(path, pathProtoCfg) == 0)
    {
        return strProtoCfg;
    }
    if (strcmp(path, pathWhiteList) == 0)
    {
        return strWhiteListCfg;
    }
    if (strcmp(path, pathBlackList) == 0)
    {
        return strBlackListCfg;
    }
    return NULL;
}


/** @brief 保存jsCfgObj到配置文件,设置完毕后删除cJSON指针
 *
 * @param path uint8_t*
 * @param jsCfgObj cJSON*
 * @return ErrorCode_t
 *
 */
ErrorCode_t SetCfgObj(char *path, cJSON *jsCfgObj)
{
    int fd;
    uint8_t *pbuff;
    uint32_t len;
    uint32_t bw;
    ErrorCode_t errcode;

    pbuff = NULL;
    errcode = ERR_NO;
    
    pbuff = cJSON_Print(jsCfgObj);
    len = strlen(pbuff);
    if(pbuff == NULL)
    {
        errcode = ERR_SET_SERIALIZATION;
        goto exit;
    }
    fd = yaffs_open(path, O_CREAT | O_TRUNC | O_RDWR, S_IWRITE | S_IREAD);
    if (fd < 0)
    {
        ThrowFSCode(yaffs_get_error(), path, "SetCfgObj()-open");
        errcode = ERR_FILE_NO;
        goto exit;
    }
    taskENTER_CRITICAL();
    bw = yaffs_write(fd, pbuff, len);
    taskEXIT_CRITICAL();
    if(len != bw)
    {
        ThrowFSCode(yaffs_get_error(), path, "SetCfgObj()-write");
        errcode = ERR_FILE_RW;
        goto exit_write;
    }
exit_write:
    yaffs_close(fd);
    free(pbuff);
exit:
    cJSON_Delete(jsCfgObj);
    return errcode;
}

/** @brief 获取配置文件cJSON结构体,注意使用完成后需要调用cJSON_Delete对cJSON指针进行释放
 *
 * @param path uint8_t* 配置文件存放位置
 * @param perrcode ErrorCode_t* 返回错误代码
 * @return cJSON* 返回cJSON指针
 *
 */
cJSON *GetCfgObj(char *path, ErrorCode_t *perrcode)
{
    int fd;
    uint8_t *rbuff;
    uint32_t fsize;
    struct yaffs_stat st;
    uint32_t  br;   //byte read

    cJSON *jsCfgObj = NULL;
    int i;
    *perrcode = ERR_NO;
    /*读取文件*/
    fd = yaffs_open(path, O_RDWR, 0);
    if (fd < 0)
    {
        ThrowFSCode(yaffs_get_error(), path, "GetCfgObj-open");
        *perrcode = ERR_FILE_NO;
        goto exit;
    }
    yaffs_stat(path, &st);
    fsize = st.st_size;
    rbuff = (uint8_t *)malloc(fsize * sizeof(uint8_t));
    taskENTER_CRITICAL();
    br = yaffs_read(fd, rbuff, fsize);
    taskEXIT_CRITICAL();
    if(fsize != br)
    {
        ThrowFSCode(yaffs_get_error(), path, "GetCfgObj-read");
        *perrcode = ERR_FILE_RW;
        goto exit_read;
    }

    /*json解析*/
    jsCfgObj = cJSON_Parse(rbuff);
    if(jsCfgObj == NULL)
    {
        printf_safe("cfg file parse fail, remove orig file and create new cfg file!!\n");
        yaffs_unlink(path);
        create_cfg_file(path, select_ctx_from_path(path));
        *perrcode = ERR_FILE_PARSE;
        goto exit_parse;
    }
exit_read:
exit_parse:
    yaffs_close(fd);
    free(rbuff);
exit:
    return jsCfgObj;
}

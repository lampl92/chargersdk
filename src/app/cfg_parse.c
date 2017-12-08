/**
* @file cfg_parse.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-18
*/
#include "includes.h"
#include "stringName.h"
#include "interface.h"
#include "cJSON.h"
#include "yaffsfs.h"
#include "user_app.h"

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
    int res = 0;
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
        ThrowFSCode(res = yaffs_get_error(), path, "SetCfgObj()-open");
    }
    if(res != 0)
    {
        errcode = ERR_FILE_RW;
        goto exit;
    }
    taskENTER_CRITICAL();
    if (flag_power_out_l == 1)
    {
        POWER_L_ON;
    }
    if (flag_power_out_n == 1)
    {
        POWER_N_ON;
    }
    
    bw = yaffs_write(fd, pbuff, len);
    taskEXIT_CRITICAL();
    if (flag_power_out_l == 1)
    {
        POWER_L_ON;
    }
    if (flag_power_out_n == 1)
    {
        POWER_N_ON;
    }
    if(len != bw)
    {
        ThrowFSCode(res = yaffs_get_error(), path, "SetCfgObj()-write");
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
    int res = 0;
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
        ThrowFSCode(res = yaffs_get_error(), path, "GetCfgObj-open");
    }
    if(res != 0)
    {
        *perrcode = ERR_FILE_RW;
        goto exit;
    }
    //fsize = yaffs_lseek(fd, 0, SEEK_END);
    //yaffs_lseek(fd, 0, SEEK_SET);
    yaffs_stat(path, &st);
    fsize = st.st_size;
    rbuff = (uint8_t *)malloc(fsize * sizeof(uint8_t));
    taskENTER_CRITICAL();
    yaffsfs_SetError(0);
    br = yaffs_read(fd, rbuff, fsize);
    taskEXIT_CRITICAL();
    if(fsize != br)
    {
        ThrowFSCode(res = yaffs_get_error(), path, "GetCfgObj-read");
        *perrcode = ERR_FILE_RW;
        goto exit_read;
    }

    /*json解析*/
    jsCfgObj = cJSON_Parse(rbuff);
    if(jsCfgObj == NULL)
    {
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

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

/** @brief 保存jsCfgObj到配置文件,设置完毕后删除cJSON指针
 *
 * @param path uint8_t*
 * @param jsCfgObj cJSON*
 * @return ErrorCode_t
 *
 */
ErrorCode_t SetCfgObj(char *path, cJSON *jsCfgObj)
{
    FIL f;
    FRESULT res;
    uint8_t *pbuff;
    uint32_t len;
    UINT bw;
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
    taskENTER_CRITICAL(); 
    ThrowFSCode(res = f_open(&f, path, FA_CREATE_ALWAYS|FA_WRITE), path, "SetCfgObj()-open");
    f_sync(&f);
    taskEXIT_CRITICAL();
    if(res != FR_OK)
    {
        errcode = ERR_FILE_RW;
        goto exit;
    }
    taskENTER_CRITICAL(); 
    ThrowFSCode(res = f_write(&f, pbuff, len, &bw), path, "SetCfgObj()-write");
    f_sync(&f); 
    taskEXIT_CRITICAL();
    if(len != bw)
    {
        errcode = ERR_FILE_RW;
        goto exit_write;
    }
exit_write:
    f_close(&f);
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
    FIL f;
    FRESULT res;
    uint8_t *rbuff;
    FSIZE_t fsize;
    UINT  br;   //byte read

    cJSON *jsCfgObj = NULL;
    int i;
    *perrcode = ERR_NO;
    /*读取文件*/
    taskENTER_CRITICAL();
    ThrowFSCode(res = f_open(&f, path, FA_READ), path, "GetCfgObj-open");
    taskEXIT_CRITICAL();
    if(res != FR_OK)
    {
        *perrcode = ERR_FILE_RW;
        goto exit;
    }
    fsize = f_size(&f);
    rbuff = (uint8_t *)malloc(fsize * sizeof(uint8_t));
    taskENTER_CRITICAL();
    ThrowFSCode(res = f_read(&f, rbuff, fsize, &br), path, "GetCfgObj-read");
    taskEXIT_CRITICAL();
    if(fsize != br)
    {
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
    f_close(&f);
    free(rbuff);
exit:
    return jsCfgObj;
}

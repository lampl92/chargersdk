/**
* @file cfg_parse.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-18
*/
#include "includes.h"
#include "cJSON.h"

/** @brief ����jsCfgObj�������ļ�,������Ϻ�ɾ��cJSONָ��
 *
 * @param path uint8_t*
 * @param jsCfgObj cJSON*
 * @return ErrorCode_t
 *
 */
ErrorCode_t SetCfgObj(uint8_t *path, cJSON *jsCfgObj)
{
    FIL f;
    FRESULT res;
    uint8_t *pbuff;
    uint32_t len;
    UINT bw;
    ErrorCode_t errcode;

    pbuff = NULL;
    errcode = ERR_NO;

    ThrowFSCode(res = f_open(&f, path, FA_WRITE));
    if(res != FR_OK)
    {
        errcode = ERR_FILE_RW;
        goto exit;
    }
    pbuff = cJSON_Print(jsCfgObj);
    len = strlen(pbuff);
    if(pbuff == NULL)
    {
        errcode = ERR_SET_SERIALIZATION;
        goto exit;
    }
    ThrowFSCode(res = f_write(&f, pbuff, len, &bw));
    if(len != bw)
    {
        errcode = ERR_FILE_RW;
        goto exit_write;
    }
exit_write:
    free(pbuff);
    f_close(&f);
exit:
    cJSON_Delete(jsCfgObj);
    return errcode;
}

/** @brief ��ȡ�����ļ�cJSON�ṹ��,ע��ʹ����ɺ���Ҫ����cJSON_Delete��cJSONָ������ͷ�
 *
 * @param path uint8_t* �����ļ����λ��
 * @param perrcode ErrorCode_t* ���ش������
 * @return cJSON* ����cJSONָ��
 *
 */
cJSON *GetCfgObj(uint8_t *path, ErrorCode_t *perrcode)
{
    FIL f;
    FRESULT res;
    uint8_t *rbuff;
    FSIZE_t fsize;
    UINT  br;   //byte read

    cJSON *jsCfgObj = NULL;
    int i;
    *perrcode = ERR_NO;
    /*��ȡ�ļ�*/
    ThrowFSCode(res = f_open(&f, path, FA_READ));
    if(res != FR_OK)
    {
        *perrcode = ERR_FILE_RW;
        goto exit;
    }
    fsize = f_size(&f);
    rbuff = (uint8_t *)malloc(fsize * sizeof(uint8_t));
    ThrowFSCode(res = f_read(&f, rbuff, fsize, &br));
    if(fsize != br)
    {
        *perrcode = ERR_FILE_RW;
        goto exit_read;
    }

    /*json����*/
    jsCfgObj = cJSON_Parse(rbuff);
    if(jsCfgObj == NULL)
    {
        *perrcode = ERR_FILE_PARSE;
        goto exit_parse;
    }
exit_read:
exit_parse:
    free(rbuff);
    f_close(&f);
exit:
    return jsCfgObj;
}

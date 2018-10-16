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
#include "cJSON.h"
#include "yaffsfs.h"
#include "sysinit.h"
#include "utils.h"
#include "cfg_parse.h"
#include <string.h>
#include <stdarg.h>

#define DEBUG_PARSE     0

#define MAX_STRING_LEN     200

/*
{
    "a": 1,
    "b": 2,
    "c": {
        "ca": 1,
        "cb": 2,
        "cc": [0, 1, 2]
    },
    "d": [{
            "da": 1,
            "db": 1,
            "dc": 1
        },
        {
            "da": 1,
            "db": 1,
            "dc": 1
        }
    ]
}
*/
//"a"
//"c.ca"
//"c.cc:2"
//"d:1.da"

static cJSON *cfgobj_scan(cJSON *pCfgObj, char *str_key)
{
    char *p;
    char *str_n;
    int n;
    cJSON *pRootObj;
    cJSON *pSubObj;
    
    pRootObj = pCfgObj;
    int i = 0;
    while ((p = strsep(&str_key, ".")) != NULL)
    {
#if DEBUG_PARSE
        i++;
        printf_safe("string = %s\n", p);
        printf_safe("size = %d\n", strlen(p));
        printf_safe("i = %d\n", i);
#endif
        str_n = strchr(p, ':');
        if (str_n != NULL)
        {             
            // p....str_n[0]
            // ↓    ↓
            //"array:0" => "array" 
            str_n[0]  = '\0';       
            // ......str_n+1
            //       ↓
            //"array:0"
            str_n = str_n + 1;     
            n = atoi(str_n);
            pSubObj = cJSON_GetObjectItem(pRootObj, p);
            if (pSubObj == NULL)
            {
                return NULL;
            }
            else
            {
                pRootObj = cJSON_GetArrayItem(pSubObj, n);
            }
        }
        else
        {
            pSubObj = cJSON_GetObjectItem(pRootObj, p);
            if (pSubObj == NULL)
            {
                return NULL;
            }
            else
            {
                pRootObj = pSubObj;
            }
        }
    }
    return pRootObj;
}

static ErrorCode_t cfgobj_get(cJSON *pCfgObj, void *retval, char *str_key, uint8_t type)
{
    ErrorCode_t errcode = ERR_NO;
    cJSON *pRootObj;
    
    pRootObj = cfgobj_scan(pCfgObj, str_key);
    if (pRootObj == NULL)
    {
        errcode = ERR_FILE_PARSE;
        goto exit;
    }
    switch (type)
    {
    case ParamTypeU8:
        *(uint8_t *)retval = pRootObj->valueint;
        break;
    case ParamTypeU16:
        *(uint16_t *)retval = pRootObj->valueint;
        break;
    case ParamTypeU32:
        *(uint32_t *)retval = (uint32_t)pRootObj->valuedouble;
        break;
    case ParamTypeS32:
        *(int32_t *)retval = pRootObj->valueint;
        break;
    case ParamTypeDouble:
        *(double *)retval = pRootObj->valuedouble;
        break;
    case ParamTypeString:
        strcpy((char *)retval, pRootObj->valuestring);
        break;
    default:
        retval = NULL;
        errcode = ERR_FILE_PARAM;
        break;
    }
    
exit:
    return errcode;
}
static ErrorCode_t cfgobj_set(cJSON *pCfgObj, void *setval, char *str_key, uint8_t type)
{
    ErrorCode_t errcode = ERR_NO;
    cJSON *pRootObj;
    
    pRootObj = cfgobj_scan(pCfgObj, str_key);
    if (pRootObj == NULL)
    {
        errcode = ERR_FILE_PARSE;
        goto exit;
    }
    switch (type)
    {
    case ParamTypeU8:
        cJSON_SetNumberValue(pRootObj, *(uint8_t *)setval);
        break;
    case ParamTypeU16:
        cJSON_SetNumberValue(pRootObj, *(uint16_t *)setval);
        break;
    case ParamTypeU32:
        cJSON_SetNumberValue(pRootObj, *(uint32_t *)setval);
        break;
    case ParamTypeS32:
        cJSON_SetNumberValue(pRootObj, *(int32_t *)setval);
        break;
    case ParamTypeDouble:
        cJSON_SetNumberValue(pRootObj, *(double *)setval);
        break;
    case ParamTypeString:
        free(pRootObj->valuestring);
        pRootObj->valuestring = utils_strdup((char *)setval);
        break;
    default:
        errcode = ERR_FILE_PARAM;
        break;
    }
    
exit:
    return errcode;
}
ErrorCode_t cfgobj_get_uint8(cJSON *pCfgObj, uint8_t *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_get(pCfgObj, (void *)retval, str, ParamTypeU8);
}
ErrorCode_t cfgobj_get_uint16(cJSON *pCfgObj, uint16_t *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_get(pCfgObj, (void *)retval, str, ParamTypeU16);
}
ErrorCode_t cfgobj_get_uint32(cJSON *pCfgObj, uint32_t *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_get(pCfgObj, (void *)retval, str, ParamTypeU32);
}

ErrorCode_t cfgobj_get_int32(cJSON *pCfgObj, int32_t *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_get(pCfgObj, (void *)retval, str, ParamTypeS32);
}

ErrorCode_t cfgobj_get_double(cJSON *pCfgObj, double *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_get(pCfgObj, (void *)retval, str, ParamTypeDouble);
}
ErrorCode_t cfgobj_get_string(cJSON *pCfgObj, char *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_get(pCfgObj, (void *)retval, str, ParamTypeString);
}
ErrorCode_t cfgobj_set_uint8(cJSON *pCfgObj, uint8_t *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_set(pCfgObj, (void *)setval, str, ParamTypeU8);
}
ErrorCode_t cfgobj_set_uint16(cJSON *pCfgObj, uint16_t *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_set(pCfgObj, (void *)setval, str, ParamTypeU16);
}
ErrorCode_t cfgobj_set_uint32(cJSON *pCfgObj, uint32_t *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_set(pCfgObj, (void *)setval, str, ParamTypeU32);
}

ErrorCode_t cfgobj_set_int32(cJSON *pCfgObj, int32_t *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_set(pCfgObj, (void *)setval, str, ParamTypeS32);
}

ErrorCode_t cfgobj_set_double(cJSON *pCfgObj, double *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_set(pCfgObj, (void *)setval, str, ParamTypeDouble);
}
ErrorCode_t cfgobj_set_string(cJSON *pCfgObj, char *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);

    return cfgobj_set(pCfgObj, (void *)setval, str, ParamTypeString);
}
/** @brief 保存jsCfgObj到配置文件,设置完毕后删除cJSON指针
 *
 * @param path uint8_t*
 * @param jsCfgObj cJSON*
 * @return ErrorCode_t
 *
 */
ErrorCode_t SetCfgObj(char *path, cJSON *jsCfgObj, int DoNotDeleteObj)
{
    int fd;
    char *pbuff;
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
    bw = yaffs_write(fd, pbuff, len);
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
    if (DoNotDeleteObj == 0x5555)
    {
    }
    else
    {
        cJSON_Delete(jsCfgObj);
    }
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
    char *rbuff;
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
    if (fsize == 0)//预防fsize为0导致程序错误
    {
        yaffs_close(fd);
        yaffs_unlink(path);
        NVIC_SystemReset();
        //Goodbye!!!
    }
    rbuff = (char *)malloc(fsize * sizeof(char));
    if (rbuff == NULL)
    {
        *perrcode = ERR_MEMORY;
        goto exit;
    }
    br = yaffs_read(fd, rbuff, fsize);
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

ErrorCode_t cfg_get_uint8(char *path, uint8_t *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_get(pObj, retval, str, ParamTypeU8);
    cJSON_Delete(pObj);
    return errcode;
}
ErrorCode_t cfg_get_uint16(char *path, uint16_t *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_get(pObj, retval, str, ParamTypeU16);
    cJSON_Delete(pObj);
    return errcode;   
}
ErrorCode_t cfg_get_uint32(char *path, uint32_t *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_get(pObj, retval, str, ParamTypeU32);
    cJSON_Delete(pObj);
    return errcode;
}
ErrorCode_t cfg_get_int32(char *path, int32_t *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_get(pObj, retval, str, ParamTypeS32);
    cJSON_Delete(pObj);
    return errcode;
}
ErrorCode_t cfg_get_double(char *path, double *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_get(pObj, retval, str, ParamTypeDouble);
    cJSON_Delete(pObj);
    return errcode;     
}
ErrorCode_t cfg_get_string(char *path, char *retval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_get(pObj, retval, str, ParamTypeString);
    cJSON_Delete(pObj);
    return errcode;
}

ErrorCode_t cfg_set_uint8(char *path, uint8_t *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_set(pObj, setval, str, ParamTypeU8);
    SetCfgObj(path, pObj, 0);
    return errcode;
}
ErrorCode_t cfg_set_uint16(char *path, uint16_t *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_set(pObj, setval, str, ParamTypeU16);
    SetCfgObj(path, pObj, 0);
    return errcode;
}
ErrorCode_t cfg_set_uint32(char *path, uint32_t *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_set(pObj, setval, str, ParamTypeU32);
    SetCfgObj(path, pObj, 0);
    return errcode;     
}
ErrorCode_t cfg_set_int32(char *path, int32_t *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_set(pObj, setval, str, ParamTypeS32);
    SetCfgObj(path, pObj, 0);
    return errcode;   
}
ErrorCode_t cfg_set_double(char *path, double *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_set(pObj, setval, str, ParamTypeDouble);
    SetCfgObj(path, pObj, 0);
    return errcode; 
}
ErrorCode_t cfg_set_string(char *path, char *setval, char *fmt, ...)
{
    char str[MAX_STRING_LEN] = { 0 };
    va_list va;
    uint32_t n;

    va_start(va, fmt);
    n  = vsnprintf(str, sizeof(str) - 1, fmt, va);
    va_end(va);
    
    cJSON *pObj;
    ErrorCode_t errcode;
    pObj = GetCfgObj(path, &errcode);
    if (pObj == NULL)
    {
        return errcode;
    }
    errcode = cfgobj_set(pObj, setval, str, ParamTypeString);
    SetCfgObj(path, pObj, 0);
    return errcode;      
}
#if 0
int test_cfgobj_set(int val)
{
    cJSON *pObj;
    ErrorCode_t errcode;
    uint8_t ucVal;
    uint16_t usVal;
    uint32_t ulVal;
    double dVal;
    char strVal[64] = "654456654";
    
    pObj = GetCfgObj(pathEVSECfg, &errcode);
    if (pObj == NULL)
    {
        return -1;
    }
    dVal = val;
    errcode = cfgobj_set_double(pObj, &dVal, "%s", "Lng");
    ucVal = 10;
    cfgobj_set_uint8(pObj, &ucVal, "%s", jnEVSEType);
    
    cfgobj_set_string(pObj, strVal, "%s", jnEVSESN);
    sprintf(strVal, "QR_%d",val);
    cfgobj_set_string(pObj, strVal, "%s:%d.%s", jnCONArray, 0, jnQRCode);
    SetCfgObj(pathEVSECfg, pObj, 0);
}
int test_cfgobj_get(void)
{
    cJSON *pObj;
    ErrorCode_t errcode;
    uint8_t ucVal;
    uint16_t usVal;
    uint32_t ulVal;
    double dVal;
    char strVal[64];
    
    pObj = GetCfgObj(pathEVSECfg, &errcode);
    if (pObj == NULL)
    {
        return -1;
    }
    errcode = cfgobj_get_double(pObj, &dVal, "%s", "Lng");
    if (errcode != ERR_NO)
    {
        printf_safe("double value = %s\n", "解析失败");
    }
    else
    {
        printf_safe("double val = %lf\n", dVal);
    }
#if 1
    errcode = cfgobj_get_double(pObj, &dVal, "%s", "La1t");
    if (errcode != ERR_NO)
    {
        printf_safe("double value = %s\n", "解析失败");
    }
    else
    {
        printf_safe("double val = %lf\n", dVal);
    }
    cfgobj_get_uint8(pObj, &ucVal, "%s", jnEVSEType);
    printf_safe("%s = %d\n", jnEVSEType, ucVal);
    
    cfgobj_get_uint16(pObj, &usVal, "%s", jnTotalCON);
    printf_safe("%s = %d\n", jnTotalCON, usVal);
    
    cfgobj_get_uint32(pObj, &ulVal, "%s", jnServiceFeeType);
    printf_safe("%s = %d\n", jnServiceFeeType, ulVal);
    
    cfgobj_get_string(pObj, strVal, "%s", jnEVSEID);
    printf_safe("%s = %s\n", jnEVSEID, strVal);
    
    cfgobj_get_string(pObj, strVal, "%s", jnEVSESN);
    printf_safe("%s = %s\n", jnEVSESN, strVal);

    cfgobj_get_uint32(pObj, &ulVal, "%s:%d.%s", jnCONArray, 0, jnID);
    printf_safe("CON%d %s = %d\n",0, jnID, ulVal);
    
    cfgobj_get_string(pObj, strVal, "%s:%d.%s", jnCONArray, 0, jnQRCode);
    printf_safe("CON%d %s = %s\n", 0, jnQRCode, strVal);
#endif
    cJSON_Delete(pObj);
    return 0;
}

void test_cfg_get(void)
{
    uint8_t ucVal = 111;
    uint16_t usVal;
    uint32_t ulVal;
    double dVal;
    char strVal[64];
    cfg_get_uint8(pathEVSECfg, &ucVal, "%s", jnEVSEType);
    printf_safe("%s = %d\n", jnEVSEType, ucVal);
}
#endif

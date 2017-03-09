/**
* @file cli_json.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-08
*/
#include "includes.h"
#include "cJSON.h"

char * makeJson()
{
    cJSON * pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if(NULL == pJsonRoot)
    {
        //error happend here
        return NULL;
    }
    cJSON_AddStringToObject(pJsonRoot, "hello", "hello world");
    cJSON_AddNumberToObject(pJsonRoot, "number", 10010);
    cJSON_AddBoolToObject(pJsonRoot, "bool", 1);
    cJSON * pSubJson = NULL;
    pSubJson = cJSON_CreateObject();
    if(NULL == pSubJson)
    {
        // create object faild, exit
        cJSON_Delete(pJsonRoot);
        return NULL;
    }
    cJSON_AddStringToObject(pSubJson, "subjsonobj", "a sub json string");
    cJSON_AddItemToObject(pJsonRoot, "subobj", pSubJson);

    char * p = cJSON_Print(pJsonRoot);
  // else use :
    // char * p = cJSON_PrintUnformatted(pJsonRoot);
    if(NULL == p)
    {
        //convert json list to string faild, exit
        //because sub json pSubJson han been add to pJsonRoot, so just delete pJsonRoot, if you also delete pSubJson, it will coredump, and error is : double free
        cJSON_Delete(pJsonRoot);
        return NULL;
    }
    //free(p);

    cJSON_Delete(pJsonRoot);

    return p;
}

void parseJson(char * pMsg)
{
    uint8_t *p;
    if(NULL == pMsg)
    {
        return;
    }
    cJSON * pJson = cJSON_Parse(pMsg);
    if(NULL == pJson)
    {
        // parse faild, return
      return ;
    }

    // get string from json
    cJSON * pSub = cJSON_GetObjectItem(pJson, "hello");
    if(NULL == pSub)
    {
        //get object named "hello" faild
    }
    printf_safe("obj_1 : %s\n", pSub->valuestring);

    // get number from json
    pSub = cJSON_GetObjectItem(pJson, "number");
    if(NULL == pSub)
    {
        //get number from json faild
    }
    printf_safe("obj_2 : %d\n", pSub->valueint);
    //pSub->valuedouble = 1.1;
    cJSON_ReplaceItemInObject(pJson, "number", cJSON_CreateNumber(200));



    // get bool from json
    pSub = cJSON_GetObjectItem(pJson, "bool");
    if(NULL == pSub)
    {
        // get bool from json faild
    }
    printf_safe("obj_3 : %d\n", pSub->valueint);

 // get sub object
    pSub = cJSON_GetObjectItem(pJson, "subobj");
    if(NULL == pSub)
    {
        // get sub object faild
    }
    cJSON * pSubSub = cJSON_GetObjectItem(pSub, pSub->child->string);
    if(NULL == pSubSub)
    {
        // get object from subject object faild
    }
    printf_safe("sub_obj_1 : %s\n", pSubSub->valuestring);
    cJSON_ReplaceItemInObject(pSub, "subjsonobj", cJSON_CreateString("RGW"));
    //pSubSub->valuestring = "RGW";

    p = cJSON_Print(pJson);
    printf_safe("\n******modify\n");
    printf_safe("%s", p);
    free(p);

    cJSON_Delete(pJson);
}
//创建数组，数组值是另一个JSON的item，这里使用数字作为演示
char * makeArray(int iSize)
{
    cJSON * root =  cJSON_CreateArray();
    if(NULL == root)
    {
        printf("create json array faild\n");
        return NULL;
    }
    int i = 0;

    for(i = 0; i < iSize; i++)
    {
        cJSON_AddItemToArray(root , cJSON_CreateNumber(i));
    }
    char * out = cJSON_Print(root);
    cJSON_Delete(root);

    return out;
}

//解析刚刚的CJSON数组
void parseArray(char * pJson)
{
    if(NULL == pJson)
    {
        return ;
    }
    cJSON * root = NULL;
    if((root = cJSON_Parse(pJson)) == NULL)
    {
        return ;
    }
    int iSize = cJSON_GetArraySize(root);
    for(int iCnt = 0; iCnt < iSize; iCnt++)
    {
        cJSON * pSub = cJSON_GetArrayItem(root, iCnt);
        if(NULL == pSub)
        {
            continue;
        }
        int iValue = pSub->valueint;
        printf("value[%2d] : [%d]\n", iCnt, iValue);
    }
    cJSON_Delete(root);
    return;
}
static void cli_makeJson_fnt(int argc, char **argv)
{
    FIL fp;
    int i;
    uint32_t bw;
    uint8_t * p = makeJson();
    printf_safe("\n%s\n", p);
//    p = makeArray(20);
//    printf_safe("\n%s\n", p);
    f_open(&fp, "test.json", FA_CREATE_ALWAYS | FA_WRITE);
    f_write(&fp, p, strlen(p), &bw);
    printf_safe("写如%d字节\n",bw);
    f_close(&fp);
    free(p);
}
static void cli_parseJson_fnt(int argc, char **argv)
{
    FIL fp;
    uint32_t flen,br;
    uint8_t *p = NULL;
    f_open(&fp, "test.json", FA_READ);
    flen = f_size(&fp);
    p = (uint8_t *)malloc(flen* sizeof(uint8_t));
    f_read(&fp, p, flen, &br);
    printf_safe("读取%d字节\n",br);
    f_close(&fp);

    parseJson(p);
    free(p);

}
tinysh_cmd_t cli_parseJson_cmd = {0, "parsejson", "json解析测试", "[args]",
                                cli_parseJson_fnt, 0, 0, 0
                               };

tinysh_cmd_t cli_makeJson_cmd = {0, "makejson", "json创建测试", "[args]",
                                       cli_makeJson_fnt, 0, 0, 0
                                      };

/**
* @file cli_json.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-08
*/
#include "includes.h"
#include "cJSON.h"

char *makeJson()
{
    cJSON *pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if(NULL == pJsonRoot)
    {
        //error happend here
        return NULL;
    }
    cJSON_AddStringToObject(pJsonRoot, "hello1", "hello world1");
    cJSON_AddStringToObject(pJsonRoot, "hello2", "hello world2");
    cJSON_AddStringToObject(pJsonRoot, "hello3", "hello world3");
    cJSON_AddStringToObject(pJsonRoot, "hello4", "hello world4");
    cJSON_AddStringToObject(pJsonRoot, "hello5", "hello world5");
    cJSON_AddStringToObject(pJsonRoot, "hello6", "hello world6");
    cJSON_AddNumberToObject(pJsonRoot, "number1", 1234);
    cJSON_AddNumberToObject(pJsonRoot, "number2", 431);
    cJSON_AddNumberToObject(pJsonRoot, "number3", 12.3);
    cJSON_AddNumberToObject(pJsonRoot, "number4", 234);
    cJSON_AddNumberToObject(pJsonRoot, "number5", 4535);
    cJSON_AddNumberToObject(pJsonRoot, "number6", 1234);
    cJSON_AddNumberToObject(pJsonRoot, "number7", 4543);
    cJSON_AddNumberToObject(pJsonRoot, "number8", 567);
    cJSON_AddNumberToObject(pJsonRoot, "number9", 234);
    cJSON_AddNumberToObject(pJsonRoot, "number0", 45676);
    cJSON_AddBoolToObject(pJsonRoot, "bool", 1);
    cJSON *pSubJson = NULL;
    pSubJson = cJSON_CreateObject();
    if(NULL == pSubJson)
    {
        // create object faild, exit
        cJSON_Delete(pJsonRoot);
        return NULL;
    }
    cJSON_AddStringToObject(pSubJson, "subjsonobj", "a sub json string");
    cJSON_AddItemToObject(pJsonRoot, "subobj", pSubJson);

    char *p = cJSON_Print(pJsonRoot);
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

void parseJson(char *pMsg)
{
    uint8_t *p;
    if(NULL == pMsg)
    {
        return;
    }
    cJSON *pJson = cJSON_Parse(pMsg);
    if(NULL == pJson)
    {
        // parse faild, return
        return ;
    }

    // get string from json
    cJSON *pSub = cJSON_GetObjectItem(pJson, "hello5");
    if(NULL == pSub)
    {
        //get object named "hello" faild
    }
    printf_safe("hello5 : %s\n", pSub->valuestring);

    // get number from json
    pSub = cJSON_GetObjectItem(pJson, "number8");
    if(NULL == pSub)
    {
        //get number from json faild
    }
    printf_safe("number8 : %d\n", pSub->valueint);
    //pSub->valuedouble = 1.1;
    cJSON_ReplaceItemInObject(pJson, "number8", cJSON_CreateNumber(200));//�޸�number��ֵ



    // get bool from json
    pSub = cJSON_GetObjectItem(pJson, "bool");
    if(NULL == pSub)
    {
        // get bool from json faild
    }
    printf_safe("bool : %d\n", pSub->valueint);

// get sub object
    pSub = cJSON_GetObjectItem(pJson, "subobj");
    if(NULL == pSub)
    {
        // get sub object faild
    }
    cJSON *pSubSub = cJSON_GetObjectItem(pSub, pSub->child->string);
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
//�������飬����ֵ����һ��JSON��item������ʹ��������Ϊ��ʾ
char *makeArray(int iSize)
{
    cJSON *root =  cJSON_CreateArray();
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
    char *out = cJSON_Print(root);
    cJSON_Delete(root);

    return out;
}

//�����ոյ�CJSON����
void parseArray(char *pJson)
{
    if(NULL == pJson)
    {
        return ;
    }
    cJSON *root = NULL;
    if((root = cJSON_Parse(pJson)) == NULL)
    {
        return ;
    }
    int iSize = cJSON_GetArraySize(root);
    for(int iCnt = 0; iCnt < iSize; iCnt++)
    {
        cJSON *pSub = cJSON_GetArrayItem(root, iCnt);
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
    UINT bw;
    int i;

    uint8_t *p = makeJson();
    printf_safe("\n");
    printf_safe("%s", p);
    printf_safe("\n");
    f_open(&fp, "test.json", FA_CREATE_ALWAYS | FA_WRITE);
    f_write(&fp, p, strlen(p), &bw);
    printf_safe("д��%d�ֽ�\n", bw);
    f_close(&fp);
    free(p);
}
static void cli_parseJson_fnt(int argc, char **argv)
{
    FIL fp;
    UINT flen, br;
    uint8_t *p = NULL;
    f_open(&fp, "test.json", FA_READ);
    flen = f_size(&fp);
    p = (uint8_t *)malloc(flen * sizeof(uint8_t));
    f_read(&fp, p, flen, &br);
    printf_safe("��ȡ%d�ֽ�\n", br);
    f_close(&fp);

    parseJson(p);
    free(p);

}
tinysh_cmd_t cli_parseJson_cmd = {0, "parsejson", "json��������", "[args]",
                                  cli_parseJson_fnt, 0, 0, 0
                                 };

tinysh_cmd_t cli_makeJson_cmd = {0, "makejson", "json��������", "[args]",
                                 cli_makeJson_fnt, 0, 0, 0
                                };

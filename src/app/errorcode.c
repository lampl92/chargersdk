/**
* @file errorcode.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#include "includes.h"
#include "errorcode.h"

//
//  !!! һ��Ҫ��֤code�� **˳��** ��ͷ�ļ�����һ��
//
const uint8_t *strErrorCode[100] =
{
    "�޹���",
    "��ͣ��ⷢ������",
    "�������ٶȴ�������������",
    "�����ӵ������Դ�������������",
    "����״̬�������������",
    "��������������",

    "���ǹ ���ͨ�Ź���",
    "���ǹ ���CP����",
    "���ǹ ���CC����",
    "���ǹ ��ǹ״̬�������������",
    "���ǹ B������ǹ��״̬�������������",
    "���ǹ B������ǹ���޷���ֹ",
    "���ǹ L�����¶ȼ�ⷢ������",
    "���ǹ N�����¶ȼ�ⷢ������",
    "���ǹ B�������¶ȼ���1��������",
    "���ǹ B�������¶ȼ���2��������",
    "���ǹ ��ʼ������",
    "���ǹ ����������",
    "���ǹ ����ճ��",

    "�ļ���д����",
    "��Ŀ���ļ�",
    "������������",
    "�������ݴ���",
    "���ò�����Χ����",//   ERR_SET_PARAM,          //
    "JSON���л�����",//ERR_SET_SERIALIZATION
    "��������",
    "����������",
    "GPRSģ�����",
    "������żУ�����",
    "������������",
    "����֡����",
    "���ڳ���",
    "����DMA�������",
    "����ͨ�Ź���",
    "����ͨ�ų�ʱ",
    "�ڴ�������",

    "�����Ų����"
};

void ThrowErrorCode(uint32_t ulDevID, ErrorCode_t errcode, ErrorLevel_t errlevel, uint8_t *msg)
{
    ErrorPackage_t package;

    package.ulDevID = ulDevID;
    package.code = errcode;
    package.level = errlevel;
    strcpy(package.msg, msg);

    xQueueSend(xHandleQueueErrorPackage, (void *)&package, 0);
}

void ThrowFSCode (FRESULT rc, uint8_t *path, uint8_t *info)
{
    const char *str =
        "OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
        "INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
        "INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
        "LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0" "INVALID_PARAMETER\0";
    FRESULT i;
    if(rc != 0)
    {
        for (i = 0; i != rc && *str; i++)
        {
            while (*str++) ;
        }
        printf_safe("%s, %s, rc=%u FR_%s\n", (UINT)rc, str);
    }
}

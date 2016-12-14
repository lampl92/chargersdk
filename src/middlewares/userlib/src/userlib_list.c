#include "userlib_list.h"
#include <stdlib.h>

//List�����������
static void ListDelete(void *pList)
{
    if(((List *)pList)->pListPointArray != NULL)     //���ͷ�ָ������
    {
        free(((List *)pList)->pListPointArray);
    }
    free(pList);                                     //���ͷ�����List��
}
//Ԫ�����Ӻ���
static void ListAdd(List *pList, void *pListPoint)
{
    void **tListPointArray = malloc(sizeof(int *) * (pList->Total + 1));     //�����ԭ����һ���洢��Ԫ���ڴ�
    int pListIndex;
    for(pListIndex = 0; pListIndex < pList->Total; pListIndex++)        //����
    {
        if(pList->pListPointArray[pListIndex] == pListPoint)                 //�жϣ��������ͬ��Ԫ�ش���
        {   
            free(tListPointArray);                                        //�ͷ���������ڴ�
            return;                                                     //����
        }
        tListPointArray[pListIndex] = pList->pListPointArray[pListIndex];         //����
    }
    tListPointArray[pList->Total] = pListPoint;                              //����ӵ�Ԫ�طŵ����һ���洢��Ԫ��
    pList->Total += 1;                                                  //������1
    if(pList->pListPointArray != NULL) free(pList->pListPointArray);                      //�ͷ�ԭ�����ڴ�
    pList->pListPointArray = tListPointArray;                                            //���µľ���滻ԭ���
}
//Ԫ���Ƴ�����
static void ListRemove(List *pList, void *pListPoint)
{
    int pListIndex, tListIndex;
    void **tListPointArray;
    void **FreePointArray;
    void **SavePointArray;
    if(pList->Total == 0) return;                                       //����Ϊ0ʱ�˳�
    tListPointArray = malloc(sizeof(int) * (pList->Total - 1));    //�����ԭ��Сһ���洢��Ԫ���ڴ�
    FreePointArray = tListPointArray;                                  //����������ڴ�ռ���ΪĬ�ϵ��ͷſռ�
    SavePointArray = pList->pListPointArray;                           //�����е��ڴ�ռ���ΪĬ�ϵĴ洢�ռ�
    for(pListIndex = 0, tListIndex= 0; pListIndex < pList->Total; pListIndex++) //�����Ƴ���
    {
        if(pList->pListPointArray[pListIndex] == pListPoint)         //��ǰ�����Ƴ���
        {
            FreePointArray = pList->pListPointArray;            //�ı��ͷ��ڴ�ָ��
            SavePointArray = tListPointArray;                   //�ı䱣���ڴ�ָ��
            continue;                                           //��������ѭ��
        }
        if(tListIndex < (pList->Total -1))                      //�����ǰ�㲻���Ƴ��㣬�������С��������1
        {
            tListPointArray[tListIndex] = pList->pListPointArray[pListIndex];     //����
            tListIndex++;                                               //������ż�1
        }
    }
    pList->Total = (SavePointArray == tListPointArray) ? pList->Total - 1 : pList->Total;   //���ݱ������ڴ��ı�������ֵ
    if(FreePointArray != NULL) free(FreePointArray);        //�ͷŸ��ͷŵĲ��õ��ڴ��
    pList->pListPointArray = SavePointArray; //�����ñ�����
}
//List���캯��
List *ListCreate(void)
{
    List *pList = (List *)malloc(sizeof(List));
    pList->Total = 0;
    pList->pListPointArray = NULL;
    pList->Add = ListAdd;
    pList->Remove = ListRemove;
    pList->Delete = ListDelete;
    return pList;
}

#include "userlib_list.h"
#include <stdlib.h>

//List类的析构函数
static void ListDelete(void *pList)
{
    if(((List *)pList)->pListPointArray != NULL)     //先释放指针数组
    {
        free(((List *)pList)->pListPointArray);
    }
    free(pList);                                     //再释放整个List类
}
//元素增加函数
static void ListAdd(List *pList, void *pListPoint)
{
    void **tListPointArray = malloc(sizeof(int *) * (pList->Total + 1));     //申请比原来大一个存储单元的内存
    int pListIndex;
    for(pListIndex = 0; pListIndex < pList->Total; pListIndex++)            //拷贝
    {
        if(pList->pListPointArray[pListIndex] == pListPoint)                 //判断，如果有相同的元素存在
        {
            free(tListPointArray);                                          //释放现申请的内存
            return;                                                         //返回
        }
        tListPointArray[pListIndex] = pList->pListPointArray[pListIndex];         //拷贝
    }
    tListPointArray[pList->Total] = pListPoint;                              //将添加的元素放到最后一个存储单元中
    pList->Total += 1;                                                  //总数加1
    if(pList->pListPointArray != NULL)
    {
        free(pList->pListPointArray);    //释放原来的内存
    }
    pList->pListPointArray = tListPointArray;                                            //将新的句柄替换原句柄
}
//元素移除函数
static void ListRemove(List *pList, void *pListPoint)
{
    int pListIndex, tListIndex;
    void **tListPointArray;
    void **FreePointArray;
    void **SavePointArray;
    if(pList->Total == 0)
    {
        return;    //总数为0时退出
    }
    tListPointArray = malloc(sizeof(int) * (pList->Total - 1));    //申请比原来小一个存储单元的内存
    FreePointArray = tListPointArray;                                  //将刚申请的内存空间作为默认的释放空间
    SavePointArray = pList->pListPointArray;                           //将已有的内存空间作为默认的存储空间
    for(pListIndex = 0, tListIndex = 0; pListIndex < pList->Total; pListIndex++) //查找移除点
    {
        if(pList->pListPointArray[pListIndex] == pListPoint)         //当前点是移除点
        {
            FreePointArray = pList->pListPointArray;            //改变释放内存指针
            SavePointArray = tListPointArray;                   //改变保留内存指针
            continue;                                           //结束本次循环
        }
        if(tListIndex < (pList->Total - 1))                     //如果当前点不是移除点，拷贝序号小于总量减1
        {
            tListPointArray[tListIndex] = pList->pListPointArray[pListIndex];     //拷贝
            tListIndex++;                                               //拷贝序号加1
        }
    }
    pList->Total = (SavePointArray == tListPointArray) ? pList->Total - 1 : pList->Total;   //根据保留的内存块改变总数的值
    if(FreePointArray != NULL)
    {
        free(FreePointArray);    //释放该释放的不用的内存块
    }
    pList->pListPointArray = SavePointArray;                //保留该保留的
}
//List构造函数
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

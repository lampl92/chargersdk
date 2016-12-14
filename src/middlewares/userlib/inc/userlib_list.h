#ifndef __USER_LIB_LIST_H
#define __USER_LIB_LIST_H
//LIST数组
typedef struct _List
{
    void **pListPointArray;                         //LIST数组指针
    int Total;                                      //元素个数
    void (*Add)(struct _List *pList, void *pListPoint);     //添加
    void (*Remove)(struct _List *pList, void *pListPoint);  //移除
    void (*Delete)(void *pList);                    //析构
}List;

List *ListCreate(void);
#endif

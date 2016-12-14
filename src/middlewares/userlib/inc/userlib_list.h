#ifndef __USER_LIB_LIST_H
#define __USER_LIB_LIST_H
//LIST����
typedef struct _List
{
    void **pListPointArray;                         //LIST����ָ��
    int Total;                                      //Ԫ�ظ���
    void (*Add)(struct _List *pList, void *pListPoint);     //���
    void (*Remove)(struct _List *pList, void *pListPoint);  //�Ƴ�
    void (*Delete)(void *pList);                    //����
}List;

List *ListCreate(void);
#endif

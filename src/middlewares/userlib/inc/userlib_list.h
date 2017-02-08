#ifndef __USER_LIB_LIST_H
#define __USER_LIB_LIST_H
//LIST����
typedef struct _UserList_t
{
    void **pListPointArray;                         //LIST����ָ��
    int Total;                                      //Ԫ�ظ���
    void (*Add)(struct _UserList_t *pList, void *pListPoint);     //���
    void (*Remove)(struct _UserList_t *pList, void *pListPoint);  //�Ƴ�
    void (*Delete)(void *pList);                    //����
}UserList_t;
UserList_t *UserListCreate(void);
#endif

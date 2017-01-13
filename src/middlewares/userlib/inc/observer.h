#ifndef __OBSERVER_H__
#define __OBSERVER_H__
#include <string.h>
#include "userlib_list.h"


//����֪ͨ�߽ӿ�
typedef struct _Subject
{
    char *(*GetSubjectState)(void *pSubject);
    void (*SetSubjectState)(void *pSubject, char *pSubjectState);
    void (*Attach)(void *pSubject, void *pObserver);
    void (*Detach)(void *pSubject, void *pObserver);
    void (*Notify)(void *pSubject);
    void (*Delete)(void *pSubject);
}Subject_t;

//����۲���
typedef struct _Observer
{
    char *Name;
    Subject_t *pSub;
    void (*Update)(void *pObserver);
    void (*Delete)(void *pObserver);
}Observer_t;

typedef struct _Boss
{
    Subject_t mSubject;
    List *ObserverList;
    char *Action;
}Boss_t;

//����Ʊ��ͬ��
typedef struct _StockObserver
{
    Observer_t mObserver;
} StockObserver_t;
//��NBA��ͬ��
typedef struct _NBAObserver
{
    Observer_t mObserver;
} NBAObserver_t;

Observer_t *ObserverCreate(char *Name, void *pSub, size_t Size);
Subject_t *SubjectCreate(size_t Size);
#endif

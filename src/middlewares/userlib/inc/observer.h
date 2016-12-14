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
}Subject;

//����۲���
typedef struct _Observer
{
    char *Name;
    Subject *pSub;
    void (*Update)(void *pObserver);
    void (*Delete)(void *pObserver);
}Observer;



Observer *ObserverCreate(char *Name, void *pSub, size_t Size);
Subject *SubjectCreate(size_t Size);
#endif

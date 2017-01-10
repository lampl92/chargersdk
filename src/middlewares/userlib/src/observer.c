/**
* @file observer.c
* @brief 订阅发布设计模式
* @author rgw
* @version v1.0
* @date 2016-12-14
*/
#include <stdlib.h>
#include <string.h>
#include "observer.h"

static void ObserverDelete(void *pObserver)
{
    free(pObserver);
}

Observer_t *ObserverCreate(char *Name, void *pSub, size_t Size)
{
    Observer_t *pObserver = (Observer_t *)malloc(Size);
    pObserver->Name = Name;
    pObserver->pSub = pSub;
    pObserver->Delete = ObserverDelete;
    return pObserver;
}


static void SubjectDelete(void *pSubject)
{
    free(pSubject);
}

Subject_t *SubjectCreate(size_t Size)
{
    Subject_t *pSubject = (Subject_t *)malloc(Size);
    pSubject->Delete = SubjectDelete;
    return pSubject;
}

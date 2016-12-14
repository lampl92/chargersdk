#include <stdlib.h>
#include <string.h>
#include "observer.h"

static void ObserverDelete(void *pObserver)
{
    free(pObserver);
}

Observer *ObserverCreate(char *Name, void *pSub, size_t Size)
{
    Observer *pObserver = (Observer *)malloc(Size);
    pObserver->Name = Name;
    pObserver->pSub = pSub;
    pObserver->Delete = ObserverDelete;
    return pObserver;
}


static void SubjectDelete(void *pSubject)
{
    free(pSubject);
}

Subject *SubjectCreate(size_t Size)
{
    Subject *pSubject = (Subject *)malloc(Size);
    pSubject->Delete = SubjectDelete;
    return pSubject;
}

#include <stdlib.h>
#include "observer.h"
//具体通知者

static char *BossGetSubjectState(void *pBoss)
{
    return ((Boss_t *)pBoss)->Action;
}
static void BossSetSubjectState(void *pBoss, char *pBossState)
{
    ((Boss_t *)pBoss)->Action = pBossState;
}
static void BossAttach(void *pBoss, void *pObserver)
{
    List *pList = ((Boss_t *)pBoss)->ObserverList;
    pList->Add(pList, pObserver);
}
static void BossDetach(void *pBoss, void *pObserver)
{
    List *pList = ((Boss_t *)pBoss)->ObserverList;
    pList->Remove(pList, pObserver);
}
static void BossNotify(void *pBoss)
{
    List *pList = ((Boss_t *)pBoss)->ObserverList;
    int i;
    for(i = 0; i < pList->Total; i++)
    {
        ((Observer_t *)(pList->pListPointArray[i]))->Update(((Observer_t *)(pList->pListPointArray[i])));
    }
}
static void BossDelete(void *pBoss)
{
    ((Boss_t *)pBoss)->ObserverList->Delete(((Boss_t *)pBoss)->ObserverList);
    free(pBoss);
}
Boss_t *BossCreate(size_t Size)
{
    Boss_t *pBoss = (Boss_t *)SubjectCreate(Size);
    pBoss->ObserverList = ListCreate();
    //重写函数
    ((Subject_t *)pBoss)->GetSubjectState = BossGetSubjectState;
    ((Subject_t *)pBoss)->SetSubjectState = BossSetSubjectState;
    ((Subject_t *)pBoss)->Attach = BossAttach;
    ((Subject_t *)pBoss)->Detach = BossDetach;
    ((Subject_t *)pBoss)->Notify = BossNotify;
    ((Subject_t *)pBoss)->Delete = BossDelete;
    return pBoss;
}

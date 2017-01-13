#include "observer.h"
//����۲���

static void StockObserverUpdata(void *pStockObserver)
{
    Subject_t *pSub = ((Observer_t *)pStockObserver)->pSub;
//   pSub->GetSubjectState(pSub);
//   pSub->GetSubjectState(pSub);
//   char *State = *pSub->GetSubjectState(pSub);
    printf(" %s��%s�رչ�Ʊ���飬��������\n"
           , pSub->GetSubjectState(pSub)
           , ((Observer_t *)pStockObserver)->Name);
}
StockObserver_t *StockObserverCreate(char *Name, void *pSub, size_t Size)
{
    StockObserver_t *pStockObserver = (StockObserver_t *)ObserverCreate(Name, pSub, Size);
    ((Observer_t *)pStockObserver)->Update = StockObserverUpdata;
    return pStockObserver;
}




static void NBAObserverUpdata(void *pNBAObserver)
{
    Subject_t *pSub = ((Observer_t *)pNBAObserver)->pSub;
    printf(" %s��%s�ر�NBAת������������\n", pSub->GetSubjectState(pSub), ((Observer_t *)pNBAObserver)->Name);
}
NBAObserver_t *NBAObserverCreate(char *Name, void *pSub, size_t Size)
{
    NBAObserver_t *pNBAObserver = (NBAObserver_t *)ObserverCreate(Name, pSub, Size);
    ((Observer_t *)pNBAObserver)->Update = NBAObserverUpdata;
    return pNBAObserver;
}

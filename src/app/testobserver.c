#include <stdio.h>
#include <stdlib.h>
#include "observer.h"

//客户端
void ObserverModel(void)
{
    Boss_t *huhasan = BossCreate(sizeof(Boss_t));
    StockObserver_t *tongshi1 = StockObserverCreate("小明", huhasan, sizeof(StockObserver_t));
    NBAObserver_t *tongshi2 = NBAObserverCreate("Jack", huhasan, sizeof(NBAObserver_t));
    ((Subject_t *)huhasan)->Attach(huhasan, tongshi1);
    ((Subject_t *)huhasan)->Attach(huhasan, tongshi2);
    ((Subject_t *)huhasan)->SetSubjectState(huhasan, "(Boss comming......)");
    ((Subject_t *)huhasan)->Notify(huhasan);
    ((Subject_t *)huhasan)->Detach(huhasan, tongshi2);
    ((Subject_t *)huhasan)->Notify(huhasan);

    ((Subject_t *)huhasan)->Delete(huhasan);
    ((Observer_t *)tongshi1)->Delete(tongshi1);
    ((Observer_t *)tongshi2)->Delete(tongshi2);
}

void testobserver(void)
{
    while(1)
    {
        ObserverModel();
        vTaskDelay(1000);
    }
}

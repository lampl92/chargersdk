#ifndef GUI_BackStage_H
#define GUI_BackStage_H

#include "GUI_backstage.h"
#include "includes.h"
#include "taskcreate.h"
#include "taskrfid.h"
#include "interface.h"
#include "utils.h"

typedef enum MyEnum
{
    HomeState,
    GetGunInfoState,
    GetBalanceState,
    KnowBalanceState,
}GBSState_E;//后台状态

typedef enum gunenum
{
    GunfreeState = 1,
    GunchargingState,
    GunchargedoneState,
    Gunerror,
}GUNState_E;

typedef struct orderinfo
{
    RfidQPkg_t rfid_pkg;
}OrderInfo_S;

typedef struct userlike
{
    UserChargeCondition_t user_like;
    int UserLikeFlag;//1表示用户选完
}UserLike_S;

extern GUNState_E gunstate[2];
extern GBSState_E gbsstate;
extern OrderInfo_S GunInfo[defMaxCON];
extern RfidQPkg_t Temprfid_pkg;//没选枪之前保存刷卡的卡号
extern UserLike_S Tempuserlike;

#endif 

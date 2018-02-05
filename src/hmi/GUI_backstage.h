#ifndef GUI_BackStage_H
#define GUI_BackStage_H

#include "GUI_backstage.h"
#include "includes.h"
#include "taskcreate.h"
#include "taskrfid.h"
#include "interface.h"
#include "utils.h"

extern double GBSBalance;

typedef enum MyEnum
{
    StateHome,
    StateGetGunInfo,
    StateReadyStart,
    StateNetTimeout,
    StateCardconditionNotOk,
    StatePlugTimeout,
    StatePleasePlug,
    StateChargingOk,
}GBSState_E;//后台状态

typedef struct userlike
{
    UserChargeCondition_t user_like;
    int UserLikeFlag;//1表示用户选完
}UserLike_S;


extern GBSState_E gbsstate;
extern RfidQPkg_t Temprfid_pkg;//没选枪之前保存刷卡的卡号
extern UserLike_S Tempuserlike;

void GBSTask();

#endif 

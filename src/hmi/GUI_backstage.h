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
}GBSState_E;

typedef struct orderinfo
{
    RfidQPkg_t rfid_pkg;
}OrderInfo_S;

typedef struct userlike
{
    UserChargeCondition_t user_like;
    int UserLikeFlag;//1��ʾ�û�ѡ��
}UserLike_S;

extern GBSState_E gbsstate;
extern OrderInfo_S GunInfo[defMaxCON];
extern RfidQPkg_t Temprfid_pkg;//ûѡǹ֮ǰ����ˢ���Ŀ���
extern UserLike_S Tempuserlike;

#endif 

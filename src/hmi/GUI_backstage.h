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
    StateHome,//主状态
    StateGetGunInfo,//选枪状态
    StateOperateTimeout,//操作超时
    StateQuit,//退出
    StatePwd,//需要密码
    StateTestPwd,//校验密码
    StatePwdErrorAgain,//密码错误,重新输入
    StatePwdFull,//密码次数超限
    StateTestChargeCondition,//检测充电条件    
    StateNetTimeout,//网络超时
    StateCardconditionNotOk,//卡条件不足
    StatePlugTimeout,//连接超时
    StatePleasePlug,//请连接充电插头
    StateWaitBecomeCharge,//等待进入充电
    StateCantChargeOfDevice,//设备原因无法启动
    StateChargingOk,//启动充电成功
}GBSState_E;//后台状态

typedef enum gunenum
{
    GunfreeState       = 1,//枪空闲
    GunchargingState,//枪正在充电
    GunchargedoneState,//枪充电完成
    Gunerror,//枪错误
}GUNState_E;//后台状态StateHome用来做枪状态显示

typedef struct userlike
{
    UserChargeCondition_t user_like;
    int UserLikeFlag;//1表示用户选完
}UserLike_S;


extern GBSState_E gbsstate;
extern GUNState_E GBSgunstate[2];
extern RfidQPkg_t Temprfid_pkg;//没选枪之前保存刷卡的卡号
extern UserLike_S Tempuserlike;
extern int quitflag;

void GBSTask();

#endif 

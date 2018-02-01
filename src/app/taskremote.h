/**
* @file taskremote.h
* @brief 远程通信的操作，接收通信实例的指针
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#ifndef  __TASKREMOTE_H
#define  __TASKREMOTE_H

typedef enum
{
    REMOTE_NO,
    REMOTE_CONNECTED,
    REMOTE_REGEDITED,
    REMOTE_RECONNECT,
    REMOTE_ERROR
} RemoteState_t;

typedef enum
{
    REMOTEHEART_IDLE,
    REMOTEHEART_RECV
} RemoteHeartState_e;

////////远程状态///////////

//刷卡启动数据结构
typedef enum _statCardCtrl
{
    CARDCTRL_IDLE,
    CARDCTRL_WAIT_START,
    CARDCTRL_WAIT_START_RECV,
    CARDCTRL_WAIT_STOP,
    CARDCTRL_WAIT_STOP_RECV
} statCardCtrl_e;

typedef struct
{
    statCardCtrl_e stat;
    time_t timestamp;
}RemoteCardStatus_t;

//上传订单数据结构
typedef enum _statRemoteOrder_e
{
    REMOTEOrder_IDLE,
    REMOTEOrder_Send,
    REMOTEOrder_WaitRecv
} statRemoteOrder_e;

typedef struct
{
    uint8_t ucCheckOrderTmp;
    statRemoteOrder_e stat;
    time_t timestamp;
}RemoteOrderStatus_t;

//远程启停数据结构
typedef enum
{
    REMOTECTRL_IDLE,
    REMOTECTRL_WAIT_START,
    REMOTECTRL_SUCC,
    REMOTECTRL_WAIT_STOP,
    REMOTECTRL_STOP,
    REMOTECTRL_FAIL
} RemoteCtrlState_e;

typedef struct
{
    uint8_t id;
    uint8_t ctrl_onoff;//1 start, 2 stop
    RemoteCtrlState_e stat;
    time_t timestamp;
}RemoteCtrlStatus_t;
//实时数据数据结构
typedef enum
{
    REMOTERTData_IDLE,
    REMOTERTData_START,
    REMOTERTData_STOP
} RemoteRTDataState_e;
typedef struct
{
    uint8_t stop_reason;
    RemoteRTDataState_e stat;
}RemoteRTDataStatus_t;

typedef struct _statRemote
{
    RemoteCardStatus_t card;
    RemoteOrderStatus_t order;
    RemoteCtrlStatus_t rmt_ctrl;
    RemoteRTDataStatus_t rtdata;
}statRemote_t;

#endif

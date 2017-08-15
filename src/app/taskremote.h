/**
* @file taskremote.h
* @brief 远程通信的操作，接收通信实例的指针
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#ifndef  __TASKREMOTE_H
#define  __TASKREMOTE_H
typedef enum _statCartCtrl
{
    CARDCTRL_IDLE,
    CARDCTRL_WAIT_START,
    CARDCTRL_SUCC,
    CARDCTRL_WAIT_STOP,
    CARDCTRL_STOP,
    CARDCTRL_FAIL
} statCartCtrl_e;

typedef struct
{
    statCartCtrl_e stat;
    time_t timestamp;
}RemoteCardStatus_t;

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

typedef enum
{
    REMOTECTRL_IDLE,
    REMOTECTRL_WAIT_START,
    REMOTECTRL_SUCC,
    REMOTECTRL_WAIT_STOP,
    REMOTECTRL_STOP,
    REMOTECTRL_FAIL
} RemoteCtrlState_e;

typedef enum
{
    REMOTERTData_IDLE,
    REMOTERTData_START,
    REMOTERTData_STOP
} RemoteRTDataState_e;

typedef enum
{
    REMOTEOrder_IDLE,
    REMOTEOrder_Send,
    REMOTEOrder_WaitRecv
} RemoteOrderState_e;

#endif

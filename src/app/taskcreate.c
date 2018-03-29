/**
* @file taskcreate.c
* @brief 创建任务Todolist 对照表
*        1. 定义STACK大小
*        2. 定义PRIORITY
*        3. 声明任务
*        4. 定义任务句柄
*        5. 任务入口
*        6. 创建任务创建任务Todolist 对照表
* @author rgw
* @version v1.0
* @date 2016-11-03
*/
#include "includes.h"
#include "interface.h"
#include "cli_main.h"
#include "timercallback.h"
#include "gprs_m26.h"
#include "bsp.h"
#include "modem.h"
#include "GUI_backstage.h"
/*---------------------------------------------------------------------------/
/ 任务栈大小
/---------------------------------------------------------------------------*/
#define defSTACK_TaskInit                   (1024*10)      
#define defSTACK_TaskCLI                    (1024 * 50)
//#define defSTACK_TaskGUI                    (1024*10)
#define defSTACK_TaskGUI                    (1024*20)
#define defSTACK_TaskGUIBS                 (1024)
//#define defSTACK_TaskTouch                  512
#define defSTACK_TaskTouch                  1024
#define defSTACK_TaskOTA                    512
//#define defSTACK_TaskPPP                    (1024*10)
#define defSTACK_TaskTCPClient               (1024*10)
#define defSTACK_TaskRemoteCmdProc          (1024*10)

#define defSTACK_TaskEVSERemote             (1024*10)
#define defSTACK_TaskEVSERFID               (1024*5)
#define defSTACK_TaskEVSECharge             512
#define defSTACK_TaskEVSEMonitor            512
#define defSTACK_TaskEVSEDiag               512
#define defSTACK_TaskEVSEData               (1024*50)


//#define TCPIP_THREAD_STACKSIZE      512

/*---------------------------------------------------------------------------/
/ 任务优先级
/---------------------------------------------------------------------------*/
//优先级规则为系统任务优先级低，OTA > 充电任务 > 故障处理 > 系统监视 > 刷卡与通信 > 数据处理与系统任务
#define defPRIORITY_TaskInit                31
#define defPRIORITY_TaskOTA                 31/* 最高*/

#define defPRIORITY_TaskEVSEDiag            27
#define defPRIORITY_TaskEVSECharge          25
#define defPRIORITY_TaskEVSEMonitor         23
//#define configTIMER_TASK_PRIORITY     ( defined in FreeRTOSConfig.h ) 22
#define defPRIORITY_TaskEVSERFID            20
#define defPRIORITY_TaskEVSERemote          18
#define defPRIORITY_TaskEVSEData            16

//#define defPRIORITY_TaskPPP                 14
#define defPRIORITY_TaskTCPClient           12
#define defPRIORITY_TaskRemoteCmdProc       19

#define defPRIORITY_TaskTouch               6
#define defPRIORITY_TaskGUI                 4   //不能高,GUI任务时间太长,会影响硬件响应
#define defPRIORITY_TaskGUIBS               3
#define defPRIORITY_TaskCLI                 16  //1. 原优先级2，修改16保证执行添加订单时订单存储被高优先级任务打断。2017年12月15日

//#define TCPIP_THREAD_PRIO         13 //defined in lwipopts.h
// define PPP_THREAD_PRIO          14

/*---------------------------------------------------------------------------/
/ 任务名称
/---------------------------------------------------------------------------*/
const char *TASKNAME_INIT           = "TaskInit";
const char *TASKNAME_CLI            = "TaskCLI";
const char *TASKNAME_GUI            = "TaskGUI";
const char *TASKNAME_GUIBS          = "TaskGUIBS";
const char *TASKNAME_Touch          = "TaskTouch";
const char *TASKNAME_OTA            = "TaskOTA";
const char *TASKNAME_TCP_CLIENT     = "TaskTCPClient";
const char *TASKNAME_RemoteCmdProc  = "TaskRemoteCmdProc" ;

const char *TASKNAME_EVSERemote     = "TaskEVSERemote";
const char *TASKNAME_EVSERFID       = "TaskEVSERFID";
const char *TASKNAME_EVSECharge     = "TaskEVSECharge";
const char *TASKNAME_EVSEMonitor    = "TaskEVSEMonitor";
const char *TASKNAME_EVSEDiag       = "TaskEVSEDiag";
const char *TASKNAME_EVSEData       = "TaskEVSEData";
//#define TCPIP_THREAD_NAME           "tcpip_thread"
/*---------------------------------------------------------------------------/
/ 任务声明
/---------------------------------------------------------------------------*/
void vTaskInit(void *pvParameters);
void vTaskCLI(void *pvParameters);
void vTaskGUI(void *pvParameters);
void vTaskGUIBS(void *pvParameters);
void vTaskTouch(void *pvParameters);
void vTaskOTA(void *pvParameters);
void vTaskTCPClient(void *pvParameters);
void vTaskRemoteCmdProc(void *pvParameters);

void vTaskEVSERemote(void *pvParameters);
void vTaskEVSERFID(void *pvParameters);
void vTaskEVSECharge(void *pvParameters);
void vTaskEVSEMonitor(void *pvParameters);
void vTaskEVSEDiag(void *pvParameters);
void vTaskEVSEData(void *pvParameters);

/*---------------------------------------------------------------------------/
/ 任务句柄
/---------------------------------------------------------------------------*/
static TaskHandle_t xHandleTaskInit = NULL;
static TaskHandle_t xHandleTaskCLI = NULL;
static TaskHandle_t xHandleTaskGUI = NULL;
static TaskHandle_t xHandleTaskGUIBS = NULL;
static TaskHandle_t xHandleTaskTouch = NULL;
static TaskHandle_t xHandleTaskOTA = NULL;
static TaskHandle_t xHandleTaskTCPClient = NULL;
static TaskHandle_t xHandleTaskRemoteCmdProc = NULL;

static TaskHandle_t xHandleTaskEVSERemote = NULL;
static TaskHandle_t xHandleTaskEVSERFID = NULL;
static TaskHandle_t xHandleTaskEVSECharge = NULL;
static TaskHandle_t xHandleTaskEVSEMonitor = NULL;
static TaskHandle_t xHandleTaskEVSEDiag = NULL;
static TaskHandle_t xHandleTaskEVSEData = NULL;
/*---------------------------------------------------------------------------/
/ 任务间通信
/---------------------------------------------------------------------------*/
SemaphoreHandle_t xMutexTimeStruct;
SemaphoreHandle_t xMutexNandHW;
SemaphoreHandle_t xprintfMutex = NULL;
SemaphoreHandle_t xMeterMutex = NULL;
SemaphoreHandle_t xTempMutex = NULL;


EventGroupHandle_t xHandleEventTimerCBNotify = NULL;
EventGroupHandle_t xHandleEventData = NULL;
EventGroupHandle_t xHandleEventDiag = NULL;
EventGroupHandle_t xHandleEventRemote = NULL;
EventGroupHandle_t xHandleEventHMI  = NULL;
EventGroupHandle_t xHandleEventTCP   = NULL;

//下面的Event在相应结构体中定义
//pRFIDDev->xHandleEventGroupRFID
//pCON->status.xHandleEventCharge;
//pCON->status.xHandleEventException;
//队列
QueueHandle_t xHandleQueueRfidPkg = NULL;
QueueHandle_t xHandleQueueUserChargeCondition = NULL;
QueueHandle_t xHandleQueueOrders = NULL;
QueueHandle_t xHandleQueueErrorPackage = NULL;
//Timer句柄
TimerHandle_t xHandleTimerTemp = NULL;
TimerHandle_t xHandleTimerLockState = NULL;
TimerHandle_t xHandleTimerPlugState = NULL;
TimerHandle_t xHandleTimerChargingData = NULL;
TimerHandle_t xHandleTimerEVSEState = NULL;
TimerHandle_t xHandleTimerRFID = NULL;
TimerHandle_t xHandleTimerDataRefresh = NULL;
TimerHandle_t xHandleTimerRemoteHeartbeat = NULL;
TimerHandle_t xHandleTimerRemoteStatus    = NULL;
//con中还定义了几个定时器，xHandleTimerVolt，xHandleTimerCurr，xHandleTimerCharge分别在使用时进行初始化
//Mutex
void vTaskInit(void *pvParameters)
{
    AppObjCreate();
    sys_Init();
    EVSEinit();
    SysTaskCreate();
    AppTaskCreate();
#ifdef RELEASE
   // IWDG_Init(IWDG_PRESCALER_64, 2500); //64/32*2500=5s
#endif
    while (1)
    {
        IWDG_Feed();
        vTaskDelay(1000);
    }
}
void vTaskCLI(void *pvParameters)
{
    cli_main();
}

void vTaskGUI(void *pvParameters)
{
    MainTask();
}

void vTaskGUIBS(void *pvParameters)
{
    GBSTask();
}
void vTaskTouch(void *pvParameters)
{
    while(1)
    {
        GUI_TOUCH_Exec();//激活XY轴的测量
        vTaskDelay(10);
    }
}

void TaskInit(void)
{
    xTaskCreate( vTaskInit, TASKNAME_INIT, defSTACK_TaskInit, NULL, defPRIORITY_TaskInit, &xHandleTaskInit );
}

void SysTaskCreate (void)
{
    xTaskCreate( vTaskCLI, TASKNAME_CLI, defSTACK_TaskCLI, NULL, defPRIORITY_TaskCLI, &xHandleTaskCLI );
#if EVSE_USING_GUI
    xTaskCreate( vTaskGUI, TASKNAME_GUI, defSTACK_TaskGUI, NULL, defPRIORITY_TaskGUI, &xHandleTaskGUI );
    xTaskCreate(vTaskGUIBS, TASKNAME_GUIBS, defSTACK_TaskGUIBS, NULL, defPRIORITY_TaskGUIBS, &xHandleTaskGUIBS);
    xTaskCreate( vTaskTouch, TASKNAME_Touch, defSTACK_TaskTouch, NULL, defPRIORITY_TaskTouch, &xHandleTaskTouch );
#endif
    xTaskCreate( vTaskOTA, TASKNAME_OTA, defSTACK_TaskOTA, NULL, defPRIORITY_TaskOTA, &xHandleTaskOTA );
    xTaskCreate( vTaskTCPClient, TASKNAME_TCP_CLIENT, defSTACK_TaskTCPClient, NULL, defPRIORITY_TaskTCPClient, &xHandleTaskTCPClient );
    xTaskCreate( vTaskRemoteCmdProc, TASKNAME_RemoteCmdProc, defSTACK_TaskRemoteCmdProc, (void *)pechProto, defPRIORITY_TaskRemoteCmdProc, &xHandleTaskRemoteCmdProc);

}

void AppTaskCreate (void)
{
    xTaskCreate( vTaskEVSERemote, TASKNAME_EVSERemote, defSTACK_TaskEVSERemote, NULL, defPRIORITY_TaskEVSERemote, &xHandleTaskEVSERemote );
    xTaskCreate( vTaskEVSERFID, TASKNAME_EVSERFID, defSTACK_TaskEVSERFID, NULL, defPRIORITY_TaskEVSERFID, &xHandleTaskEVSERFID );
    xTaskCreate( vTaskEVSECharge, TASKNAME_EVSECharge, defSTACK_TaskEVSECharge, NULL, defPRIORITY_TaskEVSECharge, &xHandleTaskEVSECharge );
    xTaskCreate( vTaskEVSEMonitor, TASKNAME_EVSEMonitor, defSTACK_TaskEVSEMonitor, NULL, defPRIORITY_TaskEVSEMonitor, &xHandleTaskEVSEMonitor );
    xTaskCreate( vTaskEVSEDiag, TASKNAME_EVSEDiag, defSTACK_TaskEVSEDiag, NULL, defPRIORITY_TaskEVSEDiag, &xHandleTaskEVSEDiag );
    xTaskCreate( vTaskEVSEData, TASKNAME_EVSEData, defSTACK_TaskEVSEData, NULL, defPRIORITY_TaskEVSEData, &xHandleTaskEVSEData );
}

/** @brief
 */
void AppObjCreate (void)
{
    xMutexTimeStruct = xSemaphoreCreateMutex();
    xMutexNandHW = xSemaphoreCreateMutex();
    xprintfMutex = xSemaphoreCreateMutex();
    xMeterMutex = xSemaphoreCreateMutex();
    xTempMutex = xSemaphoreCreateMutex();
    
    xHandleEventTimerCBNotify = xEventGroupCreate();
    xHandleEventData = xEventGroupCreate();
    xHandleEventDiag = xEventGroupCreate();
    xHandleEventRemote = xEventGroupCreate();
    xHandleEventHMI = xEventGroupCreate();
    xHandleEventTCP = xEventGroupCreate();

    xHandleQueueRfidPkg = xQueueCreate(defMaxCON, sizeof(RfidQPkg_t));
    xHandleQueueUserChargeCondition = xQueueCreate(defMaxCON, sizeof(UserChargeCondition_t));
    xHandleQueueOrders = xQueueCreate(defMaxCON, sizeof(OrderData_t));
    xHandleQueueErrorPackage = xQueueCreate(50, sizeof(ErrorPackage_t));

    xHandleTimerTemp = xTimerCreate("TimerTemp", defMonitorTempCyc, pdTRUE, (void *)defTIMERID_Temp, vCONTimerCB);
    xHandleTimerLockState = xTimerCreate("TimerLockState", defMonitorLockStateCyc, pdTRUE, (void *)defTIMERID_LockState, vCONTimerCB);
    xHandleTimerPlugState = xTimerCreate("TimerPlugState", defMonitorPlugStateCyc, pdTRUE, (void *)defTIMERID_PlugState, vCONTimerCB);
    xHandleTimerChargingData = xTimerCreate("TimerChargingData", defMonitorChargingDataCyc, pdTRUE, (void *)defTIMERID_ChargingData, vCONTimerCB);
    xHandleTimerEVSEState = xTimerCreate("TimerEVSEState", defMonitorEVSEStateCyc, pdTRUE, (void *)defTIMERID_EVSEState, vEVSETimerCB);
#if EVSE_USING_RFID
    xHandleTimerRFID = xTimerCreate("TimerRFID", defMonitorRFIDCyc, pdTRUE, (void *)defTIMERID_RFID, vRFIDTimerCB);
#endif
    xHandleTimerDataRefresh = xTimerCreate("TimerDataRefresh", defMonitorDataRefreshCyc, pdTRUE, (void *)defTIMERID_DATAREFRESH, vEVSETimerCB);
    xHandleTimerRemoteHeartbeat = xTimerCreate("TimerHeartbeat", defRemoteHeartbeatCyc, pdTRUE, (void *)defTIMERID_RemoteHeartbeat, vEVSETimerCB);
    xHandleTimerRemoteStatus = xTimerCreate("TimerRemoteStatus", defRemoteStatusCyc, pdTRUE, (void *)defTIMERID_RemoteStatus, vEVSETimerCB);

    xTimerStart(xHandleTimerTemp, 0);
    xTimerStart(xHandleTimerLockState, 0);
    xTimerStart(xHandleTimerPlugState, 0);
    xTimerStart(xHandleTimerChargingData, 0);
    xTimerStart(xHandleTimerEVSEState, 0);
#if EVSE_USING_RFID
    //xTimerStart(xHandleTimerRFID, 0);//已经放到taskrfid中使能
#endif
    xTimerStart(xHandleTimerDataRefresh, 0);
    //TimerHeartbeat远程服务器连接后开启定时器
}

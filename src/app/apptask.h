#ifndef APPTASK_H
#define APPTASK_H

#include "includes.h"
#include "cli_main.h"

extern TaskHandle_t xHandleTaskUserIF ;
extern TaskHandle_t xHandleTaskLED ;
extern TaskHandle_t xHandleTaskMsgPro ;
extern TaskHandle_t xHandleTaskStart ;

void AppTaskCreate (void);


#endif


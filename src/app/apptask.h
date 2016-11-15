#ifndef APPTASK_H
#define APPTASK_H

#include "includes.h"
#include "cli_main.h"

extern TaskHandle_t xHandleTaskCLI ;
extern TaskHandle_t xHandleTaskGUI ;
extern TaskHandle_t xHandleTaskMsgPro ;
extern TaskHandle_t xHandleTaskStart ;


void AppTaskCreate (void);


#endif


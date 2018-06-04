#ifndef __SYSINIT_H
#define __SYSINIT_H

#include "cfg_sys.h"

#define defSysModuleGPRS    2//3G :3  2G:2
#define defSysModuleMeter   1//1 电表  2计量芯片

extern Sysconf_t   xSysconf;//存放系统初始化参数

void sys_Init(void);
void create_cfg_file(const char *path, const char *context);

#endif

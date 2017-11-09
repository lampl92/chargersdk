#ifndef __SYSINIT_H
#define __SYSINIT_H

#include "cfg_sys.h"

#define defSysModuleGPRS    2//3G :3  2G:2
#define defSysModuleMeter   1//1 电表  2计量芯片


extern FATFS NANDDISKFatFs;  /* File system object for RAM disk logical drive */
extern char NANDDISKPath[10]; /* RAM disk logical drive path */

extern Sysconf_t   xSysconf;//存放系统初始化参数

void sys_Init(void);

#endif

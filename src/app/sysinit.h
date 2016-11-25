#ifndef __SYS_H
#define __SYS_H

extern FATFS NANDDISKFatFs;  /* File system object for RAM disk logical drive */
extern char NANDDISKPath[4]; /* RAM disk logical drive path */

void sys_Init(void);

#endif

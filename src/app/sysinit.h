#ifndef __SYSINIT_H
#define __SYSINIT_H

typedef struct
{
    uint16_t ad_top;
    uint16_t ad_bottom;
    uint16_t ad_left;
    uint16_t ad_right;
} Calibrate_t;

typedef struct
{
    Calibrate_t xCalibrate;

} Sysconf_t;

extern Sysconf_t   xSysconf;


extern FATFS NANDDISKFatFs;  /* File system object for RAM disk logical drive */
extern char NANDDISKPath[4]; /* RAM disk logical drive path */

uint8_t write_sysconf_xml(void);
uint8_t read_sysconf_xml(void);
void sys_Init(void);

#endif

#ifndef __SYS_H
#define __SYS_H

typedef struct
{
    uint16_t ad_top;
    uint16_t ad_bottom;
    uint16_t ad_left;
    uint16_t ad_right;
    
}sysconf_t;

extern sysconf_t   sysconf_param;


extern FATFS NANDDISKFatFs;  /* File system object for RAM disk logical drive */
extern char NANDDISKPath[4]; /* RAM disk logical drive path */

uint8_t write_sysconf_xml(void);
uint8_t read_sysconf_xml(void);
void sys_Init(void);

#endif

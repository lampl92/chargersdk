#ifndef YAFFS_CONFIG_H
#define YAFFS_CONFIG_H

#include "sys_types.h"
#include "bsp.h"

#define NO_inline
#define LOFF_T_32_BIT
#define printf printf_safe
    
#define CONFIG_YAFFS_DIRECT
#define CONFIG_YAFFS_YAFFS2
#define CONFIG_YAFFSFS_PROVIDE_VALUES
//#define CONFIG_YAFFS_DEFINES_TYPES
#define CONFIG_YAFFS_PROVIDE_DEFS
    
typedef unsigned long loff_t;

#endif

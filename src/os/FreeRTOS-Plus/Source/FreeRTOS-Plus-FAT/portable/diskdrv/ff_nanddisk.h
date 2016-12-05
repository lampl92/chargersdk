#ifndef __NANDDISK_H__

#define __NANDDISK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ff_headers.h"


/* Create a NAND disk, supplying enough memory to hold N sectors of 512 bytes each */
FF_Disk_t *FF_NANDDiskInit( const char *pcName );

/* Unmount the volume */
BaseType_t FF_NANDDiskUnmount( FF_Disk_t *pDisk );

/* Mount the volume */
BaseType_t FF_NANDDiskMount( FF_Disk_t *pDisk );

/* Release all resources */
BaseType_t FF_NANDDiskDelete( FF_Disk_t *pDisk );

/* Show some partition information */
BaseType_t FF_NANDDiskShowPartition( FF_Disk_t *pDisk );

/* Flush changes from the driver's buf to disk */
void FF_NANDDiskFlush( FF_Disk_t *pDisk );

/* Format a given partition on an SD-card. */
BaseType_t FF_NANDDiskFormat( FF_Disk_t *pxDisk, BaseType_t aPart );

/* _RB_ Temporary function - ideally the application would not need the IO
manageer structure, just a handle to a disk. */
FF_IOManager_t *NANDdisk_ioman( FF_Disk_t *pxDisk );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __NANDDISK_H__ */

#ifndef __NAND_DISKIO_H
#define __NAND_DISKIO_H

#include "diskio.h"

DSTATUS NAND_disk_status(void);
DSTATUS NAND_disk_initialize(void);
DRESULT NAND_disk_read(BYTE *buff, DWORD sector, UINT count);
DRESULT NAND_disk_write(const BYTE *buff, DWORD sector, UINT count);
DRESULT NAND_disk_ioctl(BYTE cmd, void *buff);

#endif 



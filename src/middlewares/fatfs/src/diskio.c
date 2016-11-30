/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"     /* FatFs lower layer API */
#include "ff_gen_drv.h"
extern Disk_drvTypeDef  disk;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;
    stat = disk.drv[pdrv]->disk_status(disk.lun[pdrv]);
    return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = RES_OK;
    if(disk.is_initialized[pdrv] == 0)
    {
        disk.is_initialized[pdrv] = 1;
        stat = disk.drv[pdrv]->disk_initialize(disk.lun[pdrv]);
    }
    return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Start sector in LBA */
    UINT count      /* Number of sectors to read */
)
{
    DRESULT res = RES_PARERR;
    res = disk.drv[pdrv]->disk_read(disk.lun[pdrv], buff, sector, count);
    return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to write */
)
{
    DRESULT res = RES_PARERR;
    res = disk.drv[pdrv]->disk_write(disk.lun[pdrv], buff, sector, count);
    return res;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res = RES_PARERR;
    res = disk.drv[pdrv]->disk_ioctl(disk.lun[pdrv], cmd, buff);
    return res;
}
//TODO: 获取RTC时间
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31)  
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)     
DWORD get_fattime (void)
{
  return 0;
}

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "diskio.h"
#include "bsp_nand_flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Block Size in Bytes */
#define SECTOR_SIZE                512

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
DSTATUS NAND_disk_initialize(void)
{
    int retval;
    Stat = STA_NOINIT;

    retval = NAND_Init();
    if (retval == NAND_OK)
    {
        Stat &= ~STA_NOINIT;
    }

    return Stat;
}

/**
  * @brief  Gets Disk Status
  * @retval DSTATUS: Operation status
  */
DSTATUS NAND_disk_status(void)
{
    Stat = STA_NOINIT;

    Stat &= ~STA_NOINIT;

    return Stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT NAND_disk_read(BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_ERROR;

    if(NAND_ReadMultiSectors(buff, sector, SECTOR_SIZE, count) == NAND_OK)
    {
        res = RES_OK;
    }
    else
    {
        res = RES_ERROR;
    }

    return res;
}

/**
  * @brief  Writes Sector(s)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT NAND_disk_write(const BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_ERROR;
    if (NAND_WriteMultiSectors((uint8_t *)buff, sector, SECTOR_SIZE, count) == NAND_OK)
    {
        res = RES_OK;
    }
    else
    {
        res = RES_ERROR;
    }

    return res;
}

/**
  * @brief  I/O control operation
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
DRESULT NAND_disk_ioctl(BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;

    if (Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }

    switch (cmd)
    {

        case CTRL_SYNC:
            res = RES_OK;
            break;
        case GET_SECTOR_SIZE:
            *(WORD *)buff = SECTOR_SIZE;
            res = RES_OK;
            break;
        case GET_BLOCK_SIZE:
            *(WORD *)buff = NAND_PAGE_SIZE / SECTOR_SIZE;
            res = RES_OK;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = NAND_FormatCapacity() / SECTOR_SIZE;
            res = RES_OK;
            break;
        default:
            res = RES_PARERR;
            break;
    }

    return res;
}


/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"
#include "bsp_nand_ftl.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Block Size in Bytes */
#define BLOCK_SIZE                512

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
DSTATUS NAND_initialize (BYTE);
DSTATUS NAND_status (BYTE);
DRESULT NAND_read (BYTE, BYTE *, DWORD, UINT);
DRESULT NAND_write (BYTE, const BYTE *, DWORD, UINT);
DRESULT NAND_ioctl (BYTE, BYTE, void *);


const Diskio_drvTypeDef  NANDDISK_Driver =
{
    NAND_initialize,
    NAND_status,
    NAND_read,
    NAND_write,
    NAND_ioctl,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
DSTATUS NAND_initialize(BYTE lun)
{
    int retval;
    Stat = STA_NOINIT;

    retval = FTL_Init();
    if(retval == 0)
    {
        Stat &= ~STA_NOINIT;
    }

    return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
DSTATUS NAND_status(BYTE lun)
{
    Stat = STA_NOINIT;

    Stat &= ~STA_NOINIT;

    return Stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  lun : not used
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT NAND_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_ERROR;

    if(FTL_ReadSectors(buff, sector, BLOCK_SIZE, count) == 0)
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
  * @param  lun : not used
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT NAND_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_ERROR;
    if(FTL_WriteSectors((u8 *)buff, sector, 512, count) == 0)
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
  * @param  lun : not used
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
DRESULT NAND_ioctl(BYTE lun, BYTE cmd, void *buff)
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
            *(WORD *)buff = 512;
            res = RES_OK;
            break;
        case GET_BLOCK_SIZE:
            *(WORD *)buff = nand_dev.page_mainsize / 512;
            res = RES_OK;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = nand_dev.valid_blocknum * nand_dev.block_pagenum * nand_dev.page_mainsize / 512;
            res = RES_OK;
            break;
        default:
            res = RES_PARERR;
            break;
    }

    return res;
}


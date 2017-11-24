#include "bsp.h"
#include "bsp_cpu_flash.h"

uint32_t ADDR_FLASH_SECTOR[] = 
{ 
    0x08000000,
    0x08004000,
    0x08008000,
    0x0800C000,
    0x08010000,
    0x08020000,
    0x08040000,
    0x08060000,
    0x08080000,
    0x080A0000,
    0x080C0000,
    0x080E0000
};
/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
    uint32_t sector = 0;
  
    if ((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_SECTOR_0;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_SECTOR_1;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_SECTOR_2;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_SECTOR_3;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_SECTOR_4;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_SECTOR_5;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_SECTOR_6;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_SECTOR_7;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_SECTOR_8;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_SECTOR_9;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_SECTOR_10;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
    {
        sector = FLASH_SECTOR_11;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
    {
        sector = FLASH_SECTOR_12;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
    {
        sector = FLASH_SECTOR_13;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
    {
        sector = FLASH_SECTOR_14;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
    {
        sector = FLASH_SECTOR_15;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
    {
        sector = FLASH_SECTOR_16;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
    {
        sector = FLASH_SECTOR_17;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
    {
        sector = FLASH_SECTOR_18;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
    {
        sector = FLASH_SECTOR_19;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
    {
        sector = FLASH_SECTOR_20;  
    } 
    else if ((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
    {
        sector = FLASH_SECTOR_21;  
    }
    else if ((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
    {
        sector = FLASH_SECTOR_22;  
    }
    else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
    {
        sector = FLASH_SECTOR_23;  
    }

    return sector;
}

/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
static uint32_t GetSectorSize(uint32_t Sector)
{
    uint32_t sectorsize = 0x00;
    if ((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) ||\
       (Sector == FLASH_SECTOR_3) || (Sector == FLASH_SECTOR_12) || (Sector == FLASH_SECTOR_13) ||\
       (Sector == FLASH_SECTOR_14) || (Sector == FLASH_SECTOR_15))
    {
        sectorsize = 16 * 1024;
    }
    else if ((Sector == FLASH_SECTOR_4) || (Sector == FLASH_SECTOR_16))
    {
        sectorsize = 64 * 1024;
    }
    else
    {
        sectorsize = 128 * 1024;
    }  
    return sectorsize;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_WriteCpuFlash
*	功能说明: 写数据到CPU 内部Flash。
*	形    参: _ulFlashAddr : Flash地址
*			 _ucpSrc : 数据缓冲区
*			 _ulSize : 数据大小（单位是字节）
*	返 回 值: 0-成功，1-数据长度或地址溢出，2-写Flash出错(估计Flash寿命到)
*********************************************************************************************************
*/
uint8_t bsp_WriteCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize)
{
	uint32_t i;
	uint8_t ucRet;
    FLASH_EraseInitTypeDef EraseInitStruct;

    uint32_t FirstSector = 0, NbOfSectors = 0, Address = 0;
    uint32_t SectorError = 0;

	/* 如果偏移地址超过芯片容量，则不改写输出缓冲区 */
	if (_ulFlashAddr + _ulSize > FLASH_BASE_ADDR + FLASH_SIZE)
	{
		return 1;
	}

	/* 长度为0时不继续操作  */
	if (_ulSize == 0)
	{
		return 0;
	}
    __set_PRIMASK(1);  		/* 关中断 */
	/* FLASH 解锁 */
	HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

  /* Get the 1st sector to erase */
    FirstSector = GetSector(_ulFlashAddr);
    /* Get the number of sector to erase from 1st sector*/
    NbOfSectors = GetSector(ADDR_FLASH_SECTOR_11  +  GetSectorSize(ADDR_FLASH_SECTOR_11) - 1) - FirstSector + 1;

      /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = FirstSector;
    EraseInitStruct.NbSectors = NbOfSectors;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    {
        printf_safe("erase error!!!\n");
    }
      /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
     you have to make sure that these data are rewritten before they are accessed during code
     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
     DCRST and ICRST bits in the FLASH_CR register. */
    __HAL_FLASH_DATA_CACHE_DISABLE();
    __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

    __HAL_FLASH_DATA_CACHE_RESET();
    __HAL_FLASH_INSTRUCTION_CACHE_RESET();

    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    __HAL_FLASH_DATA_CACHE_ENABLE();
    FLASH_WaitForLastOperation(50000);                //等待上次操作完成

    


	/* 按字节模式编程（为提高效率，可以按字编程，一次写入4字节） */
	for (i = 0; i < _ulSize; i++)
	{
    	HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, _ulFlashAddr, *_ucpSrc);
    	_ulFlashAddr++;
    	_ucpSrc++;
	}

  	/* Flash 加锁，禁止写Flash控制寄存器 */
  	HAL_FLASH_Lock();
    
    __set_PRIMASK(0);  		/* 开中断 */
    
	return 0;
}

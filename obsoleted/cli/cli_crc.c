/**
* @file D:\Documents\Projects\chargersdk\src\cli\cli_aes.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-17
*/
#include "includes.h"
#include "yaffsfs.h"
#include "utils.h"

void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc)
{
    __HAL_RCC_CRC_CLK_ENABLE();
}
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc)
{
  /* Enable CRC reset state */
    __CRC_FORCE_RESET();
  
    /* Release CRC from reset state */
    __CRC_RELEASE_RESET();
}
uint32_t CRC_Handle_8(CRC_HandleTypeDef *hcrc, uint8_t pBuffer[], uint32_t BufferLength)
{
    uint32_t i = 0; /* input data buffer index */
  
     /* Processing time optimization: 4 bytes are entered in a row with a single word write,
      * last bytes must be carefully fed to the CRC calculator to ensure a correct type
      * handling by the IP */
    __HAL_CRC_DR_RESET(hcrc);

    for (i = 0; i < (BufferLength / 4); i++)
    {
        hcrc->Instance->DR = ((uint32_t)pBuffer[4*i] << 24) | ((uint32_t)pBuffer[4*i + 1] << 16) | ((uint32_t)pBuffer[4*i + 2] << 8) | (uint32_t)pBuffer[4*i + 3];      
    }
    /* last bytes specific handling */
    if ((BufferLength % 4) != 0)
    {
        if (BufferLength % 4 == 1)
        {
            *(uint8_t volatile*)(&hcrc->Instance->DR) = pBuffer[4*i];
        }
        if (BufferLength % 4 == 2)
        {
            *(uint16_t volatile*)(&hcrc->Instance->DR) = ((uint32_t)pBuffer[4*i] << 8) | (uint32_t)pBuffer[4*i + 1];
        }
        if (BufferLength % 4 == 3)
        {
            *(uint16_t volatile*)(&hcrc->Instance->DR) = ((uint32_t)pBuffer[4*i] << 8) | (uint32_t)pBuffer[4*i + 1];
            *(uint8_t volatile*)(&hcrc->Instance->DR) = pBuffer[4*i + 2];       
        }
    }
  
  /* Return the CRC computed value */ 
    return hcrc->Instance->DR;
}
int testcrc()
{
    clock_t clocknow;
    uint32_t ulCrc32;
    clocknow = clock();
    GetFileCrc32("chargesdk.bin", &ulCrc32);
    printf_safe("ulCrc32 = %X\n", ulCrc32);
    printf_safe("clock = %d\n", clock()-clocknow);

}
#if 0
int testcrchal()
{
    int fd;
    int res;
    uint8_t *pbuff;
    uint32_t size;
    uint32_t br;
    CRC_HandleTypeDef CrcHandle;
    uint32_t ulCrc32_hal = 0xFFFFFFFF;
    int i;
    
    CrcHandle.Instance = CRC;
    if (HAL_CRC_Init(&CrcHandle) != HAL_OK)
    {
      /* Initialization Error */
        printf_safe("HAL_CRC init error!\n");
        printf_safe("HAL_CRC init error!\n");
        return 0;
    }
    fd = yaffs_open(&f, "chargesdk.bin", FA_OPEN_EXISTING | FA_READ);
    size = f_size(&f);

    pbuff = (uint8_t *)malloc(size * sizeof(uint8_t));
    res = f_read(&f, (void *)pbuff, size, &br);
    if (size != br)
    {
        printf_safe("read crc file error\n");
        printf_safe("read crc file error\n");
        return 0;
    }
    ulCrc32_hal = CRC_Handle_8(&CrcHandle, pbuff, size);
    HAL_CRC_DeInit(&CrcHandle);
    
    printf_safe("ulCrc32_hal = %X\n", ulCrc32_hal);
    f_close(&f);
    free(pbuff);
    return 1;
}
#endif
static void cli_crctest_fnt(int argc, char **argv)
{
    testcrc();
    //testcrchal();
}

tinysh_cmd_t cli_crctest_cmd = {
     0,
    "crctest",
    "crctest",
    "crc测试",
    cli_crctest_fnt,
    0,
    0,
    0
};

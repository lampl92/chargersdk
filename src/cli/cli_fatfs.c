#include "includes.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "nand_diskio.h"

FATFS NANDDISKFatFs;  /* File system object for RAM disk logical drive */
FIL MyFile;          /* File object */
char NANDDISKPath[4]; /* RAM disk logical drive path */
void Error_Handler()
{
    while(1);
}
static void cli_fatfs_fnt(int argc, char **argv)
{
    FRESULT res;                                          /* FatFs function common result code */
    uint32_t byteswritten, bytesread;                     /* File write/read counts */
    uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
    uint8_t rtext[100];                                   /* File read buffer */
    BYTE work[_MAX_SS]; /* Work area (larger is better for processing time) */
    /*##-1- Link the RAM disk I/O driver #######################################*/
    if(FATFS_LinkDriver(&NANDDISK_Driver, NANDDISKPath) == 0)
    {
        /*##-2- Register the file system object to the FatFs module ##############*/
        if(f_mount(&NANDDISKFatFs, (TCHAR const *)NANDDISKPath, 0) != FR_OK)
        {
            /* FatFs Initialization Error */
            Error_Handler();
        }
        else
        {
            /*##-3- Create a FAT file system (format) on the logical drive #########*/
            if(f_mkfs((TCHAR const *)NANDDISKPath, FM_FAT, 0,work, sizeof(work)) != FR_OK)
            {
                /* FatFs Format Error */
                Error_Handler();
            }
            else
            {
                /*##-4- Create and Open a new text file object with write access #####*/
                if(f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
                {
                    /* 'STM32.TXT' file Open for write Error */
                    Error_Handler();
                }
                else
                {
                    /*##-5- Write data to the text file ################################*/
                    res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);

                    if((byteswritten == 0) || (res != FR_OK))
                    {
                        /* 'STM32.TXT' file Write or EOF Error */
                        Error_Handler();
                    }
                    else
                    {
                        /*##-6- Close the open text file #################################*/
                        f_close(&MyFile);

                        /*##-7- Open the text file object with read access ###############*/
                        if(f_open(&MyFile, "STM32.TXT", FA_READ) != FR_OK)
                        {
                            /* 'STM32.TXT' file Open for read Error */
                            Error_Handler();
                        }
                        else
                        {
                            /*##-8- Read data from the text file ###########################*/
                            res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);

                            if((bytesread == 0) || (res != FR_OK))
                            {
                                /* 'STM32.TXT' file Read or EOF Error */
                                Error_Handler();
                            }
                            else
                            {
                                /*##-9- Close the open text file #############################*/
                                f_close(&MyFile);
                                printf("%s\n",rtext);
                                /*##-10- Compare read data with the expected data ############*/
                                if ((bytesread != byteswritten))
                                {
                                    /* Read data is different from the expected data */
                                    Error_Handler();
                                }
                                else
                                {
                                    /* Success of the demo: no error occurrence */
                                    //BSP_LED_On(LED3);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /*##-11- Unlink the RAM disk I/O driver ####################################*/
    FATFS_UnLinkDriver(NANDDISKPath);

}

tinysh_cmd_t cli_fatfs_cmd = {0, "testfatfs", "fatfs command", "[args]",
                              cli_fatfs_fnt, 0, 0, 0
                             };


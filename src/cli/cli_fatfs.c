#include "includes.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "nand_diskio.h"

FIL MyFile;          /* File object */


static void cli_mkfs_fnt(int argc, char **argv)
{
    BYTE work[_MAX_SS]; /* Work area (larger is better for processing time) */
    char format;
    taskENTER_CRITICAL();
    if(strcmp(argv[1], "-t") == 0)
    {
        //FM_FAT        0x01
        //FM_FAT32      0x02
        //FM_EXFAT      0x04
        //FM_ANY        0x07
        //FM_SFD        0x08
        if(strcasecmp(argv[2], "fat") == 0)
        {
            format = FM_FAT;
        }
        else if(strcasecmp(argv[2], "fat32") == 0)
        {
            format = FM_FAT32;
        }
        else if(strcasecmp(argv[2], "exfat") == 0)
        {
            format = FM_EXFAT;
        }
        else if(strcasecmp(argv[2], "any") == 0)
        {
            format = FM_ANY;
        }
        else if(strcasecmp(argv[2], "sfd") == 0)
        {
            format = FM_SFD;
        }
    }
    if(strcasecmp(argv[3], "nand") == 0)
    {
        /*##-3- Create a FAT file system (format) on the logical drive #########*/
        if(f_mkfs((TCHAR const *)NANDDISKPath, format, 0, work, sizeof(work)) != FR_OK)
        {
            /* FatFs Format Error */
            Error_Handler();
        }
        else
        {
            printf_safe("\n格式化完成,当前文件系统%s\n", argv[2]);
        }
    }
    else if(strcmp(argv[1], "--help") == 0)
    {

        printf_safe("\nformat: FAT,FAT32,exFAT,ANY,SFD\n");

        printf_safe("\neg:mkfs -t FAT nand\n");

    }
    else
    {
        printf_safe("\nerror input\n");
        printf_safe("format: FAT,FAT32,exFAT,ANY,SFD\n");
        printf_safe("eg:mkfs -t FAT nand\n");
    }
    taskEXIT_CRITICAL();
}

static void cli_mount_fnt(int argc, char **argv)
{
    taskENTER_CRITICAL();
    if(strcasecmp(argv[1], "nand") == 0)
    {
        /*##-2- Register the file system object to the FatFs module ##############*/
        if(f_mount(&NANDDISKFatFs, (TCHAR const *)NANDDISKPath, 1) != FR_OK)
        {
            /* FatFs Initialization Error */
            Error_Handler();
        }
        else
        {
            printf_safe("\nmount ok.\n");
        }
    }
    else if(strcmp(argv[1], "--help") == 0)
    {

        printf_safe("\neg:mount nand\n");

    }
    taskEXIT_CRITICAL();
}

static void cli_umount_fnt(int argc, char **argv)
{
    taskENTER_CRITICAL();
    if(strcasecmp(argv[1], "nand") == 0)
    {
        if(f_mount(NULL, (TCHAR const *)NANDDISKPath, 0) != FR_OK)
        {
            /* FatFs Initialization Error */
            Error_Handler();
        }
        else
        {
            printf_safe("\numount ok.\n");
        }
    }
    else if(strcmp(argv[1], "--help") == 0)
    {

        printf_safe("\neg:umount nand\n");

    }
    taskEXIT_CRITICAL();
    /*##-11- Unlink the RAM disk I/O driver ####################################*/
    //FATFS_UnLinkDriver(NANDDISKPath);
}


static void cli_testfatfs_fnt(int argc, char **argv)
{
    UINT byteswritten, bytesread;                     /* File write/read counts */
    uint8_t wtext[200] = "This is z working with FatFs"; /* File write buffer */
    uint8_t *rtext;                                   /* File read buffer */
    rtext = malloc(200);
    memset(rtext, 0, 200);
    /*##-4- Create and Open a new text file object with write access #####*/
    if(f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        /* 'STM32.TXT' file Open for write Error */
        Error_Handler();
    }
    else
    {

        f_write(&MyFile, wtext, strlen((char *)wtext), &byteswritten);

        f_close(&MyFile);

        if(f_open(&MyFile, "STM32.TXT", FA_READ) != FR_OK)
        {
            Error_Handler();
        }
        else
        {
            f_read(&MyFile, rtext, f_size(&MyFile), &bytesread);

            {
                f_close(&MyFile);
                taskENTER_CRITICAL();
                printf_safe("%s\n", rtext);
                free(rtext);
                taskEXIT_CRITICAL();
            }
        }
    }
}

extern int f_getc( FIL *fp);
static void cli_cat_fnt(int argc, char **argv)
{
    FRESULT res;                                          /* FatFs function common result code */
    int c;
    taskENTER_CRITICAL();

    if(argc == 2)
    {
        res = f_open(&MyFile, argv[1], FA_READ);
        if(res != FR_OK)
        {
            printf_safe("Error file\n");
        }
        else
        {
            c = f_getc(&MyFile);
            while(c != EOF)
            {
                printf_safe("%c", c);
                c = f_getc(&MyFile);
            }
            if (f_eof(&MyFile) != 0)
            {
                printf_safe("\n End of file reached.\n");
            }
            else
            {
                printf_safe("\n Something went wrong.\n");
            }
        }
    }
    else
    {
        printf_safe("no file input\n");
    }



    taskEXIT_CRITICAL();
}

tinysh_cmd_t cli_cat_cmd = {0, "cat", "fatfs 文件显示", "[file]",
                            cli_cat_fnt, 0, 0, 0
                           };
tinysh_cmd_t cli_fatfs_cmd = {0, "testfatfs", "fatfs 测试", "[none]",
                              cli_testfatfs_fnt, 0, 0, 0
                             };
tinysh_cmd_t cli_mkfs_cmd = {0, "mkfs", "制作文件系统, 使用前请查看帮助 --help", "--help",
                             cli_mkfs_fnt, 0, 0, 0
                            };
tinysh_cmd_t cli_mount_cmd = {0, "mount", "挂在磁盘, 使用前请查看帮助 --help", "--help",
                              cli_mount_fnt, 0, 0, 0
                             };
tinysh_cmd_t cli_umount_cmd = {0, "umount", "卸载磁盘, 使用前请查看帮助 --help", "--help",
                               cli_umount_fnt, 0, 0, 0
                              };

#include "includes.h"
#include "tinysh.h"
#include "cli_app.h"
#include "cli_rtos.h"
#include "cli_main.h"
#include "bsp_uart.h"
#include "userlib_queue.h"

int cli_huart = -1;

void tinysh_char_out(unsigned char c)
{
//    uart_write_fast(cli_huart, &c, 1);//为确保线程安全，不要直接使用串口输出。
    printf_safe("%c", c);
}

void cli_init(void)
{
    cli_huart = uart_open(CLI_UARTx, CLI_UART_BPS);
#if BOOTLOADER
    tinysh_set_prompt("\n[bootldr]$ ");
#else
    tinysh_set_prompt("\n[charger]$ ");
#endif
    /************系统信息****************/
    tinysh_add_command(&cli_hello_cmd);
    tinysh_add_command(&cli_reboot_cmd);
    tinysh_add_command(&cli_systemdate_cmd);
    tinysh_add_command(&cli_tasklist_cmd);
    tinysh_add_command(&cli_taskstack_cmd);
#if BOOTLOADER
    tinysh_add_command(&cli_exit_cmd);
#else
#endif

    /************磁盘管理****************/

//    tinysh_add_command(&cli_mkfs_cmd);
//    tinysh_add_command(&cli_mount_cmd);
//    tinysh_add_command(&cli_umount_cmd);
//    tinysh_add_command(&cli_cat_cmd);
    tinysh_add_command(&cli_ls_cmd);
    tinysh_add_command(&cli_file_cmd);
    tinysh_add_command(&cli_cat_cmd);
    tinysh_add_command(&cli_rm_cmd);
    tinysh_add_command(&cli_ymodem_cmd);
#if BOOTLOADER
    tinysh_add_command(&cli_format_cmd);
    tinysh_add_command(&cli_set_crc32_cmd);
#else
    tinysh_add_command(&cli_ftpserver_cmd);
#endif
//    tinysh_add_command(&cli_unzip_cmd);
//    tinysh_add_command(&cli_zip_cmd);

    /************系统测试****************/
//    tinysh_add_command(&cli_fatfs_cmd);
//    tinysh_add_command(&cli_testsdram_cmd);
//    tinysh_add_command(&cli_parseJson_cmd);
//    tinysh_add_command(&cli_makeJson_cmd);
//    tinysh_add_command(&cli_aestest_cmd);
//    tinysh_add_command(&cli_yaffs2test_cmd);
//    tinysh_add_command(&cli_crctest_cmd);
    /************电桩信息****************/
#if BOOTLOADER
#else
    tinysh_add_command(&cli_evseinfo_cmd);
    tinysh_add_command(&cli_evseorder_cmd);
    tinysh_add_command(&cli_evsestatus_cmd);
    tinysh_add_command(&cli_modeminfo_cmd);
    tinysh_add_command(&cli_protoinfo_cmd);
    tinysh_add_command(&cli_evselog_cmd);
    tinysh_add_command(&cli_networkinfo_cmd);
    tinysh_add_command(&cli_printlog_cmd);
    tinysh_add_command(&cli_setload_cmd);
#endif
    /************设备测试****************/
//    tinysh_add_command(&cli_gprs_cmd);

    /* add the foo command
    */
//    tinysh_add_command(&myfoocmd);

    /* add sub commands
     */
//    tinysh_add_command(&ctxcmd);
//    tinysh_add_command(&item1);
//    tinysh_add_command(&item2);

    /* add atoxi support test command */
//    tinysh_add_command(&atoxi_cmd);

}

void cli_main(void)
{
    uint8_t ch[1];
    uint32_t l;

    while(1)
    {
        l = uart_read_fast(cli_huart, ch, 1);
        if(l == 1)
        {
            tinysh_char_in(ch[0]);
        }
        vTaskDelay(50);
    }
}


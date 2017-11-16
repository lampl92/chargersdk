#include "includes.h"
#include "tinysh.h"
#include "cli_app.h"
#include "cli_rtos.h"
#include "cli_main.h"
#include "bsp_uart.h"
#include "userlib_queue.h"

void tinysh_char_out(unsigned char c)
{
    //xputc(c);
    printf_safe("%c",c);//为确保线程安全，不要直接使用串口输出。
}


void cli_init(void)
{
    tinysh_set_prompt("\n[charger]$ ");
    /************系统信息****************/
    tinysh_add_command(&cli_hello_cmd);
    tinysh_add_command(&cli_systemdate_cmd);
    tinysh_add_command(&cli_tasklist_cmd);

    /************磁盘管理****************/

//    tinysh_add_command(&cli_mkfs_cmd);
//    tinysh_add_command(&cli_mount_cmd);
//    tinysh_add_command(&cli_umount_cmd);
//    tinysh_add_command(&cli_cat_cmd);
    tinysh_add_command(&cli_xmodem_cmd);
    

    /************系统测试****************/
//    tinysh_add_command(&cli_fatfs_cmd);
//    tinysh_add_command(&cli_testsdram_cmd);
//    tinysh_add_command(&cli_parseJson_cmd);
//    tinysh_add_command(&cli_makeJson_cmd);
//    tinysh_add_command(&cli_aestest_cmd);
//    tinysh_add_command(&cli_yaffs2test_cmd);
//    tinysh_add_command(&cli_crctest_cmd);
    /************电桩信息****************/
    tinysh_add_command(&cli_evseinfo_cmd);
    tinysh_add_command(&cli_evseorder_cmd);
    tinysh_add_command(&cli_evsestatus_cmd);
    tinysh_add_command(&cli_modeminfo_cmd);
    tinysh_add_command(&cli_protoinfo_cmd);
    tinysh_add_command(&cli_evselog_cmd);
    /************设备测试****************/
//    tinysh_add_command(&cli_gprs_cmd);

    /* add the foo command
    */
    tinysh_add_command(&myfoocmd);

    /* add sub commands
     */
    tinysh_add_command(&ctxcmd);
    tinysh_add_command(&item1);
    tinysh_add_command(&item2);

    /* add atoxi support test command */
    tinysh_add_command(&atoxi_cmd);

}

void cli_main(void)
{
    uint8_t ch[1];
    uint32_t l;
    cli_init();
    while(1)
    {
        l = uart_read(UART_PORT_CLI, ch, 1, 1);
        if(l == 1)
        {
            tinysh_char_in(ch[0]);
        }
        vTaskDelay(200);
    }
}


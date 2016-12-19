#include "includes.h"
#include "tinysh.h"
#include "cli_app.h"
#include "cli_rtos.h"
#include "cli_fatfs.h"
#include "cli_xml.h"
#include "bsp_uart.h"
#include "userlib_queue.h"
extern uint8_t aCliRxBuffer[1];
extern uint16_t CLI_RX_STA;

extern void myputc(uint8_t ch);
void tinysh_char_out(unsigned char c)
{
    myputc(c);
}


void cli_init(void)
{
    tinysh_set_prompt("\n[charger]$ ");
    /************系统信息****************/
    tinysh_add_command(&cli_hello_cmd);
    tinysh_add_command(&cli_systemdate_cmd);
    tinysh_add_command(&cli_tasklist_cmd);

    /************磁盘管理****************/

    tinysh_add_command(&cli_mkfs_cmd);
    tinysh_add_command(&cli_mount_cmd);
    tinysh_add_command(&cli_umount_cmd);
    tinysh_add_command(&cli_cat_cmd);

    /************系统测试****************/
    tinysh_add_command(&cli_fatfs_cmd);
    tinysh_add_command(&cli_testxml_cmd);
    tinysh_add_command(&cli_testxml_create_cmd);
    tinysh_add_command(&cli_testsdram_cmd);

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
extern Queue *pCliRecvQue;
void cli_main(void)
{
    uint8_t ch;
    int8_t res;
    cli_init();
    while(1)
    {
        res = readRecvQue(pCliRecvQue, &ch, 1);
        if(res == 0)
        {
            tinysh_char_in(ch);
        }
//      tinysh_char_in((unsigned char)getchar());
        vTaskDelay(100);
    }
}


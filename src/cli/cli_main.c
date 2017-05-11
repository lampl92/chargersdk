#include "includes.h"
#include "tinysh.h"
#include "cli_app.h"
#include "cli_rtos.h"
#include "cli_fatfs.h"
#include "cli_xml.h"
#include "cli_main.h"
#include "bsp_uart.h"
#include "userlib_queue.h"

void tinysh_char_out(unsigned char c)
{
    //xputc(c);
    printf_safe("%c",c);//Ϊȷ���̰߳�ȫ����Ҫֱ��ʹ�ô��������
}


void cli_init(void)
{
    tinysh_set_prompt("\n[charger]$ ");
    /************ϵͳ��Ϣ****************/
    tinysh_add_command(&cli_hello_cmd);
    tinysh_add_command(&cli_systemdate_cmd);
    tinysh_add_command(&cli_tasklist_cmd);

    /************���̹���****************/

    tinysh_add_command(&cli_mkfs_cmd);
    tinysh_add_command(&cli_mount_cmd);
    tinysh_add_command(&cli_umount_cmd);
    tinysh_add_command(&cli_cat_cmd);

    /************ϵͳ����****************/
    tinysh_add_command(&cli_fatfs_cmd);
    tinysh_add_command(&cli_testsdram_cmd);
    tinysh_add_command(&cli_parseJson_cmd);
    tinysh_add_command(&cli_makeJson_cmd);
    tinysh_add_command(&cli_aestest_cmd);
    /************��׮��Ϣ****************/
    tinysh_add_command(&cli_evseinfo_cmd);
    tinysh_add_command(&cli_evseorder_cmd);
    tinysh_add_command(&cli_evsestatus_cmd);
    /************�豸����****************/
    tinysh_add_command(&cli_gprs_cmd);

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
    uint8_t ch[1];
    uint32_t l;
    int8_t res;
    cli_init();
    while(1)
    {
        res = readRecvQueEx(pCliRecvQue, ch, 1, &l);
//        res = readRecvQue(pCliRecvQue, &ch, 1);
        if(res == 1)
        {
            tinysh_char_in(ch[0]);
        }
        vTaskDelay(100);
    }
}


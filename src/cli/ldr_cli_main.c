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

extern tinysh_cmd_t cli_exit_cmd;
void cli_init(void)
{
    tinysh_set_prompt("\n[bootldr]$ ");
    /************系统信息****************/
    tinysh_add_command(&cli_hello_cmd);
    tinysh_add_command(&cli_systemdate_cmd);
    tinysh_add_command(&cli_tasklist_cmd);
    tinysh_add_command(&cli_exit_cmd);

    /************磁盘管理****************/
    tinysh_add_command(&cli_ls_cmd);
    tinysh_add_command(&cli_file_cmd);
    tinysh_add_command(&cli_cat_cmd);
    tinysh_add_command(&cli_rm_cmd);
    tinysh_add_command(&cli_ymodem_cmd);
    tinysh_add_command(&cli_reboot_cmd);
    tinysh_add_command(&cli_format_cmd);
    tinysh_add_command(&cli_set_crc32_cmd);
    
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

